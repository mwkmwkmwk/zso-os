#include <stdint.h>
#include "print.h"
#include "pic.h"
#include "io.h"
#include "thread.h"

// put ugly test code in this file
#include "main_debug.h"

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

struct reg_store {
	uint32_t gs;
	uint32_t fs;
	uint32_t ss;
	uint32_t ds;
	uint32_t es;
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	uint8_t esp[0];
};

void store_regs(struct reg_store *p) {
	printf("STORE %p\n", p);
	current_thread->eax = p->eax;
	current_thread->ecx = p->ecx;
	current_thread->edx = p->edx;
	current_thread->ebx = p->ebx;
	current_thread->ebp = p->ebp;
	current_thread->esi = p->esi;
	current_thread->edi = p->edi;
	current_thread->eip = p->eip;
	current_thread->esp = (uint32_t)&p->esp;
	current_thread->es = p->es;
	current_thread->cs = p->cs;
	current_thread->ss = p->ss;
	current_thread->ds = p->ds;
	current_thread->fs = p->fs;
	current_thread->gs = p->gs;
	current_thread->eflags = p->eflags;
};

void *restore_regs() {
	struct reg_store *ptr = (void *)(current_thread->esp - sizeof *ptr);
	printf("RESTORE %p\n", ptr);
	ptr->eax = current_thread->eax;
	ptr->ecx = current_thread->ecx;
	ptr->edx = current_thread->edx;
	ptr->ebx = current_thread->ebx;
	ptr->ebp = current_thread->ebp;
	ptr->esi = current_thread->esi;
	ptr->edi = current_thread->edi;
	ptr->eip = current_thread->eip;
	ptr->es = current_thread->es;
	ptr->cs = current_thread->cs;
	ptr->ss = current_thread->ss;
	ptr->ds = current_thread->ds;
	ptr->fs = current_thread->fs;
	ptr->gs = current_thread->gs;
	ptr->eflags = current_thread->eflags;
	return ptr;
};

asm (
	".text\n"
	".global int21_asm\n"
	"int21_asm:\n"
	"pushl %eax\n"
	"pushl %ecx\n"
	"pushl %edx\n"
	"pushl %ebx\n"
	"pushl %ebp\n"
	"pushl %esi\n"
	"pushl %edi\n"
	"pushl %es\n"
	"pushl %ds\n"
	"pushl %ss\n"
	"pushl %fs\n"
	"pushl %gs\n"
	"pushl %esp\n"
	"call store_regs\n"
	"call int21\n"
	"call restore_regs\n"
	"movl %eax, %esp\n"
	"popl %gs\n"
	"popl %fs\n"
	"popl %ss\n"
	"popl %ds\n"
	"popl %es\n"
	"popl %edi\n"
	"popl %esi\n"
	"popl %ebp\n"
	"popl %ebx\n"
	"popl %edx\n"
	"popl %ecx\n"
	"popl %eax\n"
	"iretl\n"
);

void switch_thread() {
	struct thread *orig_thread = current_thread;
	while (1) {
		current_thread = current_thread->next;
		if (!current_thread->blocked)
			break;
		if (current_thread == orig_thread) {
			current_thread = &idle_thread;
			break;
		}
	}
	printf("%x\n", current_thread);
}

void int21() {
	const char *hex = "0123456789abcdef";
	uint8_t byte = inb(0x60);
	char buf[3] = { hex[byte >> 4], hex[byte & 0xf], 0 };
	print("INTERRUPT 0x21: 0x");
	print(buf);
	print("\n");
	outb(0x20, 0x20);
	if (byte & 0x80)
		switch_thread();
}

extern uint8_t int21_asm[];

volatile int counter = 0;
uint8_t stack_a[0x1000];
uint8_t stack_b[0x1000];

struct thread thread_a;
struct thread thread_b;

void _Noreturn func_a(void *param) {
	printf("F_A\n");
	while(1)
		counter++;
}

void _Noreturn func_b(void *param) {
	printf("F_B\n");
	while(1) {
		printf("%x\n", counter);
	}
}

void _Noreturn main() {
	int i;
	set_idt_entry(0x21, &int21_asm);
	init_pic();
	irq_enable(1);
	init_screen();

	main_debug();

	start_thread(&thread_a, stack_a + sizeof stack_a, func_a, 0);
	start_thread(&thread_b, stack_b + sizeof stack_b, func_b, 0);
	asm ("sti\n");
	while (1) {
		asm ("hlt\n");
	}
	__builtin_unreachable();
}
