#pragma once

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define PRINT_COLOR 0x0e

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

extern uint16_t ekran[SCREEN_HEIGHT][SCREEN_WIDTH];
struct FILE* stdout;

struct FILE {
    void (*putc)(char c);
};

void fputc(struct FILE *f, char c);
size_t fputs(struct FILE *F, const char *);
void fprintf(struct FILE *F, const char *, ...);
void vfprintf(struct FILE *F, const char *, va_list);
void printf(const char *fmt, ...);

int getc(void);
