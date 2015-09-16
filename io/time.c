#include "time.h"

#include "io/interrupts.h"
#include "io/pit.h"
#include "mem/kalloc.h"
#include "stdlib/list.h"
#include "stdlib/printf.h"
#include "threading/scheduler.h"
#include "utils/asm.h"

#define TICKS_PER_SEC 1000

static volatile ull current_time = 0; // In 1/2^32 seconds

struct callback_t {
	struct list list_node;
	timer_callback_t* callback;
};

struct list callbacks_head;

static void pit_int_handler(struct context** context_ptr) {
	atomic_add_8b(&current_time, (1llu << 32) / TICKS_PER_SEC);

	// static int cnt = 0;
	// cnt++;
	// if (cnt % TICKS_PER_SEC == 0)
	// 	printf("Timer tick! current_time: %u : %x\n", (uint)(current_time >> 32), (uint)current_time);

	for (struct list* it = callbacks_head.next; it != &callbacks_head; it = it->next) {
		((struct callback_t*)it)->callback(context_ptr);
	}

	finish_pic_interrupt(INT_PIT);
}

void init_timers(void) {
	list_init(&callbacks_head);
	register_int_handler(INT_PIT, pit_int_handler, false, 0);
	init_pit(TICKS_PER_SEC);
}

// Callable only from non-IRQ context
void sleep(ull ms) {
	ull start = get_current_time();
	ull delta = ms / 1000.0 * (1llu << 32);
	while (get_current_time() - start < delta) {
		yield();
	}
}

// Currently no correlation with wall-clock time, just a monotonic timer
ull get_current_time(void) {
	return atomic_read_8b(&current_time);
}

void add_tick_callback(timer_callback_t* f) {
	struct callback_t* callback = kalloc(sizeof(struct callback_t));
	callback->callback = f;
	list_insert_before(&callbacks_head, &callback->list_node);
}
