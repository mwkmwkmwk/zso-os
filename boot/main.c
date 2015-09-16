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
#include "threading/scheduler.h"
#include "utils/asm.h"

extern thread_entry user_main;

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
	uint intvar = 123;
	assert(atomic_read(&intvar) == 123);
	assert(atomic_add(&intvar, 3) == 123);
	assert(intvar == 126);
	assert(atomic_read(&intvar) == 126);

	assert(strcmp("123", "123") == 0);
	assert(strcmp("123", "223") == -1);
	assert(strcmp("223", "023") == 1);
	assert(strncmp("1234", "123", 3) == 0);
	assert(strncmp("1234", "123", 4) == 1);
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
	printf("--------------------------------------\n");
	printf("Self test passed!\n");
	printf("--------------------------------------\n");
}

noreturn void panic(const char *arg) {
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

int idling_thread(void* arg) {
	while (1) {
		asm volatile ("hlt");
	}
}

int kernel_main_thread(void* void_arg) {
	uint arg = (uint)void_arg;
	printf("Started kernel thread, arg = %x\n", arg);
	double test = 1.0;
	double test2 = 1.0;
	for (int i = 0; i < 100000; i++) {
		test *= test2;
		test2 += 1.0;
	}
	//printf("main work work work: %u\n", (uint)test);
	while (1) {
		// Do something here
		sleep(3000);
	}
}

int kernel_worker_thread(void* void_arg) {
	uint arg = (uint)void_arg;
	printf("Started kernel thread, arg = %x\n", arg);
	double test = 0.0;
	for (int i = 0; i < 100000; i++) {
		test += 1.0;
	}
	//printf("work work work: %u\n", (uint)test);
	//for (int i = 0; i < 10; i++) {
	while (1) {
		// Do something here
		sleep(500);
	}
	return 123;
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

	init_scheduler();
	create_kernel_thread(idling_thread,        (void*)0x12345678, "[idle]");
	create_kernel_thread(kernel_main_thread,   (void*)0x12345678, "Kernel main");
	create_kernel_thread(kernel_worker_thread, (void*)0xaaaaaaaa, "Kernel worker");
	create_user_thread(&user_main, (void*)0xca11ab1e, "User main");
	start_scheduling();

	panic("Error: This code should have never get reached!");
}
