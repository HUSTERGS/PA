#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
// 将参数列表的参数，int/str转化为param，然后进行打印
static struct param {
  char * buffer; // 用于将参数转化为字符之后保存数据
  int value;
  const char * str;
  short sign; // 正负数
};

int printf(const char *fmt, ...) {
  
  while (*fmt != '\0') {
    // 遇到转义
    if (*fmt == '%') {
      switch (*(fmt+1))
      {
      case 'd':
        // 如果是数字
        break;
      case 's':
        // 如果是字符串
        break;
      case '%':
        _putc('%');
      default:
        assert(0);
      }
    }
  }
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  return 0;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
