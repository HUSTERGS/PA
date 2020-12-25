#include "nemu.h"
#include <stdlib.h>
#include <time.h>

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  uint32_t sample[8];
  uint32_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

void isa_reg_display() {
  // QUESTION: 为何这个地方会变红，因为引用的问题吗
  printf("eax: %08x\n", cpu.eax);
  printf("ebx: %08x\n", cpu.ebx);
  printf("ecx: %08x\n", cpu.ecx);
  printf("edx: %08x\n", cpu.edx);
  printf("ebp: %08x\n", cpu.ebp);
  printf("esi: %08x\n", cpu.esi);
  printf("edi: %08x\n", cpu.edi);
  printf("esp: %08x\n", cpu.esp);
  printf("pc: %08x\n", cpu.pc);
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
  *success = true;
  // 整个32位寄存器
  if (!strcmp(s, "eax")) {
    return cpu.eax;
  } else if (!strcmp(s, "ebx")) {
    return cpu.ebx;
  } else if (!strcmp(s, "ecx")) {
    return cpu.ecx;
  } else if (!strcmp(s, "edx")) {
    return cpu.edx;
  } else if (!strcmp(s, "ebp")) {
    return cpu.ebp;
  } else if (!strcmp(s, "esi")) {
    return cpu.esi;
  } else if (!strcmp(s, "edi")) {
    return cpu.edi;
  } else if (!strcmp(s, "esp")) {
    return cpu.esp;
  } else if (!strcmp(s, "ebx")) {
    return cpu.ebx;
  } else if (!strcmp(s, "pc")) {
    return cpu.pc;
  } 
  // 16位的寄存器
  if (!strcmp(s, "ax")) {
    return cpu.eax & 0xffff;
  } else if (!strcmp(s, "bx")) {
    return cpu.ebx & 0xffff;
  } else if (!strcmp(s, "cx")) {
    return cpu.ecx & 0xffff;
  } else if (!strcmp(s, "dx")) {
    return cpu.edx & 0xffff;
  } else if (!strcmp(s, "bp")) {
    return cpu.ebp & 0xffff;
  } else if (!strcmp(s, "si")) {
    return cpu.esi & 0xffff;
  } else if (!strcmp(s, "di")) {
    return cpu.edi & 0xffff;
  } else if (!strcmp(s, "sp")) {
    return cpu.esp & 0xffff;
  } else if (!strcmp(s, "bx")) {
    return cpu.ebx & 0xffff;
  }

  // 高8位
  if (!strcmp(s, "ah")) {
    return (cpu.eax >> 8) & 0xff;
  } else if (!strcmp(s, "bh")) {
    return (cpu.ebx >> 8) & 0xff;
  } else if (!strcmp(s, "ch")) {
    return (cpu.ecx >> 8) & 0xff;
  } else if (!strcmp(s, "dh")) {
    return (cpu.edx >> 8) & 0xff;
  } 

  // 低八位
  if (!strcmp(s, "al")) {
    return cpu.eax & 0xff;
  } else if (!strcmp(s, "bl")) {
    return cpu.ebx & 0xff;
  } else if (!strcmp(s, "cl")) {
    return cpu.ecx & 0xff;
  } else if (!strcmp(s, "dl")) {
    return cpu.edx & 0xff;
  } 
  *success = false;
  return 0;
}
