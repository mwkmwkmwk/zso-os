#ifndef STDIO_H
#define STDIO_H

void clrscr(void);
void setcursor(unsigned x, unsigned y);
void putc(uint8_t c);
void printf(uint8_t* format, ...);

#endif