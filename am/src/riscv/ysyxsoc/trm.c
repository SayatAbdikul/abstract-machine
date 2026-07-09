#include <am.h>
#include <stdint.h>

#define UART_TX ((volatile uint8_t *)0x10000000u)

extern char _heap_start;
extern char _heap_end;

int main(const char *args);

Area heap;

void putch(char ch) {
  *UART_TX = (uint8_t)ch;
}

void halt(int code) {
  putch(code == 0 ? 'G' : 'B');
  putch('\n');

  asm volatile("mv a0, %0; ebreak" : : "r"(code));

  while (1) {
  }
}

void _trm_init() {
  heap.start = &_heap_start;
  heap.end = &_heap_end;

  int ret = main("");
  halt(ret);
}
