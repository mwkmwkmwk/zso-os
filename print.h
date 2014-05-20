#ifndef PRINT_H
#define PRINT_H

// clears screen and sets cursor to top-left corner
extern void cls(void);

// starts new line, shifts text towards top if screen full
extern void new_line(void);

// puts one character where cursor was and moves it forward, possibly generating new line
extern void put(char ch);

// prints a string by putting sequence of characters until (not including) '\0'
extern void print(const char *msg);

#endif
