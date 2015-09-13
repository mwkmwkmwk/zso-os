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
		: "%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi"
	);
}

void user_sys_hello() {
	syscall(SYS_HELLO, 0, 0, 0, 0, 0);
}

void user_sys_test(int arg1, int arg2, int arg3, int arg4, int arg5) {
	syscall(SYS_TEST, arg1, arg2, arg3, arg4, arg5);
}

void user_sys_gettime(double* out_sec) {
	syscall(SYS_GETTIME, (uint)out_sec, 0, 0, 0, 0);
}

void user_yield() {
	asm volatile (
		"int %0"
		:
		: "i"(INT_YIELD)
	);
}

void user_sleep(double sec) {
	double start_time, current;
	user_sys_gettime(&start_time);
	while (1) {
		user_sys_gettime(&current);
		if (current - start_time >= sec)
			break;
		user_yield();
	}
}

void __attribute__((noreturn)) user_sys_exit(int exit_code) {
	syscall(SYS_EXIT, exit_code, 0, 0, 0, 0);
	while (1) {} // Should never happen
}
