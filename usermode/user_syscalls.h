#pragma once

#ifndef USERMODE
#error This file should be included only from usermode code!
#endif

#include "syscalls/syscall_table.h"

void syscall(int number, int arg1, int arg2, int arg3, int arg4, int arg5);
