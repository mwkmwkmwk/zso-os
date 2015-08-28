#include "interrupts.h"

#include "mem/gdt.h"

int_handler_t int_handlers[256];

// Defined in assembly
extern uint32_t int_entries_ptrs[256];

// Beware: It doesn't enable any interrupts, just installs a handler
void register_int_handler(int int_num, int_handler_t handler, bool block, 
                          int priv_level) {
	int_handlers[int_num] = handler;
	set_idt(int_num, int_entries_ptrs[int_num], block, priv_level);
}
