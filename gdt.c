#include "gdt.h"
#include <stdint.h>
#include <stdbool.h>

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
	{ 0xffff, 0, 0, 0x9b, 0xcf, 0 },
	{ 0xffff, 0, 0, 0x93, 0xcf, 0 },
};

struct idt_entry {
	uint16_t addr_lo;
	uint16_t seg;
	uint8_t _;
	uint8_t attr;
	uint16_t addr_hi;
};

struct idt_entry idt[256] = {
	0
};

struct lgdt_arg {
	uint16_t _;
	uint16_t limit;
	uint32_t base;
} lgdt_arg = {
	0,
	0x17,
	(uint32_t)&gdt,
}, lidt_arg = {
	0,
	0x7ff,
	(uint32_t)&idt,
};

void init_gdt() {
	asm volatile (
		"lgdt %0\n"
		:
		: "m"(lgdt_arg.limit)
		:
	);
	asm volatile (
		"lidt %0\n"
		:
		: "m"(lidt_arg.limit)
		:
	);
	asm volatile (
		"movl %0, %%ds\n"
		"movl %0, %%es\n"
		"movl %0, %%ss\n"
		:
		: "r"(0x10)
		:
	);
	asm volatile (
		"ljmp $0x08, $1f\n"
		"1:\n"
		:::
	);
}

void set_idt(int idx, uint32_t addr, bool block) {
	idt[idx].addr_lo = addr & 0xffff;
	idt[idx].addr_hi = addr >> 16 & 0xffff;
	idt[idx].seg = 0x8;
	idt[idx].attr = (block ? 0x8e : 0x8f);
}
