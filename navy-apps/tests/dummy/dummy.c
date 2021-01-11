#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

int main() {
  // Log("进入dummy");
  // printf("进入dummy");
  // assert(0);
  
  return _syscall_(SYS_yield, 0, 0, 0);
  return 0;
}
