#include <stdint.h>
#include "pic.h"
#include "io.h"

void init_pic() {
	outb(0x20, 0x11);
	outb(0x21, 0x20); /* start vector for IRQ0-IRQ7 */
	outb(0x21, 4);
	outb(0x21, 1);
	outb(0x21, 0xff);
	outb(0xa0, 0x11);
	outb(0xa1, 0x28); /* start vector for IRQ8-IRQ15 */
	outb(0xa1, 2);
	outb(0xa1, 1);
	outb(0xa1, 0xff);
}

void irq_enable(int idx) {
	uint8_t reg;
	if (idx & 8)
		reg = 0xa1;
	else
		reg = 0x21;
	outb(reg, inb(reg) & ~(1 << (idx & 7)));
}

void irq_disable(int idx) {
	uint8_t reg;
	if (idx & 8)
		reg = 0xa1;
	else
		reg = 0x21;
	outb(reg, inb(reg) | (1 << (idx & 7)));
}
