#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

int vsnprintf (char *stream, size_t size, const char *fmt, va_list ap);
int snprintf(char *s, size_t n, const char *fmt, ...);
int printf(const char *fmt, ...);
size_t strnlen(const char *s, size_t maxlen);
