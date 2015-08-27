#include "io.h"

void outb(uint16_t port, uint8_t val) {
	asm volatile (
		"outb %0, %1\n":
		:
		"a"(val), "d"(port):
	);
}

uint8_t inb(uint16_t port) {
	uint8_t res;
	asm volatile (
		"inb %1, %0\n":
		"=a"(res):
		"d"(port):
	);
	return res;
}
