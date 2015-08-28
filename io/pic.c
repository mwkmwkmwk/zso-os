#include "io.h"
#include "pic.h"

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
	outb(PIC1_DATA, 0xff);
	outb(PIC2_DATA, 0xff);

	// Enable interrupts
	asm volatile ("sti":::);
}
