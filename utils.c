#include "malloc.h"
#include "utils.h"

void rev_str(char *buf, size_t len) {
    size_t i = 0;
    for (i = 0; i < len/2; ++i) {
        buf[i] ^= buf[len - 1 - i];
        buf[len - 1 - i] ^= buf[i];
        buf[i] ^= buf[len - 1 - i];
    }
}

static size_t uint_to_str_le(uint64_t x, char **buf_ptr) {
    size_t len = 0,
           i = 0;
    char *buf = NULL;

    // one byte for optional sign and one for null byte
    buf = malloc(22);
    do {
        buf[len++] = (x % 10) + 0x30;
        x /= 10;
    } while (x != 0);
    buf[len] = '\x00';
    *buf_ptr = buf;
    return len;
}

size_t int_to_str_dec(int64_t x, char **buf_ptr, int size_mod) {
    int sign_present = 0;
    size_t len = 0;

    switch (size_mod) {
        case PRINT_SIZE_8:
            x = (int8_t)x;
            break;
        case PRINT_SIZE_16:
            x = (int16_t)x;
            break;
        case PRINT_SIZE_32:
            x = (int32_t)x;
            break;
    }

    if (x < 0) {
        x = -x;
        sign_present = 1;
    }
    len = uint_to_str_le(x, buf_ptr);
    if (sign_present != 0) {
        (*buf_ptr)[len++] = '-';
    }
    rev_str(*buf_ptr, len); 
    (*buf_ptr)[len] = '\x00';
    return len;
}

size_t int_to_str_udec(uint64_t x, char **buf_ptr) {
    size_t len = 0;
    len = uint_to_str_le(x, buf_ptr);
    (*buf_ptr)[len] = '\x00';
    rev_str(*buf_ptr, len);
    return len;
}

static char digit_to_hex(unsigned int x) {
    if (x < 10) {
        return x + 0x30;
    }
    return x - 10 + 0x61;
}

size_t int_to_str_hex(uint64_t x, char **buf_ptr) {
    size_t len = 0;
    char *buf = NULL;

    buf = malloc(16);
    do {
        buf[len++] = digit_to_hex(x % 16);
        x /= 16;
    } while (x != 0);
    rev_str(buf, len);
    buf[len] = '\x00';
    *buf_ptr = buf;
    return len;
}

uint64_t str_to_uint(const char *p, size_t len) {
    size_t i = 0;
    uint64_t res = 0;
    for (i = 0; i < len; ++i) {
        res *= 10;
        res += *p - 0x30;
    }
    return res;
}

int is_digit(char c) {
    if ((c >= 0x30) && (c <= 0x39)) {
        return 1;
    }
    return 0;
}

size_t str_len(const char *p) {
    size_t i = 0;
    while (*p++) {
        ++i;
    }
    return i;
}
