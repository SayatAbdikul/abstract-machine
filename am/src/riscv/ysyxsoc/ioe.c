#include <am.h>
#include <klib-macros.h>

#define UART_BASE 0x10000000u
#define UART_RBR  0
#define UART_THR  0
#define UART_LSR  5

#define UART_LSR_DR   0x01
#define UART_LSR_THRE 0x20

void __am_timer_init(void);
void __am_timer_rtc(AM_TIMER_RTC_T *rtc);
void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime);

typedef void (*handler_t)(void *buf);

static void timer_config(AM_TIMER_CONFIG_T *cfg) {
  cfg->present = true;
  cfg->has_rtc = false;
}

static volatile uint8_t *uart_reg(uint32_t offset) {
  return (volatile uint8_t *)(UART_BASE + offset);
}

static void uart_config(AM_UART_CONFIG_T *cfg) {
  cfg->present = true;
}

static void uart_tx(AM_UART_TX_T *tx) {
  while ((*uart_reg(UART_LSR) & UART_LSR_THRE) == 0) {
  }
  *uart_reg(UART_THR) = (uint8_t)tx->data;
}

static void uart_rx(AM_UART_RX_T *rx) {
  // Do not read RBR unless LSR says a byte is ready: an empty RBR read has
  // device-specific side effects. AM stores the sentinel in a signed `char`,
  // so callers must compare (uint8_t)rx.data with 0xff.
  rx->data = (*uart_reg(UART_LSR) & UART_LSR_DR)
                 ? (char)*uart_reg(UART_RBR)
                 : (char)0xff;
}

static void fail(void *buf) {
  (void)buf;
  panic("access nonexist register");
}

static handler_t lut[128] = {
  [AM_UART_CONFIG] = (handler_t)uart_config,
  [AM_UART_TX] = (handler_t)uart_tx,
  [AM_UART_RX] = (handler_t)uart_rx,
  [AM_TIMER_CONFIG] = (handler_t)timer_config,
  [AM_TIMER_RTC] = (handler_t)__am_timer_rtc,
  [AM_TIMER_UPTIME] = (handler_t)__am_timer_uptime,
};

bool ioe_init(void) {
  for (int i = 0; i < LENGTH(lut); i++) {
    if (lut[i] == NULL) lut[i] = fail;
  }
  __am_timer_init();
  return true;
}

void ioe_read(int reg, void *buf) {
  lut[reg](buf);
}

void ioe_write(int reg, void *buf) {
  lut[reg](buf);
}
