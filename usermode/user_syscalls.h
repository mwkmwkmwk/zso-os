#pragma once

#ifndef USERMODE
#error This file should be included only from usermode code!
#endif

#include "common.h"
#include "io/keyboard.h"
#include "syscalls/syscall_table.h"

ull syscall(int number, int arg1, int arg2, int arg3, int arg4, int arg5);

// Syscalls
void user_sys_hello();
void user_sys_test(int arg1, int arg2, int arg3, int arg4, int arg5);
void user_sys_gettime(double* out_sec);
void __attribute__((noreturn)) user_sys_exit(int exit_code);
void user_sys_print(const char* text);
void user_sys_create_thread(void* start, void* arg, const char* name);
void user_sys_get_keyboard_focus(void);
bool user_sys_get_key(struct keyboard_event* out_event);

// Some helper functions
void user_yield();
void user_sleep(double sec);
