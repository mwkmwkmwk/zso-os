#include <stdint.h>

#include "user_syscalls.h"

void __attribute__((noreturn)) user_thread_entry(int (*start_addr)(void*), void* arg) {
	int exit_code = start_addr(arg);
	// TODO: sys_exit
	while (1) {}
}


int user_main(void* arg) {
	syscall(SYS_TEST, (int)arg, 2, 3, 4, 5);
	while (1) {
		//syscall(SYS_HELLO, 0, 0, 0, 0, 0);
		//syscall(SYS_SLEEP, 1000, 0, 0, 0, 0);
	}
}

uint8_t user_stack[4096] = { 0 };
