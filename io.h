#ifndef IO_H
#define IO_H

#include <stdint.h>

static inline uint8_t inb(uint16_t addr) {
	uint8_t res;
	asm volatile (
		"inb %%dx, %%al\n":
		"=a"(res):
		"d"(addr)
	);
	return res;
}

static inline uint16_t inw(uint16_t addr) {
	uint16_t res;
	asm volatile (
		"inw %%dx, %%ax\n":
		"=a"(res):
		"d"(addr)
	);
	return res;
}

static inline uint32_t inl(uint16_t addr) {
	uint32_t res;
	asm volatile (
		"inl %%dx, %%eax\n":
		"=a"(res):
		"d"(addr)
	);
	return res;
}

static inline void outb(uint16_t addr, uint8_t val) {
	asm volatile (
		"outb %%al, %%dx\n":
		:
		"d"(addr), "a"(val)
	);
}

static inline void outw(uint16_t addr, uint16_t val) {
	asm volatile (
		"outw %%ax, %%dx\n":
		:
		"d"(addr), "a"(val)
	);
}

static inline void outl(uint16_t addr, uint32_t val) {
	asm volatile (
		"outl %%eax, %%dx\n":
		:
		"d"(addr), "a"(val)
	);
}

#endif
