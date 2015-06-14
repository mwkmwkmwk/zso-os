#ifndef PRINTF_H
#define PRINTF_H

#include "stddef.h"

void putc(char ch);
void puts(const char* str); // No implicit newline
void printf(const char *str, ...);

#endif
