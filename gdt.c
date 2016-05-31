#include <stdint.h>
#include <stdbool.h>
#include "gdt.h"
#include "io.h"

extern char stack_end[1];

struct tss {
	uint32_t _0;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t _1[0x68/4 - 3];
} tss = {
	.esp0 = (uint32_t)stack_end,
	.ss0 = 0x10,
};

struct idt_entry {
	uint16_t addr_lo;
	uint16_t seg;
	uint8_t _;
	uint8_t attr;
	uint16_t addr_hi;
};

struct idt_entry idt[64];

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
	// user code
	{ 0xffff, 0, 0, 0xfb, 0xcf, 0 },
	// user data
	{ 0xffff, 0, 0, 0xf3, 0xcf, 0 },
	// TSS
	{ 0x67, 0, 0, 0x89, 0x40, 0 },
};

struct lgdt_arg {
	uint16_t _;
	uint16_t limit;
	uint32_t base;
} lgdt_arg = {
	0, 0x2f,
	(uint32_t)gdt
}, lidt_arg = {
	0, 0x1ff,
	(uint32_t)idt
};

void set_idt_entry(int idx, void *handler, _Bool user) {
	uint32_t addr = (uint32_t)handler;
	idt[idx].addr_lo = addr;
	idt[idx].addr_hi = addr >> 16;
	idt[idx].seg = 0x8;
	uint8_t attr = 0x8f;
	if (user)
		attr |= 0x60;
	idt[idx].attr = attr;
}

extern char irq1_asm[1];
extern char syscall_asm[1];
extern char pagefault_asm[1];

void init_gdt(void) {
	asm volatile (
		"lgdt %0"
		:
		: "m"(lgdt_arg.limit)
	);
	asm volatile (
		"lidt %0"
		:
		: "m"(lidt_arg.limit)
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
	set_idt_entry(0x0e, pagefault_asm, false);
	set_idt_entry(0x21, irq1_asm, false);
	set_idt_entry(0x30, syscall_asm, true);
	uint32_t tss_base = (uint32_t)&tss;
	gdt[5].base_lo = tss_base;
	gdt[5].base_mid = tss_base >> 16;
	gdt[5].base_hi = tss_base >> 24;
	asm volatile (
		"ltr %0\n"
		:
		: "r"((uint16_t)0x28)
	);
	outb(0x20, 0x11);
	outb(0x21, 0x20);
	outb(0x21, 0x04);
	outb(0x21, 0x01);
	outb(0xa0, 0x11);
	outb(0xa1, 0x28);
	outb(0xa1, 0x02);
	outb(0xa1, 0x01);

	outb(0x21, 0xfd);
	outb(0xa1, 0xff);
	asm volatile ("sti");
}
