#include <stdint.h>
#include "gdt.h"

struct gdt_entry {
	uint16_t limit_lo;
	uint16_t base_lo;
	uint8_t base_mid;
	uint8_t attr;
	uint8_t ext;
	uint8_t base_hi;
};

struct gdt_entry gdt[] = {
	{ 0 },
	// kernel code
	{ 0xffff, 0, 0, 0x9b, 0xcf, 0 },
	// kernel data
	{ 0xffff, 0, 0, 0x93, 0xcf, 0 },
};

struct lgdt_arg {
	uint16_t _;
	uint16_t limit;
	uint32_t base;
} lgdt_arg = {
	0, 0x17,
	(uint32_t)gdt
};

void init_gdt(void) {
	asm volatile (
		"lgdt %0"
		:
		: "m"(lgdt_arg.limit)
	);
	asm volatile (
		"movl %0, %%ds\n"
		"movl %0, %%ss\n"
		"movl %0, %%es\n"
		:
		: "r"(0x10)
	);
	asm volatile (
		"ljmpl $0x08, $1f\n"
		"1:\n"
		:::
	);
}
