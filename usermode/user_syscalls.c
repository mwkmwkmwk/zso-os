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