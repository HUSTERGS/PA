#include "common.h"

void init_mm(void);
void init_ramdisk(void);
void init_device(void);
void init_irq(void);
void init_fs(void);
void init_proc(void);

extern const char logo[];

int main() {
  printf("%s", logo);
  Log("'Hello World!' from Nanos-lite");
  Log("Build time: %s, %s", __TIME__, __DATE__);

#ifdef HAS_VME
  init_mm();
#endif

  init_ramdisk();

  init_device();

#ifdef HAS_CTE
  init_irq();
#endif

  init_fs();

  init_proc();

  Log("Finish initialization");
/**
 * _yield -> 执行ecall指令 -> ecall_Helper
 * -> 调用raise_intr，NO设置为9？为啥，还是说其实是随便设置的，好像确实是随便设置的
 * 1. 保存当前的pc 到 SEPC
 * 2. NO保存到SCAUSE
 * 3. 读取stvec中保存的跳转目标位置
 * 4. 通过interpret_rtl_jr进行跳转
 * -> __am_asm_trap 汇编代码
 * -> 进入__am_irq_handle
 * 调用回调函数
 */
#ifdef HAS_CTE
  _yield();
#endif

  panic("Should not reach here");
}
