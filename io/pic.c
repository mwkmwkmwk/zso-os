#include "pic.h"

#include "interrupts.h"
#include "io.h"
#include "stdlib/assert.h"
#include "utils/asm.h"

void init_pic() {
	// Remap PIC interrupts
	outb(PIC1_CMD,  0x11);
	outb(PIC1_DATA, PIC_INT_START);
	outb(PIC1_DATA, 0x04);
	outb(PIC1_DATA, 0x01);

	outb(PIC2_CMD,  0x11);
	outb(PIC2_DATA, PIC_INT_START + 8);
	outb(PIC2_DATA, 0x02);
	outb(PIC2_DATA, 0x01);

	// Silence all interrupts (no idt handlers yet)
	set_pic_interrupt_mask(0xFFFF);

	// Enable interrupts
	enable_interrupts();
}

ushort get_pic_int_mask(void) {
	ushort res = inb(PIC1_DATA);
	res |= (inb(PIC2_DATA) << 8); // Assumes that PIC1 and PIC2 are mapped continously
	return res;
}

static void assert_pic_int_num(int cpu_interrupt) {
	int pic_interrupt = CPU_TO_PIC(cpu_interrupt);
	assert(pic_interrupt >= 0 && pic_interrupt < 16
		&& pic_interrupt != INT_PIC_CASCADE);
}

// interrupt is CPU interrupt number
void enable_pic_interrupt(int interrupt) {
	assert_pic_int_num(interrupt);
	ushort new_mask = get_pic_int_mask() & ~(1 << CPU_TO_PIC(interrupt));
	set_pic_interrupt_mask(new_mask);
}

void disable_pic_interrupt(int interrupt) {
	assert_pic_int_num(interrupt);
	ushort new_mask = get_pic_int_mask() | (1 << CPU_TO_PIC(interrupt));
	set_pic_interrupt_mask(new_mask);
}

void set_pic_interrupt_mask(ushort mask) {
	outb(PIC1_DATA, mask & 0xFF);
	outb(PIC2_DATA, (mask >> 8) & 0xFF);
}

void finish_pic_interrupt(int interrupt) {
	assert_pic_int_num(interrupt);
	if (CPU_TO_PIC(interrupt) < 8)
		outb(PIC1_CMD, PIC_EOI);
	else
		outb(PIC2_CMD, PIC_EOI);
}
