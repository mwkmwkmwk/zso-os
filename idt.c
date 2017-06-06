#include "idt.h"

struct tss {
	uint32_t _res1;
	uint64_t rsp0;
	uint64_t rsp1;
	uint64_t rsp2;
	uint64_t _res2;
	uint64_t rsp_ist1;
	uint64_t rsp_ist2;
	uint64_t rsp_ist3;
	uint64_t rsp_ist4;
	uint64_t rsp_ist5;
	uint64_t rsp_ist6;
	uint64_t rsp_ist7;
	uint32_t _res3[3];
} __attribute__((packed)) tss;

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
	// CS kernel 0x18
	{ 0xffff, 0, 0, 0x9b, 0xaf, 0 },
	// DS kernel 0x20
	{ 0xffff, 0, 0, 0x93, 0xaf, 0 },
	// CS32 user 0x2b
	{ 0xffff, 0, 0, 0xfb, 0xcf, 0 },
	// DS user 0x33
	{ 0xffff, 0, 0, 0xf3, 0xaf, 0 },
	// CS64 user 0x3b
	{ 0xffff, 0, 0, 0xfb, 0xaf, 0 },
	// TSS lo 0x40
	{ sizeof tss - 1, 0, 0, 0x89, 0, 0 },
	// TSS hi
	{ 0 },
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
	uint64_t tss_addr = (uint64_t)&tss;
	gdt[8].addr_lo = tss_addr & 0xffff;
	gdt[8].addr_mid = tss_addr >> 16 & 0xff;
	gdt[8].addr_hi = tss_addr >> 24 & 0xff;
	gdt[9].limit_lo = tss_addr >> 32 & 0xffff;
	gdt[9].addr_lo = tss_addr >> 48 & 0xffff;
	asm volatile (
		"ltr %0\n"
		:
		: "r"((uint16_t)0x40)
	);
}

void set_tss_rsp(uint64_t rsp) {
	tss.rsp0 = rsp;
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
