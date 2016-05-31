#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include "gdt.h"
#include "string.h"
#include "kernel.h"
#include "io.h"

void printf(char *format, ...) {
	int i;
	uint32_t n;
	va_list va;
	va_start(va, format);
	while (*format) {
		char c = *format++;
		if (c == '%') {
			c = *format++;
			switch (c) {
				case 'x':
					n = va_arg(va, uint32_t);
					outb(0x3f8, '0');
					outb(0x3f8, 'x');
					for (i = 7; i >= 0; i--)
						outb(0x3f8, "0123456789abcdef"[n >> (i * 4) & 0xf]);
					break;
			}
		} else {
			outb(0x3f8, c);
		}
	}
	va_end(va);
}

_Noreturn void panic(char *reason) {
	printf(reason);
	asm("cli; hlt");
	__builtin_unreachable();
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

void pagefault(struct regs *regs, uint32_t vaddr) {
	printf("page fault on %x\n", vaddr);
	panic("pagefault");
}

void irq1(void) {
	uint8_t val = inb(0x60);
	printf("irq1, %x\n", val);
	outb(0x20, 0x20);
}

extern uint8_t user[1];

void main(void) {
	init_gdt();
	init_paging();
	uint32_t entry = load_user(user);
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
	memcpy((void*)0x000b8000, hello, sizeof hello);
	printf("Hello, world!\n");
	asm volatile(
		"movl $0x20, %%eax\n"
		"movl %%eax, %%ds\n"
		"movl %%eax, %%es\n"
		"pushl $0x23\n"
		"pushl $0x00000000\n"
		"pushl $0x00000202\n"
		"pushl $0x1b\n"
		"pushl %%ecx\n"
		"iretl\n"
		:
		: "c"(entry)
	);
}
