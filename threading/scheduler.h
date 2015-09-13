#pragma once

#include "stdlib/list.h"
#include "threading/context.h"

#define SCHED_QUANT_MS 10
#define SCHED_QUANT (SCHED_QUANT_MS / 1000.0 * (1llu << 32)) // Task switch every 10ms
#define THREAD_STACK_SIZE 0x4000

enum THREAD_STATE {
	RUNNING,
	READY,
	EXITING,
};

struct thread {
	struct list list_node;
	enum THREAD_STATE state;
	ull scheduled_on;
	struct context context; // valid only if state != RUNNING
	const char* name;
};

extern struct list thread_list_head;

typedef int thread_entry(void*);

void __attribute__((noreturn)) start_scheduling(void);
void create_kernel_thread(thread_entry* address, void* arg, const char* name);
void create_user_thread(thread_entry* address, void* arg, const char* name);
void __attribute__((noreturn)) kill_current_thread(int exit_code);
void init_scheduler(void);
