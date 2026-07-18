#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t n = 0;
  while (s[n] != '\0') n++;
  return n;
}

char *strcpy(char *dst, const char *src) {
  char *ret = dst;
  while ((*dst++ = *src++) != '\0');
  return ret;
}

char *strncpy(char *dst, const char *src, size_t n) {
  char *ret = dst;
  size_t i = 0;
  for (; i < n && src[i] != '\0'; i++) dst[i] = src[i];
  for (; i < n; i++) dst[i] = '\0';
  return ret;
}

char *strcat(char *dst, const char *src) {
  char *ret = dst;
  while (*dst != '\0') dst++;
  while ((*dst++ = *src++) != '\0');
  return ret;
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 != '\0' && *s1 == *s2) {
    s1++;
    s2++;
  }
  return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  for (size_t i = 0; i < n; i++) {
    unsigned char c1 = s1[i];
    unsigned char c2 = s2[i];
    if (c1 != c2 || c1 == '\0') return c1 - c2;
  }
  return 0;
}

void *memset(void *s, int c, size_t n) {
  unsigned char *p = s;
  const unsigned char byte = (unsigned char)c;
  while (n != 0 && ((uintptr_t)p & (sizeof(uint32_t) - 1u)) != 0u) {
    *p++ = byte;
    n--;
  }
  const uint32_t word = (uint32_t)byte * 0x01010101u;
  uint32_t *words = (uint32_t *)p;
  while (n >= sizeof(uint32_t)) {
    *words++ = word;
    n -= sizeof(uint32_t);
  }
  p = (unsigned char *)words;
  while (n-- != 0) *p++ = byte;
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  unsigned char *d = dst;
  const unsigned char *s = src;
  if (d == s || n == 0) return dst;
  if (d < s) {
    for (size_t i = 0; i < n; i++) d[i] = s[i];
  } else {
    for (size_t i = n; i > 0; i--) d[i - 1] = s[i - 1];
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  unsigned char *d = out;
  const unsigned char *s = in;
  for (size_t i = 0; i < n; i++) d[i] = s[i];
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const unsigned char *p1 = s1;
  const unsigned char *p2 = s2;
  for (size_t i = 0; i < n; i++) {
    if (p1[i] != p2[i]) return p1[i] - p2[i];
  }
  return 0;
}

#endif
