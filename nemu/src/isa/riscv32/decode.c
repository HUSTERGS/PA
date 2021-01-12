#include "cpu/decode.h"
#include "rtl/rtl.h"

/**
 * 对temp进行符号拓展
 */
static inline void sext(rtlreg_t *temp, uint32_t width) {
  uint32_t shift = 32 - width;
  int32_t num = *temp;
  num = (num) << shift;
  num = (num) >> shift;
  *temp = num;
}

// decode operand helper
#define make_DopHelper(name) void concat(decode_op_, name) (Operand *op, uint32_t val, bool load_val)

// 这个函数的load_val的实际上并没有起作用
static inline make_DopHelper(i) {
  op->type = OP_TYPE_IMM;
  op->imm = val;
  rtl_li(&op->val, op->imm);

  print_Dop(op->str, OP_STR_SIZE, "%d", op->imm);
}

static inline make_DopHelper(r) {
  op->type = OP_TYPE_REG;
  op->reg = val;
  if (load_val) {
    rtl_lr(&op->val, op->reg, 4);
  }

  print_Dop(op->str, OP_STR_SIZE, "%s", reg_name(op->reg, 4));
}

make_DHelper(U) {
  decode_op_i(id_src, decinfo.isa.instr.imm31_12 << 12, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);

  print_Dop(id_src->str, OP_STR_SIZE, "0x%x", decinfo.isa.instr.imm31_12);
}

make_DHelper(ld) {
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_i(id_src2, decinfo.isa.instr.simm11_0, true);

  print_Dop(id_src->str, OP_STR_SIZE, "%d(%s)", id_src2->val, reg_name(id_src->reg, 4));

  rtl_add(&id_src->addr, &id_src->val, &id_src2->val);

  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
}

make_DHelper(st) {
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  int32_t simm = (decinfo.isa.instr.simm11_5 << 5) | decinfo.isa.instr.imm4_0;
  decode_op_i(id_src2, simm, true);

  print_Dop(id_src->str, OP_STR_SIZE, "%d(%s)", id_src2->val, reg_name(id_src->reg, 4));

  rtl_add(&id_src->addr, &id_src->val, &id_src2->val);

  decode_op_r(id_dest, decinfo.isa.instr.rs2, true);
}



make_DHelper(I) {
  // 通过左移右移来获得带符号(Signed Immidiate)的立即数
  int32_t simm = decinfo.isa.instr.simm11_0;
  sext(&simm, 12);
  // simm = simm << 20;
  // simm = simm >> 20;
  // 将指令涉及到的rs1(id_src)，imm(simm)以及rd(id_dest)放入对应的decinfo结构体中
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_i(id_src2, simm, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
  // 同时保存其字符串表示
  // TODO: 其实不是很清楚这个print_Dop的作用是干什么的
  // print_Dop(id_src->str, OP_STR_SIZE, "0x%x", decinfo.isa.instr.rs1);
  // print_Dop(id_src2->str, OP_STR_SIZE, "0x%x", decinfo.isa.instr.simm11_0);
  // TODO: I其实也包括了一些其他的指令，难道不需要在这里单独处理吗？
  
}

make_DHelper(B) {
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_r(id_src2, decinfo.isa.instr.rs2, true);

  t0 = (decinfo.isa.instr.simm12 << 12) | (decinfo.isa.instr.imm11 << 11) | 
       (decinfo.isa.instr.imm10_5 << 5) | (decinfo.isa.instr.imm4_1 << 1);
  // offset是13位的
  sext(&t0, 13);
  // 将jmp_pc设置为当前pc和offset(t0)的和
  decode_op_i(id_dest, t0, true);
  
  print_Dop(id_dest->str, OP_STR_SIZE, "0x%x", t0);
}

make_DHelper(J) {
  
  t0 =  (decinfo.isa.instr.simm20 << 20) | (decinfo.isa.instr.imm19_12 << 12) | 
        (decinfo.isa.instr.imm11_ << 11) | (decinfo.isa.instr.imm10_1 << 1);
  // imm是21位的
  sext(&t0, 21);
  
  decode_op_i(id_src , t0, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);

  print_Dop(id_src->str, OP_STR_SIZE, "0x%x", t0);
}

make_DHelper(R) {
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_r(id_src2, decinfo.isa.instr.rs2, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
}