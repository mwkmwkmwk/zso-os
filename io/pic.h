#pragma once

// Ports
#define PIC1_CMD 0x20
#define PIC1_DATA 0x21
#define PIC2_CMD 0xA0
#define PIC2_DATA 0xA1

// Remap all PIC interrupts starting from here
#define PIC_INT_START 0xF0

// Converts interrupt numbers between processor and PIC
// (taking PIC remapping into account)
#define PIC_TO_CPU(int_num) ((int_num) + PIC_INT_START)
#define CPU_TO_PIC(int_num) ((int_num) - PIC_INT_START)

void init_pic();
