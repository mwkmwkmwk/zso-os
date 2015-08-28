#include "keyboard.h"

#include <stdint.h>

#include "io/interrupts.h"
#include "io/io.h"
#include "mem/gdt.h"
#include "stdlib/printf.h"

extern char asm_irq1[];

void irq1(void) {
	uint8_t b = inb(0x60);
	printf("KEY %x\n", b);
	outb(0x20, 0x20);
}

void init_keyboard(void) {
	//set_idt(INT_KEYBOARD, (uint32_t)asm_irq1, true, 0);
	register_int_handler(INT_KEYBOARD, irq1, true, 0);
	outb(0x21, 0b11111101); // 0x21: Master PIC data port, sets interrupt mask (1 - ignored)
}
