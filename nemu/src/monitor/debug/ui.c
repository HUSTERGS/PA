#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

// 单步执行
static int cmd_si(char * args);

// 打印程序状态
static int cmd_info(char * args);

// 表达式求值
static int cmd_exp(char * args);

// 扫描内存
static int cmd_scan(char * args);

// 设置监视点
static int cmd_setwp(char * args);

// 移除监视点
static int cmd_rmwp(char * args);

// 扫描内存
static int cmd_scan(char * args) {
  // `x N EXPR` 如 x 10 $esp
  // 求出表达式EXPR的值, 将结果作为起始内存地址, 以十六进制形式输出连续的N个4字节
  char * N_str = strtok(NULL, " ");
  
  // 分割点设置为""将取得后续的所有字符串
  char * EXPR_str = strtok(NULL, "");
  if (N_str && EXPR_str) {
    // 分别读取N和EXPR
    long N = strtol(N_str, NULL, 10);
    // long EXPR =  strtol(EXPR_str, NULL, 16);
    bool success = false;
    uint32_t EXPR = expr(EXPR_str, &success);
    if (!success) {
      Log("cmd_scan: 求值失败");
      return -1;
    }
    Log("cmd_scan: x %ld 0x%08x\n", N,  EXPR);
    long count = 0;
    for (long offset = 0; offset < N; offset++) {
      printf("0x");
      for (int i = 0; i < 4; i++) {
        // 每四个字节为一组
        printf("%02x", pmem[EXPR + offset * 4 + i]);
        count++;
      }
      printf(" ");
      // 每8组换一行
      if (count !=0 && count % 8 == 0) {
        printf("\n");
      }
    }
    return 0;
  } else {
    Log("cmd_scan: 格式有问题:  %s\n", args);
  }
  return -1;
}

static int cmd_si(char * args) {
  // 单步执行，唯一参数为一个正整数，如果args为NULL，则默认步进为1
  long steps = 1;
  // 暂时不考虑输入的数字有误的情况
  // IMPROVE: 根据man手册里面的内容严格判断是否是数字
  if (args != NULL) {
    // strtol函数会自动去除数字之后有问题的字符
    steps = strtol(args, NULL, 10);
  }
  if (steps > 0) {
    cpu_exec(steps);
  } else {
    Log("cmd_si: 单步执行输入的数字 = %ld < 0\n", steps);
  }
  return 0;
}

// 打印程序状态
static int cmd_info(char * args) {
  // IMPROVE: 测试是否可以直接通过*args来获取第一个字符，而不使用strtok函数
  // 获取info后面的r/w
  Log("进入info");
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    // QUESTION: 如果直接按回车args会是空吗
    Log("cmd_info: 输入参数为NULL\n");
    return 0;
  }

  if (strcmp(arg, "r") == 0) {
    // 输入的参数是r，表示打印寄存器的值
    Log("打印参数点");
    isa_reg_display();
  } else if (strcmp(arg, "w") == 0) {
    // 打印监视点的值
    print_wps();
  } else {
    Log("cmd_info: 未知命令\n");
  }
  return 0;
}

// 创建监视点
static int cmd_setwp(char * args) {
  if (!new_wp(args)) {
    Log("cmd_setwp: 监视点创建失败\n");
    return -1;
  }
  return 0;
}
// 移除监视点
static int cmd_rmwp(char * args) {
  rm_wp(strtol(args, NULL, 10));
  return 0;
}

static int cmd_exp(char * args) {
  bool success = false;
  uint32_t result = expr(args, &success);
  if (success) {
    printf("结果为%d\n", result);
    return 0;
  }
  Log("cmd_exp: 求值失败\n");
  return -1;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Single step", cmd_si},
  { "info", "Show info of regs/watchpoint", cmd_info},
  { "p", "Evaluate given expression", cmd_exp},
  { "x", "Scan memory", cmd_scan},
  { "w", "Set watchpoint", cmd_setwp},
  { "d", "Remove watchpoint", cmd_rmwp},
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  // strtok函数第一次调用的时候会传入args，之后对于同一个str就需要传入NULL
  char *arg = strtok(NULL, " ");
  int i;
  // args是从"help"开始的第一个字符串，表示help要查询的具体的指令
  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
