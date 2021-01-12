#ifndef __CPU_EXEC_H__
#define __CPU_EXEC_H__

#include "nemu.h"
#include "monitor/diff-test.h"
#include "rtl/rtl.h"

#define make_EHelper(name) void concat(exec_, name) (vaddr_t *pc)
typedef void (*EHelper) (vaddr_t *);

#include "cpu/decode.h"

typedef struct {
  DHelper decode;
  EHelper execute;
  int width;
} OpcodeEntry;

/**
 * 定义了一系列的结构体，结构体的成员分别是decode函数，exec函数以及宽度
 * 在idex函数中，由于参数是OpcodeEntry类型，所以似乎会进行某种意义上的自动嵌入？
 * 也就是这些结构体的成员自动匹配到OpcodeEntry的decode，execute以及width成员中
 * 
 * 有的指令不需要译码，所以对应的decode成员就是NULL
 * id，ex分别代表了decode以及execute函数的名字，比如IDEX(ld, load)就说明decode函数叫decode_ld，execute函数叫exec_load
 * 
 */
#define IDEXW(id, ex, w)   {concat(decode_, id), concat(exec_, ex), w}
#define IDEX(id, ex)       IDEXW(id, ex, 0)
#define EXW(ex, w)         {NULL, concat(exec_, ex), w}
#define EX(ex)             EXW(ex, 0)
#define EMPTY              EX(inv)

static inline uint32_t instr_fetch(vaddr_t *pc, int len) {
  uint32_t instr = vaddr_read(*pc, len);
#ifdef DEBUG
  uint8_t *p_instr = (void *)&instr;
  int i;
  for (i = 0; i < len; i ++) {
    extern char log_bytebuf[];
    strcatf(log_bytebuf, "%02x ", p_instr[i]);
  }
#endif
  (*pc) += len;
  return instr;
}

/* Instruction Decode and EXecute */
static inline void idex(vaddr_t *pc, OpcodeEntry *e) {
  if (e->decode)
    e->decode(pc);
  e->execute(pc);
}

static inline void update_pc(void) {
  if (decinfo.is_jmp) { 
    // decinfo.is_jmp = 0; 
    // cpu.pc = decinfo.jmp_pc;
    decinfo_set_jmp(false);
  }
  else { cpu.pc = decinfo.seq_pc; }
}

void display_inv_msg(vaddr_t pc);

#ifdef DEBUG
#define print_asm(...) \
  do { \
    extern char log_asmbuf[]; \
    strcatf(log_asmbuf, __VA_ARGS__); \
  } while (0)
#else
#define print_asm(...)
#endif

#ifndef suffix_char
#define suffix_char(width) ' '
#endif

#define print_asm_template1(instr) \
  print_asm(str(instr) "%c %s", suffix_char(id_dest->width), id_dest->str)

#define print_asm_template2(instr) \
  print_asm(str(instr) "%c %s,%s", suffix_char(id_dest->width), id_src->str, id_dest->str)

#define print_asm_template3(instr) \
  print_asm(str(instr) "%c %s,%s,%s", suffix_char(id_dest->width), id_src->str, id_src2->str, id_dest->str)

#endif
