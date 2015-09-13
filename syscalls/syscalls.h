#pragma once

#include "common.h"

typedef ull syscall_handler_func_t();

struct syscall_handler_t {
	syscall_handler_func_t* handler;
	const char* name;
};

void init_syscalls(void);
