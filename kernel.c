#include <stdint.h>
#include "print.h"
#include "pic.h"
#include "io.h"
#include "multiboot.h"
#include "alloc.h"

const __attribute__((section("header"))) uint32_t multiboot_header[] = {
	0x1BADB002,
	3,
	- 0x1BADB002 - 3,
};

uint8_t stack[0x1000];
struct multiboot_info *mb_info;

struct gdt_entry {
	uint16_t limit_lo;
	uint16_t base_lo;
	uint8_t base_mid;
	uint8_t attr;
	uint8_t flags;
	uint8_t base_hi;
};

struct idt_entry {
	uint16_t addr_lo;
	uint16_t segment;
	uint8_t zero;
	uint8_t attr;
	uint16_t addr_hi;
};

struct gdt_entry gdt[3] = {
	{ 0 },
	{ 0xffff, 0, 0, 0x9b, 0xcf, 0 },
	{ 0xffff, 0, 0, 0x93, 0xcf, 0 },
};

struct idt_entry idt[256] = { 0 };

struct gdt_ptr {
	uint16_t _pad;
	uint16_t limit;
	void *base;
} gdt_ptr = {
	0,
	sizeof gdt - 1,
	gdt,
}, idt_ptr = {
	0,
	sizeof idt - 1,
	idt,
};

asm (
	".text\n"
	".global _start\n"
	"_start:\n"
	"movl $stack+0x1000, %esp\n"
	"movl %ebx, mb_info\n"
	"lgdt gdt_ptr+2\n"
	"lidt idt_ptr+2\n"
	"movl $0x10, %eax\n"
	"movl %eax, %ds\n"
	"movl %eax, %es\n"
	"movl %eax, %ss\n"
	"movl $0x0, %eax\n"
	"movl %eax, %fs\n"
	"movl %eax, %gs\n"
	"ljmp $0x08,$main\n"
);

void set_idt_entry(int idx, void *addr) {
	uint32_t a = (uint32_t)addr;
	idt[idx].addr_lo = a & 0xffff;
	idt[idx].segment = 0x08;
	idt[idx].zero = 0;
	idt[idx].attr = 0x8e;
	idt[idx].addr_hi = a >> 16;
}

asm (
	".text\n"
	".global int21_asm\n"
	"int21_asm:\n"
	"pushfl\n"
	"pushl %eax\n"
	"pushl %ecx\n"
	"pushl %edx\n"
	"call int21\n"
	"popl %edx\n"
	"popl %ecx\n"
	"popl %eax\n"
	"popfl\n"
	"iretl\n"
);

void int21() {
	const char *hex = "0123456789abcdef";
	uint8_t byte = inb(0x60);
	char buf[3] = { hex[byte >> 4], hex[byte & 0xf], 0 };
	print("INTERRUPT 0x21: 0x");
	print(buf);
	print("\n");
	outb(0x20, 0x20);
}

extern uint8_t int21_asm[];
extern char heap[];

void _Noreturn main() {
	int i; 
	uint32_t heap_size = (mb_info->mem_upper + mb_info->mem_lower);
	init_memory(heap, heap_size << 10);
	set_idt_entry(0x21, &int21_asm);
	init_pic();
	irq_enable(1);
	cls();
	if (0) {
	for (i = 0; i < 50; ++i) {
		put('0' + i % 10);
		print(" ZSO\n");
	}

	for (i = 0; i < 120; ++i) {
		put('#');
	}
	}

	asm ("sti\n");
	while (1) {
		asm ("hlt\n");
	}
	__builtin_unreachable();
}
