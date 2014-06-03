#ifndef STRING_H
#define STRING_H

#include <stdint.h>

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

// abs of int32_t to uint32_t that is correct even for values
// that would overflow in normal abs (i.e. INT_MIN)
extern uint32_t abs_i32_to_u32(int32_t);

extern uint32_t strlen(const char *str);

extern void *memcpy(void *dest, const void *src, uint32_t n);

#endif
