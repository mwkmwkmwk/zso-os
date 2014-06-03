#include "string.h"

// works up to base 36
static char digit2char[] = "0123456789abcdefghijklmnopqrstuvwxyz";

// it's user responsibility that width <= buf_size - 1
char *sprint_uint(char *buf, int buf_size, uint32_t x, int base, int width, char fill) {
	int i = buf_size - 1;
	buf[i--] = '\0';

	do {
		buf[i--] = digit2char[x % base];
		x /= base;
	} while (x > 0);

	while (i + width + 2 > buf_size) {
		buf[i--] = fill;
	}

	return buf + i + 1;
}

char *sprint_int(char *buf, int buf_size, int32_t x, int base, int width, char fill) {
	char *res;

	res = sprint_uint(buf + 1, buf_size - 1, abs_i32_to_u32(x),
			base, width == 0 ? 0 : width - 1, fill);

	if (x < 0) {
		*(--res) = '-';
		return res;
	} else {
		return res;
	}
}

uint32_t abs_i32_to_u32(int32_t x) {
	if (x >= 0) {
		return (uint32_t)x;
	} else {
		return (uint32_t)((-1) * (x + 1)) + 1;
	}
}

uint32_t strlen(const char *str) {
	uint32_t len = 0;

	while (str[len] != '\0') {
		++len;
	}

	return len;
}

void *memcpy(void *dest, const void *src, uint32_t n) {
	uint32_t i;

	// a small optimization that might be vectorized
	for (i = 0; i + 16 <= n; i += 16) {
		*(uint32_t *)(dest + i) = *(uint32_t *)(src + i);
		*(uint32_t *)(dest + i + 4) = *(uint32_t *)(src + i + 4);
		*(uint32_t *)(dest + i + 8) = *(uint32_t *)(src + i + 8);
		*(uint32_t *)(dest + i + 12) = *(uint32_t *)(src + i + 12);
	}

	for (i = 0; i < n; ++i) {
		*(char *)(dest + i) = *(char *)(src + i);
	}

	return dest;
}
