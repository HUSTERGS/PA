#include "nemu.h"
#include "monitor/diff-test.h"
#include "cpu/exec.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  for(int i=0;i<32;i++){
    if(reg_l(i)!=ref_r->gpr[i]._32){
      printf("In %s , NEMU: %08x , QEMU: %08x\n",reg_name(i,3),reg_l(i),ref_r->gpr[i]._32);
      return false;
    }
  }
  return true;
}

void isa_difftest_attach(void) {
}
