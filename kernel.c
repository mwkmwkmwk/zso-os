#include <stddef.h>
#include <stdint.h>
#include "gdt.h"
#include "string.h"

static inline void outb(uint16_t port, uint8_t val) {
	asm volatile ("outb %0, %1" : : "a"(val), "d"(port));
}

void printf(char *format, ...) {
	while (*format) {
		char c = *format++;
		outb(0x3f8, c);
	}
}

struct regs {
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
};

void syscall(struct regs *regs) {
	switch (regs->eax) {
		case 0x1:
			outb(0x3f8, regs->ecx);
			break;
		default:
			printf("Unknown syscall\n");
			break;
	}
}

void main(void) {
	init_gdt();
	static char hello[2*80*25] = {
		'H', 0x0a,
		'e', 0x0a,
		'l', 0x0a,
		'l', 0x0a,
		'o', 0x0a,
		'.', 0x0a,
		' ', 0x0a,
		'w', 0x0a,
		'o', 0x0a,
		'r', 0x0a,
		'l', 0x0a,
		'd', 0x0a,
		'!', 0x0a,
	};
	memcpy((void*)0xb8000, hello, sizeof hello);
	printf("Hello, world!\n");
	asm(
		"movl $0x20, %eax\n"
		"movl %eax, %ds\n"
		"movl %eax, %es\n"
		"pushl $0x23\n"
		"pushl $0x00000000\n"
		"pushl $0x00000002\n"
		"pushl $0x1b\n"
		"pushl $0x1000\n"
		"iretl\n"
	);
}
