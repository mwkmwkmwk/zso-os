#include <stdint.h>
#include <stddef.h>
#include "idt.h"
#include "io.h"
#include "apic.h"

asm(
	".global _start\n"
	"_start:\n"
	"movabsq $stack+0x1000, %rsp\n"
	"call main\n"
	"cli\n"
	"hlt\n"
   );

extern uint16_t ekran[25][80];
int row = 2;
int col = 0;

extern char end;
void *mem = &end;

void *malloc(size_t sz) {
	void *res = (void*)(((uint64_t)mem + 0xfff) & ~0xfff);
	mem += sz;
	return res;
}

void printf(const char *p) {
	while (*p) {
		char c = *p++;
		if (c == '\n')
			row++, col = 0;
		else
			ekran[row][col++] = c | 0x0e00;
	}
}

void div0() {
	printf("Division by 0!\n");
	printf("System halted!\n");
	asm ("cli; hlt");
}

asm(
	".global irq_asm\n"
	"irq1_asm:\n"
	"pushq %rax\n"
	"pushq %rcx\n"
	"pushq %rdx\n"
	"pushq %rsi\n"
	"pushq %rdi\n"
	"pushq %r8\n"
	"pushq %r9\n"
	"pushq %r10\n"
	"pushq %r11\n"
	"call irq1\n"
	"popq %r11\n"
	"popq %r10\n"
	"popq %r9\n"
	"popq %r8\n"
	"popq %rdi\n"
	"popq %rsi\n"
	"popq %rdx\n"
	"popq %rcx\n"
	"popq %rax\n"
	"iretq\n"
);

void irq1() {
	printf("IRQ1\n");
	unsigned char c = inb(0x60);
	char buf[] = "Byte 0x?? received by IRQ\n";
	buf[8] = "0123456789abcdef"[c & 0xf];
	buf[7] = "0123456789abcdef"[c >> 4];
	printf(buf);
	write_lapic(0xb0, 0);
}

int getc() {
	while (!(inb(0x64) & 1));
	return inb(0x60);
}

#define PHYS_BASE 0xffff800000000000ull

void map_page_user(uint64_t virt, uint64_t phys) {
	uint64_t i1 = virt >> 12 & 0x1ff;
	uint64_t i2 = virt >> 21 & 0x1ff;
	uint64_t i3 = virt >> 30 & 0x1ff;
	uint64_t i4 = virt >> 39 & 0x1ff;
	uint64_t p4;
	uint64_t p3;
	uint64_t p2;
	uint64_t p1;
	asm (
		"movq %%cr3, %0\n"
		: "=r"(p4)
		:
	);
	uint64_t *pt4 = (void *)(PHYS_BASE + p4);
	if (!(pt4[i4] & 1)) {
		void *npt = malloc(0x1000);
		pt4[i4] = ((uint64_t)npt - PHYS_BASE) | 7;
	}
	p3 = pt4[i4] & ~0xfff;
	uint64_t *pt3 = (void *)(PHYS_BASE + p3);
	if (!(pt3[i3] & 1)) {
		void *npt = malloc(0x1000);
		pt3[i3] = ((uint64_t)npt - PHYS_BASE) | 7;
	}
	p2 = pt3[i3] & ~0xfff;
	uint64_t *pt2 = (void *)(PHYS_BASE + p2);
	if (!(pt2[i2] & 1)) {
		void *npt = malloc(0x1000);
		pt2[i2] = ((uint64_t)npt - PHYS_BASE) | 7;
	}
	p1 = pt2[i2] & ~0xfff;
	uint64_t *pt1 = (void *)(PHYS_BASE + p1);
	pt1[i1] = phys | 7;
}

void *memcpy(void *dst, const void *src, size_t sz) {
	char *cdst = dst;
	const char *csrc = src;
	while (sz--)
		*cdst++ = *csrc++;
	return dst;
}

asm(
	"enter_userspace:\n"
	"pushq $0x2b\n"
	"pushq $0\n"
	"pushq $0x200\n"
	"pushq $0x23\n"
	"pushq %rdi\n"
	"iretq\n"
);

int main() {
	init_gdt();
	init_idt();
	set_idt_entry(0, (uint64_t)&div0, true);
	printf("Hello, 64-bit world!\n");
	outb(0x20, 0x11);
	outb(0x21, 0x20);
	outb(0x21, 0x04);
	outb(0x21, 0x01);
	outb(0xa0, 0x11);
	outb(0xa1, 0x28);
	outb(0xa1, 0x02);
	outb(0xa1, 0x01);
	outb(0x21, 0xff);
	outb(0xa1, 0xff);
	write_lapic(0xf0, 0x1ff);
	extern char irq1_asm;
	set_idt_entry(0x31, (uint64_t)&irq1_asm, false);
	write_ioapic(0x12, 0x00000031);
	write_ioapic(0x13, 0x00000000);

	*(uint64_t *)0x8e000 = 0;
	asm (
		"movq %%cr3, %%rax\n"
		"movq %%rax, %%cr3\n"
		:
		:
		:
		"rax"
	);
	void *user_page = malloc(0x1000);
	map_page_user(0x123456789000, (uint64_t)user_page - PHYS_BASE);
	memcpy(user_page, "\xeb\xfe", 2);
	extern void enter_userspace(uint64_t);
	enter_userspace(0x123456789000);

	asm("sti");
	while(1) {
		asm("hlt");
	}
#if 0
	volatile int a = 1;
	volatile int b = 0;
	a /= b;
#endif
	while (1) {
		uint8_t c = getc();
		char buf[] = "Byte 0x?? received\n";
		buf[8] = "0123456789abcdef"[c & 0xf];
		buf[7] = "0123456789abcdef"[c >> 4];
		printf(buf);
	}
	return 0;
}

uint8_t stack[0x1000];
