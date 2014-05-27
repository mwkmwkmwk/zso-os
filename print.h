#ifndef PRINT_H
#define PRINT_H

// initializes screen without wiping it
extern void init_screen(void);

// clears screen and sets cursor to top-left corner, can replace init_screen
extern void cls(void);

// starts new line, shifts text towards top if screen full
extern void new_line(void);

// puts one character where cursor was and moves it forward, possibly generating new line
// supports special characters \n \r \t
extern void put(char ch);

// prints a string by putting sequence of characters until (not including) '\0'
extern void print(const char *msg);

// printf supporting %d (int), %x (int), %s (const char *)
extern void printf(const char *format, ...);

#endif
