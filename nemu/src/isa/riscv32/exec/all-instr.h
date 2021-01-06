#include "cpu/exec.h"

// 存放了所有的instruction的函数声明

make_EHelper(ld);
make_EHelper(st);

make_EHelper(inv);
make_EHelper(nemu_trap);


make_EHelper(lb_lh);

// I-imm
make_EHelper(addi);
make_EHelper(slti);
make_EHelper(sltiu);
make_EHelper(xori);
make_EHelper(ori);
make_EHelper(andi);
make_EHelper(slli);
make_EHelper(sr_li_ai);


// R
make_EHelper(add_sub);
make_EHelper(sll);
make_EHelper(slt);
make_EHelper(sltu);
make_EHelper(xor);
make_EHelper(slr_sra);
make_EHelper(or);
make_EHelper(and);

// U
make_EHelper(auipc);
make_EHelper(lui);


// J
make_EHelper(jalr);
make_EHelper(jal);

// B
make_EHelper(branch);