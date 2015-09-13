#include <stdint.h>

#include "common.h"
#include "user_syscalls.h"

void __attribute__((noreturn)) user_thread_entry(int (*start_addr)(void*), void* arg) {
	int exit_code = start_addr(arg);
	user_sys_exit(exit_code);
}


int user_main(void* arg) {
	//user_sys_hello();
	user_sys_test((int)arg, 2, 3, 4, 5);
	for (int i = 0; i < 10; i++) {
		user_sys_test(i, 2, 3, 4, 5);
		user_sleep(0.7);
	}
	return 4321;
}

uint8_t user_stack[4096] = { 0 };
