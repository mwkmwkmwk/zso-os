#pragma once

#ifndef USERMODE
#error This file should be included only from usermode code!
#endif

#include "common.h"
#include "syscalls/syscall_table.h"

void syscall(int number, int arg1, int arg2, int arg3, int arg4, int arg5);

// Syscalls
void user_sys_hello();
void user_sys_test(int arg1, int arg2, int arg3, int arg4, int arg5);
void __attribute__((noreturn)) user_sys_exit(int exit_code);

// Some helper functions
void user_sys_gettime(double* out_sec);
void user_sleep(double sec);
