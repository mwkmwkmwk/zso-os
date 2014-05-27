#include "thread.h"
#include "io.h"

struct thread idle_thread = {
	.next = &idle_thread,
	.prev = &idle_thread,
	.blocked = 1,
};

struct thread *current_thread = &idle_thread;

void start_thread(struct thread *thread, uint8_t *stack, void (*func) (void *), void *param) {
	cli();
	thread->prev = current_thread;
	thread->next = current_thread->next;
	current_thread->next->prev = thread;
	current_thread->next = thread;
	uint32_t *pparam = (uint32_t *)(stack - 4);
	*pparam = (uint32_t)param;
	stack -= 8;
	thread->blocked = 0;
	thread->eip = (uint32_t)func;
	thread->esp = (uint32_t)stack;
	thread->eflags = 0x202;
	thread->es = 0x10;
	thread->cs = 0x8;
	thread->ss = 0x10;
	thread->ds = 0x10;
	thread->fs = 0;
	thread->gs = 0;
	sti();
}
