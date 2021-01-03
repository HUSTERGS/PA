#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

// 理论上只有长度超过一个的符号才需要写别名
enum {
  TK_NOTYPE = 256, 
  TK_EQ, // 等号 == 
  TK_DECI_NUM, // 十进制数字
  TK_HEX_NUM, // 十六进制数字
  TK_REG, // 寄存器
  TK_DEREF, //解引用
  TK_NTEQ,
  TK_AND,
  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\*", '*'},         // multi， 考虑到*可能是乘法也可能是解引用，而解引用的优先级要高于乘除法，所以乘号的优先级最高
  {"/", '/'},           // divide
  {"\\+", '+'},         // plus
  {"==", TK_EQ},         // equal
  {"!=", TK_NTEQ},      // not equal
  {"&&", TK_AND},       // and &&
  // FIXME: 不知道对于小括号的判断是不是正确的
  {"\\(", '('},
  {"\\)", ')'},
  {"[0-9]+", TK_DECI_NUM},           // decimal nums
  {"0x[0-9a-f]+", TK_HEX_NUM},          // hex nums
  {"\\$[a-zA-Z]{2,3}", TK_REG}, // 寄存器名应该只能是2-3长度的
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        Assert(substr_len <= 31, "字符串长度应该小于31");
        switch (rules[i].token_type) {
          case TK_DECI_NUM:
          case TK_HEX_NUM:
            // 如果是数字字面量，就直接复制字符串到str字段
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            break;
          case TK_REG:
            // TODO: 这里暂时不考虑寄存器名不存在的情况
            Assert(substr_len >= 2, "正则处理寄存器，寄存器名应该大于等于2");
            // 存储寄存器名到str字段，省略前置的$符号
            strncpy(tokens[nr_token].str, substr_start+1, substr_len-1);
            tokens[nr_token].str[substr_len-1] = '\0';
            break;
          
        }
        // 设定type
        tokens[nr_token++].type = rules[i].token_type;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool deref_match(int type) {
  char * str = "+-*/()";
  for (int i = 0; i < strlen(str); i++) {
    if (type == str[i]) {
      return true;
    }
  }
  return type == TK_DEREF || type == TK_EQ || type == TK_NTEQ || type == TK_AND;
}



bool check_parentheses(int p, int q, bool *bad) {
  // 判断表达式是否被一对匹配的括号包围着
  
  // 有两种情况说明根本不能求值，
  // 1. 在循环的过程中，出现了多余的右括号，也就是count < 0
  // 2. 检查到最后的时候，count != 0，也就是括号总数不匹配
  bool result = false;
  *bad = false;
  if (tokens[p].type == '(' && tokens[q].type == ')') {
    int count = 1;
    for (int j = p+1; j <= q; j++) {
      if (tokens[j].type == '(') {
        count++;
      } else if (tokens[j].type == ')') {
        count--;
      }
      
      if (count < 0) {
        *bad = true;
      }
      // 找到与左侧括号对应的括号，如果不是最后一个说明最后一个不匹配
      if (count == 0 && j == q) {
        result = true;
      }
    }
    if (count != 0) {
      *bad = true;
    }
  }
  return result;
}

// 寻找p,q之间的主符号
// 从右向左找
int main_op(int p, int q) {

  int backup = -1;
  for (int i = q; i >= p; i--) {
    if (tokens[i].type == ')') {
      int count = 1;
      while (--i >= q) {
        if (tokens[i].type == ')') {
          count++;
        } else if (tokens[i].type == '(') {
          count--;
        }
        if (count == 0) {
          break;
        }
      }
    }
    // Assert(tokens[i].type == '(', "main_op: 括号应该对应");
    // 找到了对应的另一个括号，位置为i，括号中间的不可能为主运算符号
    // 如果是加号或者减号，那么必定为主符号
    // TODO: 处理减号和负号的情况
    // IMPROVE: 通过定义别名的优先级来，并通过减法来简化以下if判断
    // 根据优先级来依次进行判断
    // && 
    if (tokens[i].type == TK_AND) {
      // 如果是&& 则必为主运算符
      backup = i;
    } else if (tokens[i].type == TK_EQ || tokens[i].type == TK_NTEQ) {
      if (backup != -1 && tokens[backup].type == TK_AND) {
        continue;
      }
      backup = i;
    } else if (tokens[i].type == '+' || tokens[i].type == '-') {
      if (backup != -1 && (
        tokens[backup].type == TK_AND || 
        tokens[backup].type == TK_EQ ||
        tokens[backup].type == TK_NTEQ 
      )) {
        continue;
      }
      backup = i;
    } else if (tokens[i].type == '*' || tokens[i].type == '/') {
      if (backup != -1 && (
        tokens[backup].type == TK_AND || 
        tokens[backup].type == TK_EQ ||
        tokens[backup].type == TK_NTEQ ||
        tokens[backup].type == '+' ||
        tokens[backup].type == '-'
      )) {
        continue;
      }
      backup = i;
    } else if (tokens[i].type == TK_DEREF) {
      // 如果有多个解引用，先计算第一个遇到的，也就是从右往左第一个
      if (backup != -1 && (
        tokens[backup].type == TK_AND || 
        tokens[backup].type == TK_EQ ||
        tokens[backup].type == TK_NTEQ ||
        tokens[backup].type == '+' ||
        tokens[backup].type == '-' || 
        tokens[backup].type == '*' ||
        tokens[backup].type == '/'
      )) {
        continue;
      }
      backup = i;
    }
  }

  Assert(backup != -1, "最终计算结果应该始终存在");
  return backup;
}

uint32_t eval(int p, int q, bool *success) {
  bool bad = false;
  if (p > q) {
    *success = false;
    Log("eval函数：p > q");
    return -1;
  } else  if (p == q) {
    switch (tokens[p].type)
    {
    case TK_DECI_NUM:
      // 如果是10进制
      Log("十进制");
      return strtol(tokens[p].str, NULL, 10);
    case TK_HEX_NUM:
      // 如果是16进制
      return strtol(tokens[p].str, NULL, 16);
    case TK_REG:  {
      bool success_reg = false;
      uint32_t reg_val = isa_reg_str2val(tokens[p].str, &success_reg);
      if (success_reg) {
        return reg_val;
      }
      Log("寄存器求值失败");
      *success = false;
      return -1;
    }
      
    default:
      break;
    }
  } else if (check_parentheses(p, q, &bad) == true && !bad) {
    return eval(p+1, q-1, success);
  } else {
    if (bad) {
      Log("eval: 表达式括号有问题");
      *success = false;
      return -1;
    }
    int op = main_op(p, q);

    if (tokens[op].type == TK_DEREF) {
      // 如果是解引用
      Assert(op == p, "如果main_op计算的结果为解引用，那么解引用必然是第一个");
      uint32_t address = eval(p+1, q, success);
      if (*success == false) {
        Log("main_op: 解引用失败");
        return -1;
      } else if (address < 0 || address > PMEM_SIZE - 4) {
        *success = false;
        Log("解引用操作超出内存范围");
        return -1;
      } else {
        // FIXME: 检查这个地方是否有问题
        return isa_vaddr_read(address, 4);
        // return pmem[address] | 
        // (uint32_t)pmem[address+1] << 8 |
        // (uint32_t)pmem[address+2] << 16 |
        // (uint32_t)pmem[address+3] << 24;
      }
    }

    uint32_t val1 = eval(p, op - 1, success);
    if (*success == false) {
      Log("main_op: 求第一个子表达式出错");
      return -1;
    }
    uint32_t val2 = eval(op + 1, q, success);
    if (*success == false) {
      Log("main_op: 求第二个子表达式出错");
      return -1;
    }
    switch (tokens[op].type)
    {
    case '+': return val1 + val2;
    case '-': return val1 - val2;
    case '*': return val1 * val2;
    case '/': return val1 / val2;
    case TK_EQ: return val1 == val2;
    case TK_NTEQ: return val1 != val2;
    case TK_AND:  return val1 && val2;
    default:
      panic("eval: main_op 监测到其他符号");
    }
  }
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  // 先从头遍历一遍，把解引用处理掉
  for (int i = 0; i < nr_token; i++) {
    // 如果i本身是一个乘号，然后前一个符号的类型是 +-*/或者TK_DEREF
    if (tokens[i].type == '*' && (i == 0 || deref_match(tokens[i - 1].type))) {
      tokens[i].type = TK_DEREF;
    }
  }
  

  uint32_t result = eval(0, nr_token-1, success);

  return result;
}