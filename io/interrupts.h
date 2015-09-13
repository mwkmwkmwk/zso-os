#pragma once

#include <stdbool.h>

#include "io/pic.h"
#include "threading/context.h"

// CPU interrupts (descr. from Intel manuals)
#define INT_DIV_ERROR	0 // Divide Error DIV and IDIV instructions.
#define INT_DEBUG		1 // Debug Any code or data reference.
#define INT_NMI			2 // Interrupt Non-maskable external interrupt.
#define INT_3			3 // Breakpoint INT 3 instruction.
#define INT_OVERFLOW	4 // Overflow INTO instruction.
#define INT_BOUND		5 // BOUND Range Exceeded BOUND instruction.
#define INT_UNDEF		6 // Invalid Opcode (UnDefined Opcode) UD2 instruction or reserved opcode.
#define INT_NM			7 // Device Not Available (No Math Coprocessor) Floating-point or WAIT/FWAIT instruction.
#define INT_DOUBLE_FAULT	8 // Double Fault Any instruction that can generate an exception, an NMI, or an INTR.
#define INT_MF			9 // CoProcessor Segment Overrun (reserved) Floating-point instruction.
#define INT_TSS			10 // Invalid TSS Task switch or TSS access.
#define INT_NP			11 // Segment Not Present Loading segment registers or accessing system segments.
#define INT_STACK_FAULT	12 // Stack Segment Fault Stack operations and SS register loads.
#define INT_GEN_PROT	13 // General Protection Any memory reference and other protection checks.
#define INT_PAGE_FAULT	14 // Page Fault Any memory reference.
// 15:  Reserved
#define INT_FPU_ERROR	16 // Floating-Point Error (Math Fault) Floating-point or WAIT/FWAIT instruction.
#define INT_ALIGN_CHECK	17 // Alignment Check Any data reference in memory.
#define INT_MACHINE_CHECK	18 // Machine Check Error codes (if any) and source are model dependent.
#define INT_SIMD_EXC	19 // SIMD Floating-Point Exception SIMD Floating-Point Instruction5

// 20-31: reversed

// User interrupts
#define INT_SYSCALL		0x20
#define INT_YIELD		0x21 // It doesn't go through standard syscall interface, so we'd better use another syscall number

// PIC interrupts

#define INT_PIT         PIC_TO_CPU(0)
#define INT_KEYBOARD    PIC_TO_CPU(1)
#define INT_PIC_CASCADE PIC_TO_CPU(2) // PIC1<->PIC2 internal (cascade interrupt), reserved
#define INT_COM2		PIC_TO_CPU(3)
#define INT_COM1		PIC_TO_CPU(4)
#define INT_LPT2		PIC_TO_CPU(5)
#define INT_FLOPPY		PIC_TO_CPU(6)
#define INT_LPT1		PIC_TO_CPU(7)
#define INT_CMOS		PIC_TO_CPU(8)
#define INT_PERIPH1		PIC_TO_CPU(9)
#define INT_PERIPH2		PIC_TO_CPU(10)
#define INT_PERIPH3		PIC_TO_CPU(11)
#define INT_PS2_MOUSE	PIC_TO_CPU(12)
#define INT_FPU			PIC_TO_CPU(13)
#define INT_ATA1		PIC_TO_CPU(14)
#define INT_ATA2		PIC_TO_CPU(15)

typedef void (*int_handler_t)(struct context**);

// Registered interrupt handlers
extern int_handler_t int_handlers[256];

void register_int_handler(int int_num, int_handler_t handler, bool block, 
                          int priv_level);
