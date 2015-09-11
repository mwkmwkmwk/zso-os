// Can be included from both kernel and user mode

// Undefines `DEFINE_SYSCALL()` when included with SYSCALL_TABLE_ARRAY_EXPAND, but that doesn't break anything

#ifdef SYSCALL_TABLE_ARRAY_EXPAND
	#undef DEFINE_SYSCALL
	#define DEFINE_SYSCALL(name, number) [number] = {sys_ ## name, #name},
#else
	#ifdef USERMODE
		#define DEFINE_SYSCALL(name, number)
	#else
		#define DEFINE_SYSCALL(name, number) syscall_handler_func_t sys_ ## name;
	#endif
#endif

#define SYS_HELLO	0
DEFINE_SYSCALL(hello, SYS_HELLO)
#define SYS_TEST	1
DEFINE_SYSCALL(test, SYS_TEST)

#ifndef SYSCALL_TABLE_ARRAY_EXPAND
	#ifndef _SYSCALL_TABLE_H_
	#define _SYSCALL_TABLE_H_

	#include "syscalls.h"

	#endif
#endif