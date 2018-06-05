#include <stdint.h>
#include "gdt.h"

extern void stack;

struct tss {
	uint32_t _pad;
	uint64_t rsp[3];
	uint64_t _pad2;
	uint64_t ist_rsp[7];
	uint32_t _pad3[3];
} __attribute__((packed)) tss = {
	.rsp[0] = &stack,
};

_Static_assert(sizeof tss == 0x68, "meh");

struct gdt_entry {
	uint16_t limit_lo;
	uint16_t base_lo;
	uint8_t base_mid;
	uint8_t flags_a;
	uint8_t flags_b;
	uint8_t base_hi;
} gdt[] = {
	// 0
	{ 0 },
	// 8: kernel code
	{ 0, 0, 0, 0x9b, 0xa0, 0 },
	// 0x10: user code 32-bit
	{ 0xffff, 0, 0, 0xfb, 0xcf, 0 },
	// 0x18: user data
	{ 0xffff, 0, 0, 0xf3, 0xcf, 0 },
	// 0x20: user code 64-bit
	{ 0, 0, 0, 0xfb, 0xa0, 0 },
	// 0x28: TSS
	{ sizeof tss - 1, 0, 0, 0x89, 0, 0 },
	{ 0, 0, 0, 0, 0, 0 },
};

struct idt_entry {
	uint16_t addr_lo;
	uint16_t segment;
	uint8_t ist;
	uint8_t flags;
	uint16_t addr_mid;
	uint32_t addr_hi;
	uint32_t _pad;
} idt[256] = {
	{0},
};

void init_gdt() {
	uint64_t tss_addr = (uint64_t)&tss;
	gdt[5].base_lo = tss_addr;
	gdt[5].base_mid = tss_addr >> 16;
	gdt[5].base_hi = tss_addr >> 24;
	gdt[6].limit_lo = tss_addr >> 32;
	gdt[6].base_lo = tss_addr >> 48;
	struct gdtr {
		uint16_t _pad[3];
		uint16_t limit;
		void *addr;
	} gdtr = {
		{0, 0, 0}, sizeof gdt - 1, &gdt,
	};
	struct idtr {
		uint16_t _pad[3];
		uint16_t limit;
		void *addr;
	} idtr = {
		{0, 0, 0}, sizeof idt - 1, &idt,
	};
	__asm__ volatile (
		"lgdt (%0)"
		:
		: "r"(&gdtr.limit)
	);
	__asm__ volatile (
		"lidt (%0)"
		:
		: "r"(&idtr.limit)
	);
	__asm__ volatile (
		"ltr %0"
		:
		: "r"((uint16_t)0x28)
	);
}
