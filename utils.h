#pragma once

#include <stddef.h>
#include <stdint.h>

#define PRINT_SIZE_8  -2
#define PRINT_SIZE_16 -1
#define PRINT_SIZE_32  0
#define PRINT_SIZE_64  1

void rev_str(char *, size_t);

size_t int_to_str_dec(int64_t, char **, int);
size_t int_to_str_hex(uint64_t, char **);
size_t int_to_str_udec(uint64_t, char **);

uint64_t str_to_uint(const char *, size_t);

int is_digit(char);

size_t str_len(const char *);
