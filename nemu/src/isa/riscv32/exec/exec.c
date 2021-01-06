#include "cpu/exec.h"
#include "all-instr.h"
/**
 * load 指令对应的opcode为0000011，也就是opcode_table的第一个
 * IDEX(ld, load)
 * 根据14-12位再进行细分，分为5种情况
 * lb  000 以下两个进行相同的处理，因为需要进行符号位拓展
 * lh  001
 * lw  010 以下都可以看作lw的拓展，因为都是进行0拓展？只是位数不同，所以都用ld进行处理
 *  -  011
 * lbu 100
 * lhu 101
 */
static OpcodeEntry load_table [8] = {
  EXW(lb_lh, 1), EXW(lb_lh, 2), EXW(ld, 4), EMPTY, EXW(ld, 1), EXW(ld, 2), EMPTY, EMPTY
};

static make_EHelper(load) {
  decinfo.width = load_table[decinfo.isa.instr.funct3].width;
  idex(pc, &load_table[decinfo.isa.instr.funct3]);
}
/**
 * sb 000
 * sh 001
 * sw 010
 */ 
static OpcodeEntry store_table [8] = {
  EXW(st, 1), EXW(st, 2), EXW(st, 4), EMPTY, EMPTY, EMPTY, EMPTY, EMPTY
};

static make_EHelper(store) {
  decinfo.width = store_table[decinfo.isa.instr.funct3].width;
  idex(pc, &store_table[decinfo.isa.instr.funct3]);
}

/**
 * addi  000
 * slli  001 *
 * slti  010
 * sltiu 011
 * xori  100
 * 
 * srli  101 **
 * srai  101 **
 * 
 * ori   110
 * andi  111
 */

static OpcodeEntry imm_table [8] = {
  EX(addi), EX(slli), EX(slti), EX(sltiu), EX(xori), EX(sr_li_ai), EX(ori), EX(andi),
};


static make_EHelper(imm) {
  // 其实可以不需要设置width？
  decinfo.width = imm_table[decinfo.isa.instr.funct3].width;
  idex(pc, &imm_table[decinfo.isa.instr.funct3]);
}

/**
 * add, sub 000
 * sll      001
 * slt      010
 * sltu     011
 * xor      100
 * srl, sra 101
 * or       110
 * and      111
 */
static OpcodeEntry r_table [8] = {
  EX(add_sub), EX(sll), EX(slt), EX(sltu), EX(xor), EX(slr_sra), EX(or), EX(and)
};

static make_EHelper(r) {
  decinfo.width = imm_table[decinfo.isa.instr.funct3].width;
  idex(pc, &imm_table[decinfo.isa.instr.funct3]);
}

/**
 * 由于所有的opcode的最后两位都是11，所以可以直接根据6-2共5位来进行索引，也就是2 ^ 5 = 32
 */
static OpcodeEntry opcode_table [32] = {
  /* b00 */ IDEX(ld, load), EMPTY, EMPTY, EMPTY, IDEX(I, imm), IDEX(U, auipc), EMPTY, EMPTY,
  /* b01 */ IDEX(st, store), EMPTY, EMPTY, EMPTY, IDEX(R, r), IDEX(U, lui), EMPTY, EMPTY,
  /* b10 */ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
  /* b11 */ IDEX(B, branch), IDEX(I, jalr), EX(nemu_trap), IDEX(J, jal), EMPTY, EMPTY, EMPTY, EMPTY,
};

void isa_exec(vaddr_t *pc) {
  decinfo.isa.instr.val = instr_fetch(pc, 4);
  // Log("pc = 0x%08x", decinfo.isa.instr.val);
  assert(decinfo.isa.instr.opcode1_0 == 0x3);
  idex(pc, &opcode_table[decinfo.isa.instr.opcode6_2]);
}

