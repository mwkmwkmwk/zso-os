#include "proc.h"
#include "kernel.h"

struct process proc_init;
struct process *proc_root = &proc_init;
struct process *proc_cur = &proc_init;

#define PAGE_SIZE 0x1000

struct process *start_process(void (*fun) (void *), void *param) {
	uint32_t stack = palloc();
	uint32_t pd = palloc();
	uint32_t *pdp = (uint32_t *)pd;
	int i;
	for (i = 0; i < PAGE_SIZE/4; i++)
		pdp[i] = 0;
	for (i = 0 ; i < 0x100; i++)
		pdp[i] = i << 22 | 0x183;
	uint32_t *top = (void *)(stack + 0x1000);
	*--top = (uint32_t)param;
	*--top = 0;
	*--top = (uint32_t)fun;
	*--top = 0;
	*--top = 0;
	*--top = 0;
	*--top = stack + 0x1000;
	*--top = pd;
	struct process *res = (void *)stack;
	res->esp = (uint32_t)top;
	res->next = proc_root;
	res->key_blocked = false;
	proc_root = res;
	return res;
}
