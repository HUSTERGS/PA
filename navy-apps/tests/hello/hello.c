#include <unistd.h>
#include <stdio.h>

int main() {
  write(1, "Enter hello.c file\n", 19);
  write(1, "Hello World!\n", 13);
  // write(1, "Hello World!\n", 13);
  // write(1, "Hello World!\n", 13);
  // write(1, "Hello World!\n", 13);
  char s[10] = "123";
  sprintf(s, "456");
  // int i = 2;
  // volatile int j = 0;
  // // while (1) {
  // //   j ++;
  // //   if (j == 10000) {
  // //     printf("Hello World from Navy-apps for the %dth time!\n", i ++);
  // //     j = 0;
  // //   }
  // // }
  // // printf("111");
  // write(1, "Exit hello.c file\n", 18);
  return 0;
}
