#include "cpu/exec.h"



/************ I instructions ************/

make_EHelper(addi) {
  // 将计算的结果保存在目标寄存器的val字段中，然后写入reg
  rtl_add(&id_dest->val, &id_src->val, &id_src2->val);  
  rtl_sr(id_dest->reg, &id_dest->val, 4); 
  print_asm_template2(addi);
}

make_EHelper(slti) {
  rtl_setrelop(RELOP_LT, &id_dest->val, &id_src->val, &id_src2->val);
  // rtl_li(&id_dest->val, interpret_relop(RELOP_LT, id_src->val, id_src2->val));
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  print_asm_template2(slti);
}

make_EHelper(sltiu) {
  rtl_setrelop(RELOP_LTU, &id_dest->val, &id_src->val, &id_src2->val);
  // rtl_li(&id_dest->val, interpret_relop(RELOP_LTU, id_src->val, id_src2->val));
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  print_asm_template2(sltiu);
}

make_EHelper(xori) {
  rtl_xor(&id_dest->val, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  print_asm_template2(xori);
}

make_EHelper(ori) {
  rtl_or(&id_dest->val, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  print_asm_template2(ori);
}

make_EHelper(andi) {
  rtl_and(&id_dest->val, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  print_asm_template2(andi);
}

make_EHelper(slli) {
  rtl_shl(&id_dest->val, &id_src->val, &id_src2->reg);
  rtl_sr(id_dest->reg, &id_dest->val, 4);
  print_asm_template2(slli); 
}

make_EHelper(sr_li_ai) {
  switch(decinfo.isa.instr.funct7) {
    case 0b0000000:  
      // srli 
      rtl_shr(&id_dest->val, &id_src->val, &id_src2->reg);
      print_asm_template2(srli); 
      break;
    
    case 0b0100000: 
      // srai
      rtl_sar(&id_dest->val, &id_src->val, &id_src2->reg);
      print_asm_template2(srai);
      break;
    default: 
      assert(0);
  }
  
  rtl_sr(id_dest->reg, &id_dest->val, 4);
}


/************ R instructions ************/

make_EHelper(add_sub) {
  switch(decinfo.isa.instr.funct7) {
    case 0b0000000: 
      // add
      rtl_add(&id_dest->val, &id_src->val, &id_src2->val); 
      print_asm_template3(add);
      break;

    case 0b0100000: 
      // sub
      rtl_sub(&id_dest->val, &id_src->val, &id_src2->val);
      print_asm_template3(sub);
      break;
    default: 
      assert(0);
  }
  rtl_sr(id_dest->reg, &id_dest->val, 4);
}

make_EHelper(sll) {
  rtl_shl(&id_dest->val, &id_src->val, &id_src2->val);
  print_asm_template3(sll); 
  rtl_sr(id_dest->reg, &id_dest->val, 4);
}

// TODO: 这两个函数是否可以直接一步到位？不用先写入val成员？
make_EHelper(slt) {
  rtl_setrelop(RELOP_LT, &id_dest->val, &id_src->val, &id_src2->val);
  // rtl_li(&id_dest->val, interpret_relop(RELOP_LT, id_src->val, id_src2->val));
  print_asm_template3(slt);
  rtl_sr(id_dest->reg, &id_dest->val, 4);
}

make_EHelper(sltu) {
  rtl_setrelop(RELOP_LTU, &id_dest->val, &id_src->val, &id_src2->val);
  // rtl_li(&id_dest->val, interpret_relop(RELOP_LTU, id_src->val, id_src2->val));
  print_asm_template3(sltu); 
  rtl_sr(id_dest->reg, &id_dest->val, 4);
}

make_EHelper(xor) {
  rtl_xor(&id_dest->val, &id_src->val, &id_src2->val);
  print_asm_template3(xor);
  rtl_sr(id_dest->reg, &id_dest->val, 4);
}

make_EHelper(slr_sra) {
  switch (decinfo.isa.instr.funct7)
  {
  case 0b0000000:
    // slr
    rtl_shr(&id_dest->val, &id_src->val, &id_src2->val);
    print_asm_template3(srl); 
    break;
  case 0b0100000:
    // sra
    rtl_sar(&id_dest->val, &id_src->val, &id_src2->val);
    print_asm_template3(sra);
    break;
  default:
    assert(0);
  }
  rtl_sr(id_dest->reg, &id_dest->val, 4);
}

make_EHelper(or) {
  rtl_or(&id_dest->val, &id_src->val, &id_src2->val);
  rtl_sr(id_dest->reg, &id_dest->val, 4);

  print_asm_template3(or);
}

make_EHelper(and) {
  rtl_and(&id_dest->val, &id_src->val, &id_src2->val);
  print_asm_template3(and);
  rtl_sr(id_dest->reg, &id_dest->val, 4);
}