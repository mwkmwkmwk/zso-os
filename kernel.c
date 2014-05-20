#include <stdint.h>
#include "print.h"

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
