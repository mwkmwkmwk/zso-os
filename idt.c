#include "idt.h"

struct gdt_entry {
	uint16_t limit_lo;
	uint16_t addr_lo;
	uint8_t addr_mid;
	uint8_t flags_lo;
	uint8_t flags_hi;
	uint8_t addr_hi;
};

struct gdt_entry gdt[] = {
	{ 0 },
	{ 0 },
	{ 0 },
	{ 0xffff, 0, 0, 0x9b, 0xaf, 0 },
	{ 0xffff, 0, 0, 0xfb, 0xaf, 0 },
	{ 0xffff, 0, 0, 0xf3, 0xaf, 0 },
};

struct idt_entry {
	uint16_t addr_lo;
	uint16_t seg;
	uint16_t flags;
	uint16_t addr_mid;
	uint32_t addr_hi;
	uint32_t _nic;
};

struct idt_entry idt[256];

void init_gdt() {
	struct {
		uint16_t _nic[3];
		uint16_t limit;
		void *addr;
	} gdtr;
	gdtr.limit = sizeof gdt - 1;
	gdtr.addr = &gdt;
	asm volatile (
		"lgdt 6(%0)\n"
		:
		: "r"(&gdtr)
		: "memory"
	);
}

void init_idt() {
	struct {
		uint16_t _nic[3];
		uint16_t limit;
		void *addr;
	} idtr;
	idtr.limit = 0xfff;
	idtr.addr = &idt;
	asm volatile (
		"lidt 6(%0)\n"
		:
		: "r"(&idtr)
		: "memory"
	);
}

void set_idt_entry(int idx, uint64_t addr, bool user) {
	idt[idx].addr_lo = addr;
	idt[idx].addr_mid = addr >> 16;
	idt[idx].addr_hi = addr >> 32;
	idt[idx].seg = 0x18;
	idt[idx].flags = user ? 0xee00 : 0x8e00;
}
