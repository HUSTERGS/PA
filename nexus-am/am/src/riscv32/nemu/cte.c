#include <am.h>
#include <riscv32.h>
// #include "syscall.h"
static _Context* (*user_handler)(_Event, _Context*) = NULL;

_Context* __am_irq_handle(_Context *c) {
  // Log("进入了handle函数");
  // _halt(1);  
  _Context *next = c;
  if (user_handler) {
    _Event ev = {0};
    switch (c->cause) {
      case -1: ev.event = _EVENT_YIELD; break;
      case 1: ev.event = _EVENT_SYSCALL;break;
      default: ev.event = _EVENT_ERROR; break;
    }

    next = user_handler(ev, c);
    if (next == NULL) {
      next = c;
    }
  }

  return next;
}

extern void __am_asm_trap(void);

int _cte_init(_Context*(*handler)(_Event, _Context*)) {
  // initialize exception entry
  asm volatile("csrw stvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;
  
  return 0;
}

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
  return NULL;
}
// 通过ecall指令使得cpu自陷，将会跳转到ecall指令的执行
// 其中li a7, -1将a7设置为-1，表示当前的ecall类型是自陷
void _yield() {
  asm volatile("li a7, -1; ecall");
}

int _intr_read() {
  return 0;
}

void _intr_write(int enable) {
}
