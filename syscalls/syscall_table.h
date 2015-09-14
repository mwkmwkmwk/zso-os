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
#define SYS_GETTIME	2
DEFINE_SYSCALL(gettime, SYS_GETTIME)
#define SYS_EXIT	3
DEFINE_SYSCALL(exit, SYS_EXIT)
#define SYS_PRINT	4
DEFINE_SYSCALL(print, SYS_PRINT)
#define SYS_CREATE_THREAD 5
DEFINE_SYSCALL(create_thread, SYS_CREATE_THREAD)
#define SYS_GET_KEYBOARD_FOCUS 6
DEFINE_SYSCALL(get_keyboard_focus, SYS_GET_KEYBOARD_FOCUS)
#define SYS_GET_KEY 7
DEFINE_SYSCALL(get_key, SYS_GET_KEY)

#ifndef SYSCALL_TABLE_ARRAY_EXPAND
	#ifndef _SYSCALL_TABLE_H_
	#define _SYSCALL_TABLE_H_

	#include "syscalls.h"

	#endif
#endif
