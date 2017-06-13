#pragma once

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define PRINT_COLOR 0x0e

#include <stddef.h>
#include <stdint.h>

extern uint16_t ekran[SCREEN_HEIGHT][SCREEN_WIDTH];

void putc(char);
size_t puts(const char *);
void printf(const char *, ...);

int getc(void);
