#include "syscalls.h"

#include "common.h"
#include "io/interrupts.h"
#include "panic.h"
#include "stdlib/printf.h"

#include "syscall_table.h"

//
// Syscall interface
//
// syscall invocation: int INT_SYSCALL
// arguments:
// 		eax: syscall number
// 		ebx, ecx, edx, esi, edi: syscall arguments
// return value:
//		currently the only way to return something is to pass a pointer
// 		to a result in one of the arguments

struct syscall_handler_t syscall_table[] = {
	#define SYSCALL_TABLE_ARRAY_EXPAND
	#include "syscall_table.h"
	#undef SYSCALL_TABLE_ARRAY_EXPAND
};

void sys_hello() {
	printf("Hello, world\n");
}

void sys_test(int arg1, int arg2, int arg3, int arg4, int arg5) {
	printf("test syscall called with args: %u %u %u %u %u\n", arg1, arg2, arg3, arg4, arg5);
}

void syscall_entry(int eax, int ebx, int ecx, int edx, int esi, int edi) {
	if (eax < 0 || eax >= _countof(syscall_table))
		panic("Invalid syscall number called!");
	syscall_handler_func_t* handler = syscall_table[eax].handler;
	if (!handler)
		panic("Invalid syscall number called!");
	handler(ebx, ecx, edx, esi, edi);
}

void init_syscalls(void) {
	register_int_handler(INT_SYSCALL, syscall_entry, false, 3);
}