#ifndef PRINT_H
#define PRINT_H

#include <stdint.h>

#define SCREEN_ROWS 25
#define SCREEN_COLS 80

#define TAB_SIZE 8

// initializes screen without wiping it
extern void init_screen(void);

// clears screen and sets cursor to top-left corner, can replace init_screen
extern void cls(void);

// starts new line, shifts text towards top if screen full
extern void new_line(void);

// puts one character where cursor was and moves it forward, possibly generating new line
// supports special characters:
// \n - new line
// \r - set cursor at the beginning of current line without wiping it
// \t - print 1-8 spaces until cursor column is divisible by TAB_SIZE
extern void put(char ch);

// prints a string by putting sequence of characters until (not including) '\0'
extern void print(const char *msg);

// printf supporting %d (int), %x (int), %s (const char *)
extern void printf(const char *format, ...);

// printf number `x` into string buffer `buf` of size `buf_size`
// (including terminating null character). Number will be printed in base
// `base` (2-36). If `width` is non-zero then number will be padded with
// `fill` until at least `width` characters are written, excluding sign.
// Writes to buffer from its end. Returns beginning of written part of
// buffer (usually not `buf`).
extern char *sprint_uint(char *buf, int buf_size, uint32_t x,
		int base, int width, char fill);
extern char *sprint_int(char *buf, int buf_size, int32_t x,
		int base, int width, char fill);

#endif
