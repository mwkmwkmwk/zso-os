#include "idt.h"

struct idt_entry {
	uint16_t addr_lo;
	uint16_t seg;
	uint16_t flags;
	uint16_t addr_mid;
	uint32_t addr_hi;
	uint32_t _nic;
};

struct idt_entry idt[256];

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
