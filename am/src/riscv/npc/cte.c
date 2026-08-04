#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case 11:
        if (c->GPR1 == (uintptr_t)-1) {
          ev.event = EVENT_YIELD;
          c->mepc += 4;
        } else {
          ev.event = EVENT_SYSCALL;
        }
        break;
      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *c = (Context *)kstack.end - 1;
  memset(c, 0, sizeof(Context));

  c->mstatus = 0x1800;
  c->mepc = (uintptr_t)entry;
  c->gpr[10] = (uintptr_t)arg; // a0

  return c;
}

void yield() {
#ifdef __riscv_e
  asm volatile("li a5, -1; ecall");
#else
  asm volatile("li a7, -1; ecall");
#endif
}

// Both were stubs -- ienabled() hardcoded false and iset() a no-op -- which
// made cte-context-test fail its `iset(true); assert(ienabled())` before ever
// reaching its first ecall.  The riscv/ysyxsoc port has done this through
// mstatus.MIE since it was written and ysyxsoc-cte-context-test passes with
// it; this brings the npc port in line so the same test can run under npc's
// own standalone difftest, where it is what makes mepc and mcause comparable.
bool ienabled() {
  uintptr_t mstatus;
  asm volatile("csrr %0, mstatus" : "=r"(mstatus));
  return (mstatus & (1u << 3)) != 0;
}

void iset(bool enable) {
  uintptr_t mstatus;
  asm volatile("csrr %0, mstatus" : "=r"(mstatus));
  if (enable) mstatus |= 1u << 3;
  else mstatus &= ~(1u << 3);
  asm volatile("csrw mstatus, %0" : : "r"(mstatus));
}
