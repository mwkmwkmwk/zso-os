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
extern void* load_context;
