#include "rtl/rtl.h"

void raise_intr(uint32_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  cpu.sepc=epc;
  cpu.scause=NO;
  rtl_j(cpu.stvec);
}

bool isa_query_intr(void) {
  return false;
}
