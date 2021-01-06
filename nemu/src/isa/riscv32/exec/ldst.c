#include "cpu/exec.h"
// 个人感觉lui应该是属于load store而不是属于compute所以就转移到这里了
make_EHelper(lui) {
  rtl_sr(id_dest->reg, &id_src->val, 4);
  print_asm_template2(lui);
}

make_EHelper(ld) {
  rtl_lm(&s0, &id_src->addr, decinfo.width);
  rtl_sr(id_dest->reg, &s0, 4);

  switch (decinfo.width) {
    case 4: print_asm_template2(lw); break;
    case 2: print_asm_template2(lhu); break;
    case 1: print_asm_template2(lbu); break;
    default: assert(0);
  }
}

make_EHelper(st) {
  rtl_sm(&id_src->addr, &id_dest->val, decinfo.width);

  switch (decinfo.width) {
    case 4: print_asm_template2(sw); break;
    case 2: print_asm_template2(sh); break;
    case 1: print_asm_template2(sb); break;
    default: assert(0);
  }
}

make_EHelper(lb_lh) {
  rtl_lm(&s0, &id_src->addr, decinfo.width);
  
  switch(decinfo.width) {
    case 2: rtl_sext(&s1, &s0, 2); print_asm_template2(lh);break;
    case 1: rtl_sext(&s1, &s0, 1); print_asm_template2(lb);break;
    default: assert(0);
  }
  rtl_sr(id_dest->reg, &s1, 4);  
}