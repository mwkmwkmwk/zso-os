#include <stdint.h>

#include "user_syscalls.h"

int user_main() {
	syscall(SYS_TEST, 1, 2, 3, 4, 5);
	while (1);
}

uint8_t user_stack[4096] = { 0 };
