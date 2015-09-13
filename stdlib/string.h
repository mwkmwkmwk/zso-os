#pragma once

#include <stddef.h>

void* memcpy(void* dst, const void* src, size_t num);
void* memset(void* dst, int src, size_t num);
size_t strlen(const char* str);
void strrev(char* str);
char* strncpy(char* dst, const char* src, int dst_size);
