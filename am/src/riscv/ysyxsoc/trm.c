#include <am.h>
#include <stdint.h>

#define UART_TX ((volatile uint8_t *)0x10000000u)

#define UART_RBR 0  // Receive buffer
#define UART_THR 0  // Transmit holding register
#define UART_IER 1  // Interrupt enable / divisor high
#define UART_FCR 2  // FIFO control
#define UART_LCR 3  // Line control
#define UART_LSR 5  // Line status

#define UART_LCR_DLAB 0x80
#define UART_LCR_8N1  0x03
#define UART_LSR_THRE  0x20

static volatile uint8_t *uart_reg(uint32_t offset) {
  return (volatile uint8_t *)(UART_TX + offset);
}

extern char _heap_start;
extern char _heap_end;

#ifdef YSYXSOC_REPORT_MEMORY_LAYOUT
extern char _data_start;
extern char _data_end;
extern char _bss_start;
extern char _bss_end;
extern char _stack_bottom;
extern char _stack_pointer;
#endif

int main(const char *args);

#define YSYX_STRINGIFY_INNER(value) #value
#define YSYX_STRINGIFY(value) YSYX_STRINGIFY_INNER(value)
static const char mainargs[MAINARGS_MAX_LEN] =
    YSYX_STRINGIFY(MAINARGS_PLACEHOLDER);  // patched after objcopy

static void uart_init(void) {
  // Enable divisor-latch access.
  *uart_reg(UART_LCR) = UART_LCR_DLAB;

  // Divisor = 1: low byte at offset 0, high byte at offset 1.
  *uart_reg(UART_THR) = 1;
  *uart_reg(UART_IER) = 0;

  // Disable DLAB and select 8 data bits, no parity, one stop bit.
  *uart_reg(UART_LCR) = UART_LCR_8N1;

  // Clear receive and transmit FIFOs.
  *uart_reg(UART_FCR) = 0x07;
}

Area heap;

void putch(char ch) {
  while ((*uart_reg(UART_LSR) & UART_LSR_THRE) == 0) {
  }

  *UART_TX = (uint8_t)ch;
}

static void putstr(const char *s) {
  while (*s != '\0') putch(*s++);
}

static void puthex32(uint32_t value) {
  static const char digits[] = "0123456789abcdef";

  for (int shift = 28; shift >= 0; shift -= 4) {
    putch(digits[(value >> shift) & 0xf]);
  }
}

#ifdef YSYXSOC_REPORT_MEMORY_LAYOUT
static void putrange(const char *name, const char *start, const char *end) {
  putstr(name);
  putstr("=[0x");
  puthex32((uintptr_t)start);
  putstr(",0x");
  puthex32((uintptr_t)end);
  putch(')');
}

static void print_memory_layout(void) {
  putstr("MEMORY ");
  putrange("data", &_data_start, &_data_end);
  putch(' ');
  putrange("bss", &_bss_start, &_bss_end);
  putch(' ');
  putrange("heap", &_heap_start, &_heap_end);
  putch(' ');
  putrange("stack", &_stack_bottom, &_stack_pointer);
  putch('\n');
}
#endif

static uint32_t read_mvendorid(void) {
  uint32_t value;
  asm volatile("csrr %0, 0xf11" : "=r"(value));
  return value;
}

static uint32_t read_marchid(void) {
  uint32_t value;
  asm volatile("csrr %0, 0xf12" : "=r"(value));
  return value;
}

static void print_cpu_identity(void) {
  putstr("mvendorid = ");
  puthex32(read_mvendorid());
  putch('\n');
  putstr("marchid   = ");
  puthex32(read_marchid());
  putch('\n');
}

void halt(int code) {
  putch(code == 0 ? 'G' : 'B');
  putch('\n');

  asm volatile("mv a0, %0; ebreak" : : "r"(code));

  while (1) {
  }
}

void _trm_init() {
  uart_init();
  print_cpu_identity();

  heap.start = &_heap_start;
  heap.end = &_heap_end;

#ifdef YSYXSOC_REPORT_MEMORY_LAYOUT
  print_memory_layout();
#endif

  int ret = main(mainargs);
  halt(ret);
}
