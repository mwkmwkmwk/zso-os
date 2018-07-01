#include <stdarg.h>
#include <stdint.h>
#include "stdio.h"
#include "io.h"


#define VGA_WRITE_START 0xffff8000000b8000ull
#define DEFAULT_FORCOLOR 0x0d
#define DEFAULT_BACKCOLOR 0x0f
#define WIDTH 80
#define HEIGHT 25


uint16_t screen[WIDTH * HEIGHT];
int cur_x = 0;
int cur_y = 0;


static void writec(char c, unsigned x, unsigned y, uint8_t forecolor, uint8_t backcolor) {
    uint16_t colors = (backcolor << 4) | (forecolor & 0x0F);
    uint16_t write_arg = c | colors << 8;
    unsigned pos = y * WIDTH + x;
    screen[pos] = write_arg;
    ((uint16_t*)VGA_WRITE_START)[pos] = screen[pos];
}

void setcursor(unsigned x, unsigned y) {
    if (x >= WIDTH)
        return;
    if (y >= HEIGHT)
        return;
    unsigned addr = x + y * 80;
    outb(0x3d4, 0xe);
    outb(0x3d5, addr >> 8);
    outb(0x3d4, 0xf);
    outb(0x3d5, addr);
}

static void scroll(void) {
    for (unsigned y = 1; y < HEIGHT; y++) {
        for (unsigned x = 0; x < WIDTH; x++) {
            unsigned src_pos = y * WIDTH + x;
            unsigned dst_pos = (y - 1) * WIDTH + x;
            screen[dst_pos] = screen[src_pos];
            ((uint16_t*)VGA_WRITE_START)[dst_pos] = screen[dst_pos];
        }
    }

    for (unsigned x = 0; x < WIDTH; x++) {
        writec(' ', x, HEIGHT - 1, DEFAULT_FORCOLOR, DEFAULT_BACKCOLOR);
    }
}

void clrscr(void) {
    for (unsigned x = 0; x < WIDTH; x++) {
        for (unsigned y = 0; y < HEIGHT; y++) {
            writec(' ', x, y, DEFAULT_FORCOLOR, DEFAULT_BACKCOLOR);
        }
    }
    setcursor(0, 0);
}

void putc(uint8_t c) {
    if (c == '\n') {
        cur_x = 0;
        cur_y++;
    }
    else {
        writec(c, cur_x, cur_y, DEFAULT_FORCOLOR, DEFAULT_BACKCOLOR);
        cur_y = (cur_x + 1 == WIDTH) ? cur_y + 1 : cur_y;
        cur_x = (cur_x + 1) % WIDTH;
    }

    if (cur_y == HEIGHT) {
        scroll();
        cur_y--;
    }
    setcursor(cur_x, cur_y);
}

static void print_char(char c) {
    putc(c);
}

static void print_str(uint8_t* str) {
    while (*str) {
        putc(*str);
        str++;
    }
}

static void print_num(uint32_t num, uint8_t base) {
    uint32_t tmp;
    uint8_t i = 0;
    uint8_t digits[8] = { 0 };

    if (!num) {
        putc('0');
        return;
    }

    while (num) {
        tmp = num;
        num /= base;
        digits[i] = "0123456789abcdef"[tmp - (num * base)];
        i++;
    }

    for (int j = i - 1; j >= 0; j--) {
        putc(digits[j]);
    }
}

void printf(uint8_t* format, ...) {
    va_list va;
    va_start(va, format);
    uint8_t c, b;
    uint8_t* str;
    uint32_t num;

    while (*format) {
        c = *format;
        if (c != '%') {
            putc(c);
        }
        else {
            format++;
            c = *format;

            switch(c) {
                case 'c':
                    b = (uint8_t) va_arg(va, uint32_t);
                    print_char(b);
                    break;
                case 's':
                    str = va_arg(va, uint8_t*);
                    print_str(str);
                    break;
                case 'd':
                    num = va_arg(va, uint32_t);
                    print_num(num, 10);
                    break;
                case 'x':
                    num = va_arg(va, uint32_t);
                    putc('0');
                    putc('x');
                    print_num(num, 16);
                    break;
            }
        }

        format++;
    }

    va_end(va);
}