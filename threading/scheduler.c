#include "scheduler.h"

#include "io/interrupts.h"
#include "io/time.h"
#include "mem/kalloc.h"
#include "stdlib/assert.h"
#include "stdlib/printf.h"
#include "stdlib/string.h"
#include "utils/asm.h"

struct list thread_list_head;
struct thread* current_thread;
extern void __attribute__((noreturn)) user_thread_entry(int (*)(void*), void* );
extern char user_stack[];

static volatile uint next_user_stack = 0;

static struct thread* select_next_thread() {
	for (struct list* it = current_thread->list_node.next;
	     it != &current_thread->list_node;
	     it = it->next)
	{
		struct thread* thread = (struct thread*)it;
		if (thread->state == READY)
			return thread;
	}
	// No other thread was ready to be scheduled, reschedule the same thread
	return current_thread;
}

void set_current_thread(struct thread* thread) {
	// Beware: `thread` can be equal to `current_thread`
	current_thread->state = READY;
	current_thread->scheduled_on = -1;
	thread->scheduled_on = get_current_time();
	thread->state = RUNNING;
	current_thread = thread;
}

void sched_tick(struct context** context_ptr) {
	ull time = get_current_time();
	if (time - current_thread->scheduled_on >= SCHED_QUANT
		|| current_thread->state == EXITING)
	{
		// Switch to a next thread from the list
		yield_from_irq(context_ptr);
	}
}

void yield_from_irq(struct context** context_ptr) {
	if (current_thread->state == EXITING){
		list_unlink(&current_thread->list_node);
		kfree(current_thread);
	} else {
		memcpy(&current_thread->context, *context_ptr, sizeof(struct context));
	}
	struct thread* next_thread = select_next_thread();
	set_current_thread(next_thread);
	*context_ptr = &next_thread->context;
}

// Callable only from non-IRQ context
void yield(void) {
	asm volatile (
		"int %0"
		:
		: "i"(INT_YIELD)
	);
}

void __attribute__((noreturn)) start_scheduling(void) {
	disable_interrupts();
	add_tick_callback(sched_tick);

	current_thread = (struct thread*)thread_list_head.next;
	assert("No threads to schedule!" && current_thread != (void*)&thread_list_head);
	set_current_thread(current_thread);
	load_context(&current_thread->context);
}

void kernel_thread_entry(thread_entry* start_addr, void* arg) {
	printf("Thread `%s` created\n", current_thread->name);
	int exit_code = start_addr(arg);
	kill_current_thread(exit_code);
}

void __attribute__((noreturn)) kill_current_thread(int exit_code) {
	printf("Thread `%s` exited with exit code %u\n", current_thread->name, exit_code);
	bool iflag = disable_interrupts();
	current_thread->state = EXITING;
	set_int_flag(iflag);
	while (1) {} // Current thread will disappear on next timer tick
}

void create_kernel_thread(thread_entry* address, void* arg, const char* name) {
	struct thread* thread = aligned_kalloc(sizeof(struct thread), 16);
	thread->state = READY;
	thread->scheduled_on = -1;
	strncpy(thread->name, name, 32);
	thread->context.cs = get_cs();
	thread->context.ds = get_ds();
	thread->context.es = get_es();
	thread->context.fs = get_fs();
	thread->context.gs = get_gs();
	thread->context.ss = get_ss();
	thread->context.eflags = EFLAGS_IF | 2;
	thread->context.eip = (uint)kernel_thread_entry;
	fxstate_init(&thread->context.fxstate);

	// TODO: add guard pages to detect over/underflows
	uint* stack = (uint*)((char*)kalloc(THREAD_STACK_SIZE) + THREAD_STACK_SIZE);
	*(--stack) = (uint)arg;
	*(--stack) = (uint)address;
	*(--stack) = 0xDEADBEEF;
	thread->context.esp = (uint)stack;

	bool iflag = disable_interrupts();
	list_insert_before(&thread_list_head, &thread->list_node);
	set_int_flag(iflag);
}

void create_user_thread(thread_entry* address, void* arg, const char* name) {
	struct thread* thread = aligned_kalloc(sizeof(struct thread), 16);
	thread->state = READY;
	thread->scheduled_on = -1;
	strncpy(thread->name, name, 32);

	thread->context.cs = 0x23;
	thread->context.ds = 0; // TODO: set something better here
	thread->context.es = 0;
	thread->context.fs = 0;
	thread->context.gs = 0;
	thread->context.ss = 0x2b;
	thread->context.eflags = EFLAGS_IF | 2;
	thread->context.eip = (uint)&user_thread_entry;
	fxstate_init(&thread->context.fxstate);

	// TODO: add guard pages to detect over/underflows
	uint current_stack_i = atomic_add(&next_user_stack, 1);
	uint* stack = (uint*)((char*)&user_stack + 4096 * current_stack_i); // TODO: allocate stack dynamically
	*(--stack) = (uint)arg;
	*(--stack) = (uint)address;
	*(--stack) = 0xDEADBEEF;
	thread->context.esp = (uint)stack;

	bool iflag = disable_interrupts();
	list_insert_before(&thread_list_head, &thread->list_node);
	set_int_flag(iflag);
}

void init_scheduler(void) {
	list_init(&thread_list_head);
}
