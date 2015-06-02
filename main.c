#include "vga.h"
#include "gdt.h"
#include "pic.h"
#include "io.h"

void printf(const char *str) {
	struct vga_char *cur = (void *)((char *)framebuffer + get_cursor());
	int i;
	for (i = 0; str[i]; i++) {
		cur[i].ch = str[i];
		cur[i].attr = 0x0a;
	}
	set_cursor((char *)(cur + i) - (char *)framebuffer);
}

void sys_hello() {
	printf("Hello, world\n");
}

void div_zero() {
	printf("Division by 0 - system halted\n");
	asm volatile("cli\nhlt");
}

void irq1() {
	printf("KEY\n");
	inb(0x60);
	outb(0x20, 0x20);
}

extern char asm_sys_hello[];
extern char asm_div_zero[];
extern char asm_irq1[];

void main() {
	init_gdt();
	set_idt(0x20, (uint32_t)asm_sys_hello, 0);
	set_idt(0x00, (uint32_t)asm_div_zero, 0);
	init_pic();
	set_idt(0xf1, (uint32_t)asm_irq1, 1);
	outb(0x21, 0xfd);
	asm volatile(
		"int $0x20\n"
		:::
	);
	while (1) {
		asm volatile ("hlt":::);
	}
}
