#include <am.h>
#include <klib-macros.h>

void __am_timer_init(void);
void __am_timer_rtc(AM_TIMER_RTC_T *rtc);
void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime);

typedef void (*handler_t)(void *buf);

static void timer_config(AM_TIMER_CONFIG_T *cfg) {
  cfg->present = true;
  cfg->has_rtc = false;
}

static void fail(void *buf) {
  (void)buf;
  panic("access nonexist register");
}

static handler_t lut[128] = {
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
