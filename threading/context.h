#pragma once

#include "common.h"

// Unfortunately it's redundant with context.inc's
// We still lack some not-widely used registers, e.g. dr*
// Must be aligned to 16B
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

	char __attribute__((aligned(16))) fxstate[512];
};

// Not callable from C
extern void* save_context;
// Use with care. Really.
// Modifies 12 bytes from the new threads' stack (esp-1 .. esp-12) if no privilege change occurs
extern noreturn void load_context(struct context* context);
