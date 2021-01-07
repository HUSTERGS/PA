#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  // size_t len = 0;
  // while(s[len] != '\0') {
  //   len++;
  // }
  // return len;
  size_t i = 0;
  while(s[i] && s[i] != '\0') i++;
  return i;
}

char *strcpy(char* dst,const char* src) {
  
  size_t i = 0;
  while (src[i] != '\0') {
    dst[i] = src[i];
    i++;
  }
  assert(src[i] == '\0');
  dst[i] = '\0';
  return dst;
  // size_t i;
  // for(i=0;src[i]!='\0';i++) 
  //   dst[i]=src[i];
  
  // dst[i]='\0';
  // return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  size_t index = 0;
  while(src[index] != '\0' && index++ < n) {
    dst[index] = src[index];
  }
  // 如果src的长度小于n的话，那么就在dst的后面填充'\0'
  while (index < n) {
    dst[index++] = '\0';
  }
  return dst;
}

char* strcat(char* dst, const char* src) {
  strcpy(dst + strlen(dst), src);
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  size_t i = 0;
  while (s1[i] != '\0' && s1[i] == s2[i]) {
    i++;
  }
  return ((unsigned char *) s1)[i] - ((unsigned char *) s2)[i];
}

int strncmp(const char* s1, const char* s2, size_t n) {
  size_t i = 0;
  while (s1[i] != '\0' && s1[i] == s2[i]) {
    i++;
    if (i == n) {
      return 0; 
    }
  }
  return ((unsigned char *) s1)[i] - ((unsigned char *) s2)[i];
}

void* memset(void* v,int c,size_t n) {
  // unsigned char * ptr = v;
  // unsigned char val = c;
  // while (--n >= 0) {
  //   ptr[n] = val;
  // }
  // return v;
  const unsigned char uc = c; /* only cat the lowest c */
  unsigned char *vu;
  for (vu = v; 0 < n; ++vu, --n)
	  *vu = uc;
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  const unsigned char *src = in;
  unsigned char * dst = out;
  while (n--) {
    *dst++ = *src++;
  }
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  if(!n) return 0;
  while(--n && *(char *)s1 == *(char *)s2) {
	  s1 = (char *)s1 + 1;
	  s2 = (char *)s2 + 1;
  }
  return (*((unsigned char *)s1) - *((unsigned char *)s2));
}

#endif
