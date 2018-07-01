#include <stdint.h>
#include <stddef.h>
#include <elf.h>
#include "gdt.h"
#include "msr.h"
#include "page.h"
#include "io.h"
#include "stdio.h"

char hello[] = "Hello, C world!";

void *memset(void *s, int c, size_t n) {
	unsigned char *p = s;
	while (n--)
		*p++ = c;
	return s;
}

void *memcpy(void *s, void *ss, size_t n) {
	unsigned char *p = s;
	unsigned char *pp = ss;
	while (n--)
		*p++ = *pp++;
	return s;
}

__asm__(
	".text\n"
	"go_user:\n"
	"pushq $0x1b\n"
	"pushq %rsi\n"
	"pushq $0x2\n"
	"pushq $0x23\n"
	"pushq %rdi\n"
	"iretq\n"
);

__asm__(
	"lstar:\n"
	"pushq %rcx\n"
	"pushq %r8\n"
	"pushq %r9\n"
	"pushq %r11\n"
	"pushq %r10\n"
	"pushq %rdx\n"
	"pushq %rsi\n"
	"pushq %rdi\n"
	"movq %rax, %rdi\n"
	"movq %rsp, %rsi\n"
	"call csyscall\n"
	"popq %rdi\n"
	"popq %rsi\n"
	"popq %rdx\n"
	"popq %r10\n"
	"popq %r11\n"
	"popq %r9\n"
	"popq %r8\n"
	"popq %rcx\n"
	"sysretq\n"
);

__asm__(
	"cstar:\n"
	"cli\n"
	"hlt\n"
);

extern void lstar;
extern void cstar;

_Noreturn void go_user(void *, uint64_t);

void sys_print(unsigned x, unsigned y, uint16_t *chars, unsigned n) {
	unsigned i;
	for (i = 0; i < n; i++) {
		int c = chars[i];
		if (x < 80 && y < 25) {
			volatile uint16_t *screen = (void *)0xffff8000000b8000ull;
			screen[y * 80 + x] = c;
		}
		x++;
	}
}

void sys_setcursor(unsigned x, unsigned y) {
	setcursor(x, y);
}

int kbd_getbyte(void) {
	while (!(inb(0x64) & 1));
	return inb(0x60);
}

int kbd_getcode(void) {
	int k = kbd_getbyte();
	if (k != 0xe0)
		return k;
	return kbd_getbyte() | 0x100;
}

__asm__(
	".text\n"
	"sys_getrand:\n"
	"rdrand %eax\n"
	"jnc sys_getrand\n"
	"retq\n"
);
int sys_getrand(void);

int sys_getkey(void) {
	int k;
	while (1) {
		k = kbd_getcode();
		switch (k) {
			case 0x1c:
				return '\n';
			case 0x1f:
				return 's';
			case 0x39:
				return ' ';
			case 0x14b:
				return 0x81;
			case 0x14d:
				return 0x83;
			case 0x148:
				return 0x80;
			case 0x150:
				return 0x82;
		}
	}
}

uint64_t csyscall(uint64_t func, uint64_t *regs) {
	switch (func) {
		case 0:
			outb(0x64, 0xfe);
			while (1);
			return 0;
		case 1:
			return sys_getrand();
		case 2:
			return sys_getkey();
		case 3:
			sys_print(regs[0], regs[1], (void *)regs[2], regs[3]);
			return 0;
		case 4:
			sys_setcursor(regs[0], regs[1]);
			return 0;
		default:
			return -1;
	}
}

__asm__(
	".section .rodata\n"
	"user_prog:\n"
	".incbin \"prog\"\n"
	".text\n"
);

extern void user_prog;

void main() {
	clrscr();
	for (int i = 97; i < 127; i++) {
		printf("printf demo: char=%c, dec=%d, hex=%x, str comment = %s\n", i, i, i, "\"no comment\"");
	}

	init_gdt();
	init_pg();
	// ELF loader start
	Elf64_Ehdr *ehdr = &user_prog;
	int i;
	for (i = 0; i < ehdr->e_phnum; i++) {
		Elf64_Phdr *phdr = &user_prog + ehdr->e_phoff + ehdr->e_phentsize * i;
		if (phdr->p_type == PT_LOAD) {
			uint64_t va = phdr->p_vaddr & ~0xfffull;
			uint64_t end = phdr->p_vaddr + phdr->p_memsz;
			for (uint64_t p = va; p < end; p += 0x1000) {
				map_page(p, phys_alloc(), 7);
			}
			memset((void*)phdr->p_vaddr, 0, phdr->p_memsz);
			memcpy((void*)phdr->p_vaddr, &user_prog + phdr->p_offset, phdr->p_filesz);
		}
	}
	for (i = 0; i < ehdr->e_phnum; i++) {
		Elf64_Phdr *phdr = &user_prog + ehdr->e_phoff + ehdr->e_phentsize * i;
		if (phdr->p_type == 0x60031337) {
			*(uint32_t *)phdr->p_vaddr = 200;
		}
	}
#if 0
	uint8_t *upage = (void *)0x31337000ull;
	upage[0] = 0xeb;
	upage[1] = 0xfe;
#endif
	wrmsr(0xC0000081, 0x0013000800000000ull);
	wrmsr(0xC0000082, (uint64_t)&lstar);
	wrmsr(0xC0000083, (uint64_t)&cstar);
	wrmsr(0xC0000084, 0xffffffff);

#if 0
	int x = 0;
	while (1) {
		int k = sys_getkey();
		k = sys_getrand();
		uint16_t c[2] = {
			0x0f00 | (k >> 4 & 0xf)["0123456789abcdef"],
			0x0f00 | (k & 0xf)["0123456789abcdef"],
		};
		sys_print(x, 0, c, 2);
		x += 2;
		x %= 80;
		sys_setcursor(x, 0);
	}
#endif
	sys_getkey();
	for (int i = 0x1000; i < 0x100000; i += 0x1000)
		map_page(i, phys_alloc(), 7);
	go_user((void *)ehdr->e_entry, 0x100000);
	for (int i = 0; i < sizeof hello; i++) {
		*(uint16_t *)(0x31337000 + i * 2) = hello[i] | 0x0f00;
	}
}
