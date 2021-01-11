#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdint.h>

int init_monitor(int, char *[]);
void ui_mainloop(int);
uint32_t expr(char *e, bool *success);

void test_expr(){
  char buf[65536];
  u_int32_t res,res_expr;
  bool success;
  FILE *fp=fopen("./tools/gen-expr/input","r");
  while (fscanf(fp,"%u%[^\n]",&res,buf)!=EOF){
			res_expr=expr(buf,&success);
      printf("res_expr : %d   res : %d   expr :%s\n",res_expr,res,buf);
	}
  fclose(fp);
}
int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

  //test_expr();

  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
