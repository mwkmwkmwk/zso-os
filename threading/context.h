#pragma once

#include "common.h"

// Unfortunately it's redundant with context.inc's
struct context {
	uint eax;
	uint ebx;
	uint ecx;
	uint edx;
	uint esi;
	uint edi;
	uint ebp;
	uint ds;
	uint es;
	uint fs;
	uint gs;
	uint eip;
	uint cs;
	uint eflags;
	uint esp;
	uint ss;
};

// Not callable from C
extern void* save_context;
// Use with care. Really.
// Modifies 12 bytes from the new threads' stack (esp-1 .. esp-12) if no privilege change occurs
extern __attribute__((noreturn)) void load_context(struct context* context);
