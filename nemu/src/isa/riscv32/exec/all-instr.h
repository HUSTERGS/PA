#include "cpu/exec.h"

make_EHelper(inv);
make_EHelper(nemu_trap);

//load store
make_EHelper(ld);
make_EHelper(st);
make_EHelper(lh);
make_EHelper(lb);

//U type
make_EHelper(lui);
make_EHelper(auipc);

//I type
make_EHelper(i_funct3);
make_EHelper(jalr);
make_EHelper(sys);

//J type
make_EHelper(jal);

//B type
make_EHelper(b_funct3);

//R type
make_EHelper(r_funct3);

