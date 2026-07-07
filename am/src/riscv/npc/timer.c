#include <am.h>
#include <riscv/riscv.h>

#define RTC_ADDR 0xa0000048u
#define NPC_CLINT_FREQ_HZ 500000000ull

void __am_timer_init() {
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint32_t hi1, hi2, lo;

  do {
    hi1 = inl(RTC_ADDR + 4);
    lo = inl(RTC_ADDR);
    hi2 = inl(RTC_ADDR + 4);
  } while (hi1 != hi2);

  uint64_t ticks = ((uint64_t)hi2 << 32) | lo;
  uptime->us = ticks * 1000000ull / NPC_CLINT_FREQ_HZ;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
