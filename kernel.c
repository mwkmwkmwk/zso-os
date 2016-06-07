#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include "gdt.h"
#include "string.h"
#include "kernel.h"
#include "io.h"
#include "proc.h"

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

void print_num(int x, int y, uint32_t num) {
	char s[10];
	if (x >= 80 || y >= 25)
		return;
	int i = 0;
	do {
		s[i++] = '0' + num % 10;
		num /= 10;
	} while (num);
	uint16_t *scr = (void *)0xb8000;
	scr += 80 * y + x;
	int j;
	for (j = 0; j < i; j++)
		scr[j] = s[i-1-j] | 0x0f00;
}

uint8_t buf[0x100];
uint8_t wptr, rptr;

void switch_to_something(void) {
	struct process *next = proc_cur;
	do {
		next = next->next;
		if (!next)
			next = proc_root;
	} while (next->key_blocked && rptr == wptr);
	switch_process(next);
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
		case 0x2:
			print_num(regs->ecx, regs->edx, regs->ebx);
			break;
		case 0x3:
			asm("cli");
			while (rptr == wptr) {
				proc_cur->key_blocked = true;
				switch_to_something();
			}
			proc_cur->key_blocked = false;
			regs->eax = buf[rptr++];
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

void irq0(void) {
	outb(0x20, 0x20);
	switch_to_something();
}

void irq1(void) {
	uint8_t val = inb(0x60);
	printf("irq1, %x\n", val);
	if ((uint8_t)(wptr + 1) != rptr)
		buf[wptr++] = val;
	else
		printf("oops\n");
	outb(0x20, 0x20);
}

extern uint8_t user[1];
extern uint8_t user2[1];

void user1_proc_fun(void *init_proc) {
	uint32_t entry = load_user(user);
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

void user2_proc_fun(void *init_proc) {
	uint32_t entry = load_user(user2);
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

void main(void) {
	init_gdt();
	init_paging();
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
	struct process *user1_proc = start_process(user1_proc_fun, proc_cur);
	struct process *user2_proc = start_process(user2_proc_fun, proc_cur);
	uint32_t cnt = 0;
	outb(0x61, (inb(0x61) & ~0x3) | 3);
	while(1)
		print_num(40, 10, cnt++);
}
