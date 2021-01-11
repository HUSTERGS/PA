#include "common.h"
#include "syscall.h"

_Context* do_syscall(_Context *c) {
  Log("进入do_syscall");
  uintptr_t a[4];
  a[0] = c->GPR1;
  // 针对不同的系统调用号来进行分发
  // GPR1 调用类型
  // GPR2 - 4 arg0 - arg3
  // GPRx 返回值
  switch (a[0]) {
    case SYS_yield:  _yield();c->GPRx = 0; break;
    case SYS_exit: _halt(0); c->GPRx = 0; break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }


  return NULL;
}
