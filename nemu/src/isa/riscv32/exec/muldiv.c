#include "cpu/exec.h"

make_EHelper(mul) {
    rtl_imul_lo(&id_dest->val, &id_src->val, &id_src2->val);
    rtl_sr(id_dest->reg, &id_dest->val, 4);
    print_asm_template3(mul);
}
make_EHelper(mulh) {
    // 有符号高位乘法
    rtl_imul_hi(&id_dest->val, &id_src->val, &id_src2->val);
    rtl_sr(id_dest->reg, &id_dest->val, 4);
    print_asm_template3(mulh);
}
make_EHelper(mulhsu) {
    rtl_mul_hsu(&id_dest->val, &id_src->val, &id_src2->val);
    rtl_sr(id_dest->reg, &id_dest->val, 4);
    print_asm_template3(mulhsu);
}

make_EHelper(mulhu) {
    rtl_mul_hi(&id_dest->val, &id_src->val, &id_src2->val);
    rtl_sr(id_dest->reg, &id_dest->val, 4);
    print_asm_template3(mulhu);
}
make_EHelper(div) {
    rtl_idiv_q(&id_dest->val, &id_src->val, &id_src2->val);
    rtl_sr(id_dest->reg, &id_dest->val, 4);
    print_asm_template3(div);
}

make_EHelper(divu) {
    rtl_div_q(&id_dest->val, &id_src->val, &id_src2->val);
    rtl_sr(id_dest->reg, &id_dest->val, 4);
    print_asm_template3(divu);
}

make_EHelper(rem) {
    rtl_idiv_r(&id_dest->val, &id_src->val, &id_src2->val);
    rtl_sr(id_dest->reg, &id_dest->val, 4);
    print_asm_template3(divu);
}

make_EHelper(remu) {
    rtl_div_r(&id_dest->val, &id_src->val, &id_src2->val);
    rtl_sr(id_dest->reg, &id_dest->val, 4);
    print_asm_template3(divu);
}