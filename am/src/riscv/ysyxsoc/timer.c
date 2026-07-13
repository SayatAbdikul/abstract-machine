#include <am.h>
#include <stdint.h>

#define YSYXSOC_CLOCK_HZ 50000000ull

static uint64_t read_cycle(void) {
  uint32_t hi1, hi2, lo;

  do {
    asm volatile("csrr %0, cycleh" : "=r"(hi1));
    asm volatile("csrr %0, cycle" : "=r"(lo));
    asm volatile("csrr %0, cycleh" : "=r"(hi2));
  } while (hi1 != hi2);

  return ((uint64_t)hi2 << 32) | lo;
}

void __am_timer_init(void) {
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uptime->us = read_cycle() * 1000000ull / YSYXSOC_CLOCK_HZ;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour = 0;
  rtc->day = 0;
  rtc->month = 0;
  rtc->year = 1900;
}
