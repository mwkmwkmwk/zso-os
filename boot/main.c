#include <stdbool.h>

#include "stdlib/assert.h"
#include "boot/mb.h"
#include "common.h"
#include "io/interrupts.h"
#include "io/io.h"
#include "io/keyboard.h"
#include "io/pic.h"
#include "io/pit.h"
#include "io/time.h"
#include "mem/gdt.h"
#include "mem/page.h"
#include "mem/pmalloc.h"
#include "mem/kalloc.h"
#include "panic.h"
#include "stdlib/printf.h"
#include "stdlib/string.h"
#include "syscalls/syscalls.h"
#include "threading/context.h"
#include "utils/asm.h"

void self_test(void) {
	asm volatile(
		"movl $0, %eax \n"
		"int $0x20 \n" // Hello world!
	);

	// printf tests
	printf("123 == %u\n", 123);
	printf("0x123def == 0x%x\n", 0x123def);
	printf("'x' == '%c'\n", 'x');
	printf("\"asdf%%123\" == \"%s\"\n", "asdf%123");

	ull var = 123;
	assert(atomic_read_8b(&var) == 123);
	assert(atomic_add_8b(&var, 3) == 123);
	assert(var == 126);
	assert(atomic_read_8b(&var) == 126);

	// printf("Sleeping for 1s... ");
	// active_sleep(1000);
	// printf("Done!\n");
	// Div zero test
	// {
	// 	volatile int zero = 0;
	// 	volatile int res = zero / zero;
	// }

	// Page fault test
	// {
	// 	char* ptr = (char*)0x90EEDDCC;
	// 	volatile int res = ptr[0];
	// }

	// kalloc test
	char* ptr = kalloc(10);
	memset(ptr, 1, 10);
	kfree(ptr);
	printf("Self test passed!\n");
}

_Noreturn void panic(const char *arg) {
	puts(arg);
	puts("\n");
	while (1) {
		asm volatile("cli; hlt");
	}
}

void div_zero(struct context** context_ptr) {
	panic("Division by 0 - system halted\n");
}

void err_gp(struct context** context_ptr) {
	panic("General protection - system halted\n");
}

void err_page(struct context** context_ptr) {
	uint32_t cr2;
	asm volatile (
		"movl %%cr2, %0\n"
		: "=r"(cr2)
		:
		:
	);
	printf("Page fault (%x) - system halted\n", cr2);
	asm volatile("cli; hlt");
}

void main(struct mb_header *mbhdr) {
	init_gdt();
	init_pmalloc(mbhdr);
	init_paging();
	init_kalloc();
	init_syscalls();
	register_int_handler(INT_DIV_ERROR,  div_zero,  false, 0);
	register_int_handler(INT_GEN_PROT,   err_gp,    false, 0);
	register_int_handler(INT_PAGE_FAULT, err_page,  false, 0);
	init_pic();
	init_keyboard();
	init_timers();
	self_test();

	// Jump to usermode
	asm volatile(
		"mov $0x00, %%ax\n"
		"mov %%ax, %%ds\n"
		"mov %%ax, %%es\n"
		"mov %%ax, %%fs\n"
		"mov %%ax, %%gs\n"
		"pushl $0x2b\n"            // SS
		"pushl $user_stack+4096\n" // ESP
		"pushl %0\n"               // EFLAGS 
		"pushl $0x23\n"            // ret segment
		"pushl $user_main\n"       // ret offset
		"iretl\n"
		:
		: "i"(2 | EFLAGS_IF) // EFLAGS (reserved bit at pos 1)
	);

	printf("Error: This code should have never get reached!\n");
	while (1) {
		asm volatile ("hlt");
	}
}
