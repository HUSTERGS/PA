#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
// // 将参数列表的参数，int/str转化为param，然后进行打印
// static struct param {
//   char * buffer; // 用于将参数转化为字符之后保存数据
//   int value;
//   const char * str;
//   short sign; // 正负数
// };
// 返回打印的字符的个数

// 根据dst是否为NULL来判断应该输出到标准输出还是输出到dst
static void putc(char * dst, char ch, int offset) {
  if (dst) {
    dst[offset] = ch;
  } else {
    _putc(ch);
  }
}


static int print_s(const char * data, char * dst) {
  const char * count = data;
  while (*count) {
    putc(dst, *count, count - data);
    // if (dst) {
    //   putc(dst + (count - data), *count, 0);
    // } else {
    //   putc(NULL, *count, 0);
    // }
    count++;
  }
  return count - data;
}

static int print_d(int d, int count, char * dst) {
  if (d < 0) {
    putc(dst, '-', count);
    return print_d(-d, count, dst+1) +1;
  }
  if (d / 10) {
    count += print_d(d / 10, count, dst);
    putc(dst, d % 10 + '0', count);
  } else {
    putc(dst, d + '0', count);
  }
  return count + 1;
}



int printf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int count = vsprintf(NULL, fmt, ap);
  va_end(ap);
  putc(NULL, '\0', count);
  return count;
}

int vsprintf(char *out, const char *fmt, va_list ap) {

  char *s;
  int d;
  char ch;
  int count = 0;
  while((ch = *fmt++)) {
    if (ch != '%') {
      _putc(ch);
      count++;
      continue;
    }
    switch (*fmt++)
    {
    case 's':
      s = va_arg(ap, char *);
      count += out ? print_s(s, out + count) : print_s(s, NULL);
      break;
    case 'd':
      d = va_arg(ap, int);
      count += out ? print_d(d, 0, out + count) : print_d(d, 0, NULL);
      break;
    default:
      break;
    }
  }
  return count;
}

int sprintf(char *out, const char *fmt, ...) {
  // assert(0);
  // printf("lulalal");
  va_list ap;
  va_start(ap, fmt);
  int count = vsprintf(out, fmt, ap);
  va_end(ap);
  // 应该最后要打印一个结束符号？
  putc(out, '\0', count);
  return count;
}

// TODO: snprintf
int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
