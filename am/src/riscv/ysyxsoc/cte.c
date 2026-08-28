#include <am.h>
#include <klib.h>
#include <riscv/riscv.h>

static Context *(*user_handler)(Event, Context *) = NULL;

Context *__am_irq_handle(Context *context) {
  Event event = { .event = EVENT_ERROR };

  if (context->mcause == 11) {
    if (context->GPR1 == (uintptr_t)-1) {
      event.event = EVENT_YIELD;
      context->mepc += 4;
    } else {
      event.event = EVENT_SYSCALL;
    }
  }

  if (user_handler != NULL) {
    context = user_handler(event, context);
    assert(context != NULL);
  }
  return context;
}

extern void __am_asm_trap(void);

bool cte_init(Context *(*handler)(Event, Context *)) {
  user_handler = handler;
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));
  return true;
}

Context *kcontext(Area stack, void (*entry)(void *), void *arg) {
  Context *context = (Context *)stack.end - 1;
  memset(context, 0, sizeof(*context));
  context->mstatus = 0x1800;
  context->mepc = (uintptr_t)entry;
  context->gpr[10] = (uintptr_t)arg;
  return context;
}

void yield(void) {
#ifdef __riscv_e
  asm volatile("li a5, -1; ecall");
#else
  asm volatile("li a7, -1; ecall");
#endif
}

bool ienabled(void) {
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
