#include "cpu/exec.h"

CPU_state cpu;

/* 
 * ir(immediate register) - 只能作为rtl_li的目的RTL寄存器
 * t0,t1 - 只能在RTL伪指令的实现过程中存放中间结果
 * s0,s1 - 只能在译码辅助函数和执行辅助函数的实现过程存放中间结果
 */
rtlreg_t s0, s1, t0, t1, ir;

/* shared by all helper functions */
DecodeInfo decinfo;

void decinfo_set_jmp(bool is_jmp) {
  decinfo.is_jmp = is_jmp;
}

void isa_exec(vaddr_t *pc);

vaddr_t exec_once(void) {
  decinfo.seq_pc = cpu.pc;
  isa_exec(&decinfo.seq_pc);
  update_pc();

  return decinfo.seq_pc;
}
