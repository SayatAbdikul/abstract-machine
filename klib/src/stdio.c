#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static void out_char(char **out, size_t *remain, int *count, char c) {
  if (*remain > 1) {
    **out = c;
    (*out)++;
    (*remain)--;
  }
  (*count)++;
}

static void out_str(char **out, size_t *remain, int *count, const char *s) {
  if (s == NULL) s = "(null)";
  while (*s != '\0') out_char(out, remain, count, *s++);
}

static void out_uint(char **out, size_t *remain, int *count, unsigned int x, unsigned int base) {
  char buf[16];
  int i = 0;
  const char *digits = "0123456789abcdef";
  if (x == 0) {
    out_char(out, remain, count, '0');
    return;
  }
  while (x != 0) {
    buf[i++] = digits[x % base];
    x /= base;
  }
  while (i > 0) out_char(out, remain, count, buf[--i]);
}

static void out_int(char **out, size_t *remain, int *count, int x) {
  if (x < 0) {
    out_char(out, remain, count, '-');
    out_uint(out, remain, count, (unsigned int)(-(x + 1)) + 1, 10);
  } else {
    out_uint(out, remain, count, (unsigned int)x, 10);
  }
}

int printf(const char *fmt, ...) {
  char buf[1024];
  va_list ap;
  va_start(ap, fmt);
  int n = vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  for (int i = 0; buf[i] != '\0'; i++) putch(buf[i]);
  return n;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out, (size_t)-1, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int n = vsprintf(out, fmt, ap);
  va_end(ap);
  return n;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsnprintf(out, n, fmt, ap);
  va_end(ap);
  return ret;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  char *p = out;
  size_t remain = n;
  int count = 0;

  for (; *fmt != '\0'; fmt++) {
    if (*fmt != '%') {
      out_char(&p, &remain, &count, *fmt);
      continue;
    }

    fmt++;
    switch (*fmt) {
      case '\0':
        fmt--;
        break;
      case '%':
        out_char(&p, &remain, &count, '%');
        break;
      case 'c':
        out_char(&p, &remain, &count, va_arg(ap, int));
        break;
      case 's':
        out_str(&p, &remain, &count, va_arg(ap, const char *));
        break;
      case 'd':
        out_int(&p, &remain, &count, va_arg(ap, int));
        break;
      case 'u':
        out_uint(&p, &remain, &count, va_arg(ap, unsigned int), 10);
        break;
      case 'x':
      case 'p':
        out_uint(&p, &remain, &count, va_arg(ap, unsigned int), 16);
        break;
      default:
        out_char(&p, &remain, &count, '%');
        out_char(&p, &remain, &count, *fmt);
        break;
    }
  }

  if (n > 0) {
    if (remain > 0) *p = '\0';
    else out[n - 1] = '\0';
  }
  return count;
}

#endif
