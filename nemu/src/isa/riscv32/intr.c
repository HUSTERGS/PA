#include "rtl/rtl.h"

void raise_intr(uint32_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  /* 你可能会问：stvec的值是从哪里来的？
   * stvec寄存器用来存放异常入口地址的，也就是__am_asm_trap的地址，
   * 在操作系统初始化时，_cte_init()函数中就已经将__am_asm_trap的
   * 地址通过内敛函数传递给了stvec。
   */
  // 保存当前pc
  csr_write(SEPC, epc);  
  // 保存中断号
  csr_write(SCAUSE, NO); 

  // t0 = csr_read(STVEC);  
  // Log("stvec为%p", t0);
  interpret_rtl_j(csr_read(STVEC)); 
  // Log("reach this place");
  // assert(0);
}

bool isa_query_intr(void) {
  return false;
}
