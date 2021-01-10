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

static int int_width(long value, int base) {
  int count = 1;
  while (value / base) {
    count++;
    value /= base;
  }
  return count;
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

static char int_to_ch(int value) {
  // assert(value >= 0);
  if (value < 10) {
    return value + '0';
  } else {
    return value - 10 + 'a';
  }
}

static int print_d(long d, int count, char * dst, int base) {
  assert(base == 10 || base == 16);
  if (d < 0) {
    putc(dst, '-', count);
    return print_d(-d, count, dst ? dst+1 : dst, base) +1;
  }
  
  if (d / base) {
    count += print_d(d / base, count, dst, base);
    putc(dst, int_to_ch(d % base), count);
  } else {
    putc(dst, int_to_ch(d), count);
  }
  return count + 1;
}


static int print_p(unsigned long p, int count, char * dst) {
  unsigned int base = 16;
  assert(p >= 0);
  if (p / base) {
    count += print_p(p / base, count, dst);
    putc(dst, int_to_ch(p % base), count);
  } else {
    putc(dst, int_to_ch(p), count);
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



static void print_bits(long value) {
  while (value / 16) {
    _putc(int_to_ch(value % 16));
    value /= 16;
  }
}

int vsprintf(char *out, const char *fmt, va_list ap) {

  char *s;
  long d;
  char ch;
  int count = 0;
  int width;
  while((ch = *fmt++)) {
    if (ch != '%') {
      putc(out, ch, count);
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
      print_bits(d);
      count += out ? print_d(d, 0, out + count, 10) : print_d(d, 0, NULL, 10);
      break;
    // 在pa2.3中的mainarg的main.c中使用了%c，如果klib使用自己的话就需要实现%c
    case 'c':
      ch = va_arg(ap, int);
      putc(out, ch, count);
      count++;
      break;
    // 16进制
    case 'x':
      d = va_arg(ap, int);
      count += out ? print_d(d, 0, out + count, 16) : print_d(d, 0, NULL, 16);
      break;
    case 'p':
      d = va_arg(ap, unsigned long);
      putc(out, '0', count++);
      putc(out, 'x', count++);
      count += print_p(d, 0, out ? out + count : out);
      // count += out ? print_p(d, 0, out + count) : print_p(d, 0, NULL);
      break;
    case '0':
      // 遇到0，之后的数字就是长度

      width = *fmt++ - '0';
      d = va_arg(ap, int);
      // 直接向对应的位置填充width - actual_witdth个字符
      for (int i = 0; i < width - int_width(d, *fmt == 'd' ? 10 : 16); i++) {
        putc(out, '0', count + i);
      }
      assert(width == 8);
      if (*fmt == 'd') {
        out ? print_d(d, 0, out + count, 10) : print_d(d, 0, NULL, 10);
        count += width;
      } else if (*fmt == 'x') {
        out ? print_d(d, 0, out + count, 16) : print_d(d, 0, NULL, 16);
        count += width;
      } else {
        assert(0);
      }
      fmt++;
    default:
      assert(0);
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
