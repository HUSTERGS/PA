#include "cpu/exec.h"

make_EHelper(auipc) {
  rtl_add(&id_dest->val, &cpu.pc, &id_src->val);
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  print_asm_template2(auipc);
}

/*
 *  beq  000 RELOP_EQ
 *  bne  001 RELOP_NE
 *       010
 *       011
 *  blt  100 RELOP_LT
 *  bge  101 RELOP_GE
 *  bltu 110 RELOP_LTU
 *  bgeu 111 RELOP_GEU
 */
static uint32_t branch_table[8] = {
    RELOP_EQ, RELOP_NE, RELOP_FALSE, RELOP_FALSE, RELOP_LT, RELOP_GE, RELOP_LTU, RELOP_GEU
};

make_EHelper(branch) {
    rtl_jrelop(branch_table[decinfo.isa.instr.funct3], &id_src->val, &id_src2->val, decinfo.jmp_pc);
    
    switch (branch_table[decinfo.isa.instr.funct3]) {
        case RELOP_EQ: print_asm_template3(beq); break;
        case RELOP_NE: print_asm_template3(bne); break;
        case RELOP_LT: print_asm_template3(blt); break;
        case RELOP_GE: print_asm_template3(bge); break;
        case RELOP_LTU: print_asm_template3(bltu); break;
        case RELOP_GEU: print_asm_template3(bgeu); break;
        default: assert(0);
    }
}

make_EHelper(jal) {
   t0 = cpu.pc + 4;
   rtl_sr(id_dest->reg, &t0, 4);
   rtl_add(&decinfo.jmp_pc, &id_src->val, &cpu.pc);
   interpret_rtl_j(decinfo.jmp_pc); 
   print_asm_template2(jal);
}


make_EHelper(jalr) {

    // TODO: 这个地方并没有完全按照手册来执行
    t0 = cpu.pc + 4;
    rtl_sr(id_dest->reg, &t0, 4);
    rtl_add(&decinfo.jmp_pc, &id_src->val, &id_src2->val);    
    interpret_rtl_j(decinfo.jmp_pc);

    print_asm_template2(jalr);   
    difftest_skip_dut(1, 2);
}