#include "user_syscalls.h"

#include "io/interrupts.h"

void syscall(int number, int arg1, int arg2, int arg3, int arg4, int arg5) {
	asm volatile (
		"movl %1, %%eax \n"
		"movl %2, %%ebx \n"
		"movl %3, %%ecx \n"
		"movl %4, %%edx \n"
		"movl %5, %%esi \n"
		"movl %6, %%edi \n"
		"int %0 \n"
		:
		: "i"(INT_SYSCALL), "m"(number), "m"(arg1), "m"(arg2), "m"(arg3), "m"(arg4), "m"(arg5)
		:
	);
}

void user_sys_hello() {
	syscall(SYS_HELLO, 0, 0, 0, 0, 0);
}

void user_sys_test(int arg1, int arg2, int arg3, int arg4, int arg5) {
	syscall(SYS_TEST, arg1, arg2, arg3, arg4, arg5);
}

void user_sys_sleep(ull ms) {
	syscall(SYS_SLEEP, (uint)(ms >> 32), (uint)ms, 0, 0, 0);
}

void __attribute__((noreturn)) user_sys_exit(int exit_code) {
	syscall(SYS_EXIT, exit_code, 0, 0, 0, 0);
	while (1) {} // Should never happen
}
