#include <stdint.h>
#include "vga.h"

const __attribute__((section("header"))) uint32_t multiboot_header[] = {
	0x1BADB002,
	3,
	- 0x1BADB002 - 3,
};

uint8_t stack[0x1000];

asm (
	".text\n"
	".global _start\n"
	"_start:\n"
	"movl $stack+0x1000, %esp\n"
	"jmp main\n"
);

void _Noreturn main() {
	*(uint32_t *)0xb8000 = 0x0f530f5a;
	*(uint32_t *)0xb8004 = 0x0f200f4f;
	vga_set_cursor_addr(0);
	asm (
		"cli\n"
		"hlt\n"
	);
	__builtin_unreachable();
}
