#include "syscalls.h"

#include "common.h"
#include "io/interrupts.h"
#include "io/keyboard.h"
#include "io/time.h"
#include "panic.h"
#include "stdlib/assert.h"
#include "stdlib/printf.h"
#include "syscalls/syscall_table.h"
#include "threading/context.h"
#include "threading/scheduler.h"
#include "utils/asm.h"

//
// Syscall interface
//
// syscall invocation: int INT_SYSCALL
// arguments:
// 		eax: syscall number
// 		ebx, ecx, edx, esi, edi: syscall arguments
// return value:
//		edx:eax (64 bit)

struct syscall_handler_t syscall_table[] = {
	#define SYSCALL_TABLE_ARRAY_EXPAND
	#include "syscall_table.h"
	#undef SYSCALL_TABLE_ARRAY_EXPAND
};

static void syscall_entry(struct context** context_ptr) {
	struct context* context = *context_ptr;
	if (context->eax < 0 || context->eax >= _countof(syscall_table))
		panic("Invalid syscall number called!");
	syscall_handler_func_t* handler = syscall_table[context->eax].handler;
	if (!handler)
		panic("Invalid syscall number called!");
	ull res = handler(context->ebx, context->ecx, context->edx, context->esi, context->edi);
	context->edx = (res >> 32);
	context->eax = (uint)res;
}

static void yield_syscall_entry(struct context** context_ptr) {
	yield_from_irq(context_ptr);
}

void init_syscalls(void) {
	register_int_handler(INT_SYSCALL, syscall_entry, false, 3);
	register_int_handler(INT_YIELD, yield_syscall_entry, false, 3);
}

ull sys_hello() {
	printf("Hello, world\n");
	return 0;
}

ull sys_test(int arg1, int arg2, int arg3, int arg4, int arg5) {
	printf("test syscall called with args: %x %x %x %x %x\n", arg1, arg2, arg3, arg4, arg5);
	return 0;
}

ull sys_gettime(double* out_sec) {
	*out_sec = get_current_time() / (double)(1ull << 32);
	return 0;
}

ull sys_exit(int exit_code) {
	kill_current_thread(exit_code);
	assert(!"Reached unreachable code!");
}

ull sys_print(const char* text) {
	puts(text);
	return 0;
}

ull sys_create_thread(void* start, void* arg, const char* name) {
	create_user_thread(start, arg, name);
	return 0;
}

ull sys_get_keyboard_focus(void) {
	set_active_key_buffer(&current_thread->keyboard_buffer);
	return 0;
}

ull sys_get_key(struct keyboard_event* out_event) {
	return pull_key_event(&current_thread->keyboard_buffer, out_event);
}
