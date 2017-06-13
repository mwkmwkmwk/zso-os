#include <stdarg.h>

#include "io.h"
#include "malloc.h"
#include "stdio.h"
#include "utils.h"


static int row = 2;
static int col = 0;

static void __new_line() {
    row = (row + 1) % SCREEN_HEIGHT;
    col = 0;
}

void putc(char c) {
    if (c == '\n') {
        __new_line();
        return;
    }
    ekran[row][col] = c | (PRINT_COLOR << 8);
    if (++col >= SCREEN_WIDTH) {
        __new_line();
    }
}

size_t puts(const char *p) {
    char c = 0;
    size_t len = 0;
    while (c = *p++) {
        putc(c);
        ++len;
    }
    return len;
}

static void print_padding(char pad, size_t pad_len, size_t len) {
    size_t i = 0;
    if (len < pad_len) {
        for (i = 0; i < pad_len - len; ++i) {
            putc(pad);
        }
    }
}

void printf(const char *fmt, ...) {
    va_list vl;
    char c = 0,
         pad = ' ';
    const char *pad_size_st = NULL;
    char *str = NULL;
    uint64_t pad_size = 0;
    size_t pad_str_len = 0,
           pad_len = 0,
           i = 0,
           print_len = 0;
    uint64_t x = 0;
    int size_mod = 0;

    va_start(vl, fmt);
    while (c = *fmt++) {
        if (c == '%') {
            pad_size_st = fmt;
            if ((c = *fmt) == '0') {
                pad = '0';
                ++pad_size_st;
                ++fmt;
            }
            while (c = *fmt, is_digit(c)) {
                ++fmt;
            }
            pad_str_len = fmt - pad_size_st;
            if (pad_str_len) {
                pad_len = str_to_uint(pad_size_st, pad_str_len);
            } else {
                pad_len = 0;
            }

            switch (c = *fmt) {
                case 'h':
                    ++fmt;
                    if (*fmt == 'h') {
                        ++fmt;
                        size_mod = PRINT_SIZE_8;
                    } else {
                        size_mod = PRINT_SIZE_16;
                    }
                    break;
                case 'l':
                    size_mod = PRINT_SIZE_64;
                    ++fmt;
                    break;
                default:
                    size_mod = PRINT_SIZE_32;
                    break;
            }

            switch (c = *fmt++) {
                case 'd':
                case 'u':
                case 'x':
                    switch (size_mod) {
                        case PRINT_SIZE_8:
                            x = va_arg(vl, uint32_t);
                            x &= 0xff;
                            break;
                        case PRINT_SIZE_16:
                            x = va_arg(vl, uint32_t);
                            x &= 0xffff;
                            break;
                        case PRINT_SIZE_32:
                            x = va_arg(vl, uint32_t);
                            break;
                        case PRINT_SIZE_64:
                            x = va_arg(vl, uint64_t);
                            break;
                    }
                    switch (c) {
                        case 'd':
                            print_len = int_to_str_dec(x, &str, size_mod);
                            break;
                        case 'u':
                            print_len = int_to_str_udec(x, &str);
                            break;
                        case 'x':
                            print_len = int_to_str_hex(x, &str);
                            break;
                    }
                    print_padding(pad, pad_len, print_len);
                    puts(str);
                    free(str);
                    break;
                case 's':
                    str = va_arg(vl, char *);
                    print_len = str_len(str);
                    print_padding(pad, pad_len, print_len);
                    puts(str);
                    break;
                default:
                    // TODO FIXME XXX KURWA start_ptr
                    putc('%');
                    if (pad == '0') {
                        putc('0');
                    }
                    for (i = 0; i < pad_len; ++i) {
                        putc(pad_size_st[i]);
                    }
                    switch (size_mod) {
                        case -2:
                            putc('h');
                            putc('h');
                            break;
                        case -1:
                            putc('h');
                            break;
                        case 1:
                            putc('l');
                            break;
                        default:
                            break;
                    }
                    putc(c);
                    break;
            }
            pad = ' ';
        } else {
            putc(c);
        }
    }
    va_end(vl);
}

int getc() {
    while (!(inb(0x64) & 1));
    return inb(0x60);
}
