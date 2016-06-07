#include <stdint.h>
#include <stdbool.h>
struct process {
	uint32_t esp;
	bool key_blocked;
	struct process *next;
};

extern struct process *proc_root;
extern struct process *proc_cur;

void switch_process(struct process *proc);
struct process *start_process(void (*fun) (void *), void *param);
