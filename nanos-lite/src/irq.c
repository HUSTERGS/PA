#include "common.h"
_Context* do_syscall(_Context *c);
static _Context* do_event(_Event e, _Context* c) {
  Log("进入了do_event函数，event类型为%d", e.event);
  switch (e.event) {
    case _EVENT_YIELD   : printf("self int\n");  break;
    case _EVENT_SYSCALL : do_syscall(c);         break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  // 注册了
  _cte_init(do_event);
}
