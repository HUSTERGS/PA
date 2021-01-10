#ifndef __ARCH_H__
#define __ARCH_H__

struct _Context {
  // 根据trap.S修改了顺序
  uintptr_t gpr[32], cause, status, epc;
  struct _AddressSpace *as;
};

#define GPR1 gpr[17]
#define GPR2 gpr[10]
#define GPR3 gpr[11]
#define GPR4 gpr[12]
#define GPRx gpr[10]

#endif
