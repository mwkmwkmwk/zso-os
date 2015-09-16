#include "user_syscalls.h"

#include <stdint.h>

#include "io/interrupts.h"
#include "stdlib/math.h"

uint8_t user_stack[4096 * 10] = { 0 }; // Primitive, temporary hack

// Every new user thread starts its execution here
void noreturn user_thread_entry(int (*start_addr)(void*), void* arg) {
	int exit_code = start_addr(arg);
	user_sys_exit(exit_code);
}

ull syscall(int number, int arg1, int arg2, int arg3, int arg4, int arg5) {
	int eax, edx;
	asm volatile (
		"movl %3, %%eax \n"
		"movl %4, %%ebx \n"
		"movl %5, %%ecx \n"
		"movl %6, %%edx \n"
		"movl %7, %%esi \n"
		"movl %8, %%edi \n"
		"int %2 \n"
		: "=a"(eax), "=d"(edx)
		: "i"(INT_SYSCALL), "m"(number), "m"(arg1), "m"(arg2), "m"(arg3), "m"(arg4), "m"(arg5)
		: "%ebx", "%ecx", "%esi", "%edi"
	);
	return ((ull)edx << 32) + eax;
}

// Syscall wrappers

void user_sys_hello() {
	syscall(SYS_HELLO, 0, 0, 0, 0, 0);
}

void user_sys_test(int arg1, int arg2, int arg3, int arg4, int arg5) {
	syscall(SYS_TEST, arg1, arg2, arg3, arg4, arg5);
}

void user_sys_gettime(double* out_sec) {
	syscall(SYS_GETTIME, (uint)out_sec, 0, 0, 0, 0);
}

void noreturn user_sys_exit(int exit_code) {
	syscall(SYS_EXIT, exit_code, 0, 0, 0, 0);
	while (1) {} // Should never happen
}

void user_sys_print(const char* text) {
	syscall(SYS_PRINT, (int)text, 0, 0, 0, 0);
}

void user_sys_create_thread(void* start, void* arg, const char* name) {
	syscall(SYS_CREATE_THREAD, (int)start, (int)arg, (int)name, 0, 0);
}

void user_sys_get_keyboard_focus(void) {
	syscall(SYS_GET_KEYBOARD_FOCUS, 0, 0, 0, 0, 0);
}

bool user_sys_get_key(struct keyboard_event* out_event) {
	return syscall(SYS_GET_KEY, (int)out_event, 0, 0, 0, 0);
}

// Helper functions

void user_yield() {
	asm volatile (
		"int %0"
		:
		: "i"(INT_YIELD)
	);
}

void user_sleep(double sec) {
	double start_time, current;
	user_sys_gettime(&start_time);
	while (1) {
		user_sys_gettime(&current);
		if (current - start_time >= sec)
			break;
		user_yield();
	}
}

void user_read_line(char* buf, int buf_size) {
	int i = 0;
	while (i < buf_size - 1) {
		struct keyboard_event event;
		if (!user_sys_get_key(&event)) {
			user_yield();
		} else {
			if (event.ascii) {
				char tmpbuf[2];
				tmpbuf[0] = event.ascii;
				tmpbuf[1] = 0;
				user_sys_print(tmpbuf);
				// We prefer ascii over virt. keys (e.g. to not differentiate between two enters)
				if (event.ascii == '\n')
					break;
				if (event.ascii == '\b')
					i = max(0, i - 1);
				else
					buf[i++] = event.ascii;
			}
		}
	}
	buf[i] = 0;
}

// Temporary, shameless copy-paste from string.c
// I don't want to make a syscall from it. Will be solved by
// compiling user code separately or changing linker script.
int user_strncmp(const char* str1, const char* str2, size_t max_len) {
	int i = 0;
	for (; i < max_len; i++) {
		if (!*str1 || *str1 != *str2)
			break;
		str1++;
		str2++;
	}
	if (i == max_len) {
		str1--;
		str2--;
	}
	return *str1 == *str2 ? 0 : (*str1 < *str2 ? -1 : 1);
}

void user_strrev(char* str) {
	int len = user_strlen(str);
	int i;
	char tmp;
	for (i = 0; i < len/2; i++) {
		tmp = str[i];
		str[i] = str[len - i - 1];
		str[len - i - 1] = tmp;
	}
}

size_t user_strlen(const char* str) {
	size_t len = 0;
	while (*str++)
		len++;
	return len;
}
