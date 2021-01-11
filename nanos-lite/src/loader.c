#include "proc.h"
#include <elf.h>
#include <fs.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
# define PHOFF 52

#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr elf;
  Elf_Phdr ph;
  //eph __attribute__((unused)); 

  size_t ret = ramdisk_read(&elf, 0, PHOFF);
  assert(ret == PHOFF);

  volatile uint32_t entry = elf.e_entry;
  // printf("entry = %x\n", entry);
  // printf("phoff = %x\n", elf.e_phoff);
  // printf("phnum = %x\n", elf.e_phnum);

  for(int i = 0; i < elf.e_phnum; i++){
    ramdisk_read(&ph, elf.e_phoff * (i + 1),sizeof(Elf_Phdr)); 
    //printf("offset = %x,  vaddr = %x, paddr = %x, filesz = %d, memsz = %d\n",ph.p_offset, ph.p_vaddr, ph.p_paddr, ph.p_filesz, ph.p_memsz);
    if (ph.p_type != PT_LOAD) continue;
    ramdisk_read((void *)ph.p_vaddr, ph.p_offset, ph.p_filesz);
    memset((void *)(ph.p_vaddr + ph.p_filesz), 0, ph.p_memsz - ph.p_filesz);
  }
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
