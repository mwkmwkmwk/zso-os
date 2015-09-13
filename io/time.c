#include "time.h"

#include "io/interrupts.h"
#include "io/pit.h"
#include "stdlib/printf.h"
#include "threading/context.h"
#include "utils/asm.h"

#define TICKS_PER_SEC 1000

static volatile ull current_time = 0; // In 1/2^32 seconds

static void pit_int_handler(struct context** context_ptr) {
	atomic_add_8b(&current_time, (1llu << 32) / TICKS_PER_SEC);

	// static int cnt = 0;
	// cnt++;
	// if (cnt % TICKS_PER_SEC == 0)
	// 	printf("Timer tick! current_time: %u : %x\n", (uint)(current_time >> 32), (uint)current_time);
	finish_pic_interrupt();
}

void init_timers(void) {
	register_int_handler(INT_PIT, pit_int_handler, false, 0);
	init_pit(TICKS_PER_SEC);
}

void active_sleep(ull ms) {
	ull start = current_time;
	ull delta = ms / 1000.0 * (1llu << 32);
	while (current_time - start < delta) {}
}
