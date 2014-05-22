#include <stdint.h>
#include "print.h"

const __attribute__((section("header"))) uint32_t multiboot_header[] = {
	0x1BADB002,
	3,
	- 0x1BADB002 - 3,
};

uint8_t stack[0x1000];

struct gdt_entry {
	uint16_t limit_lo;
	uint16_t base_lo;
	uint8_t base_mid;
	uint8_t attr;
	uint8_t flags;
	uint8_t base_hi;
};

struct gdt_entry gdt[3] = {
	{ 0 },
	{ 0xffff, 0, 0, 0x9b, 0xcf, 0 },
	{ 0xffff, 0, 0, 0x93, 0xcf, 0 },
};

struct gdt_ptr {
	uint16_t _pad;
	uint16_t limit;
	struct gdt_entry *base;
} gdt_ptr = {
	0,
	sizeof gdt - 1,
	gdt,
};

asm (
	".text\n"
	".global _start\n"
	"_start:\n"
	"movl $stack+0x1000, %esp\n"
	"lgdt gdt_ptr+2\n"
	"movl $0x10, %eax\n"
	"movl %eax, %ds\n"
	"movl %eax, %es\n"
	"movl %eax, %ss\n"
	"ljmp $0x08,$main\n"
);

void _Noreturn main() {
	int i;

	cls();
	for (i = 0; i < 50; ++i) {
		put('0' + i % 10);
		print(" ZSO\n");
	}

	for (i = 0; i < 120; ++i) {
		put('#');
	}

	asm (
		"cli\n"
		"hlt\n"
	);
	__builtin_unreachable();
}
