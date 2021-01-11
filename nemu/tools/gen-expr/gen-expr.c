#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536];
int length=0;
uint32_t choose(uint32_t n){
	return rand()%n;
}

void gen_num(){
  uint32_t num=choose(100);
  char temp[32];
  sprintf(temp,"%d",num);
  int i;
  for(i=length;i<length+strlen(temp);i++)
    buf[i]=temp[i-length];
  length=i;
  buf[length]='\0';
}
void gen_rand_op(){
  switch (choose(4)){
		case 0:
      buf[length++]='+';
      break;
		case 1:
      buf[length++]='-';
      break;
		case 2:
      buf[length++]='*';
      break;
		case 3:
      buf[length++]='/';
      break;
	}
  buf[length]='\0';
}
static inline void gen_rand_expr() {
  switch (choose(3)) {
    case 0: 
      gen_num(); 
      break;
    case 1: 
      buf[length++]='(';
      gen_rand_expr(); 
      buf[length++]=')';
      break;
    default: 
      gen_rand_expr(); 
      gen_rand_op(); 
      gen_rand_expr(); 
      break;
  }
}


static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();
    
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);

    memset(buf,0,strlen(buf));
    length=0;
  }
  return 0;
}
