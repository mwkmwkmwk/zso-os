#include <stddef.h>
#include <stdint.h>
#include "gdt.h"

void *memcpy(void *a, void *b, size_t s) {
	char *ca = a;
	char *cb = b;
	ptrdiff_t i;
	for (i = 0; i < s; i++)
		ca[i] = cb[i];
	return a;
}

static inline void outb(uint16_t port, uint8_t val) {
	asm volatile ("outb %0, %1" : : "a"(val), "d"(port));
}

void printf(char *format, ...) {
	while (*format) {
		char c = *format++;
		outb(0x3f8, c);
	}
}

void main(void) {
	init_gdt();
	char hello[2*80*25] = {
		'H', 0x0a,
		'e', 0x0a,
		'l', 0x0a,
		'l', 0x0a,
		'o', 0x0a,
		'.', 0x0a,
		' ', 0x0a,
		'w', 0x0a,
		'o', 0x0a,
		'r', 0x0a,
		'l', 0x0a,
		'd', 0x0a,
		'!', 0x0a,
	};
	memcpy((void*)0xb8000, hello, sizeof hello);
	printf("Hello, world!\n");
}
