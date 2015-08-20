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

extern char end_stack;

struct tss {
	uint32_t link;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t junk[0x17];
} tss = {
	0,
	(uint32_t)&end_stack,
	0x10,
};

struct gdt_entry gdt[] = {
	{ 0 },                           //  0 (0)
	{ 0xffff, 0, 0, 0x9b, 0xcf, 0 }, //  8 (1)
	{ 0xffff, 0, 0, 0x93, 0xcf, 0 }, // 10 (2)
	{   0x67, 0, 0, 0x89, 0x40, 0 }, // 18 (3)
	{ 0xffff, 0, 0, 0xfb, 0xcf, 0 }, // 20 (4)
	{ 0xffff, 0, 0, 0xf3, 0xcf, 0 }, // 28 (5) - user stack segment
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
	0x2f,
	(uint32_t)&gdt,
}, lidt_arg = {
	0,
	0x7ff,
	(uint32_t)&idt,
};

void init_gdt() {
	uint32_t tss_i = (uint32_t)&tss;
	gdt[3].base_lo = tss_i & 0xffff;
	gdt[3].base_mid = tss_i >> 16 & 0xff;
	gdt[3].base_hi = tss_i >> 24 & 0xff;
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
	asm volatile (
		"ltr %0\n"
		:
		: "r"((uint16_t)0x18)
		:
	);
}

void set_idt(int idx, uint32_t addr, bool block, int pl) {
	idt[idx].addr_lo = addr & 0xffff;
	idt[idx].addr_hi = addr >> 16 & 0xffff;
	idt[idx].seg = 0x8;
	idt[idx].attr = (block ? 0x8e : 0x8f) | pl << 5;
}
