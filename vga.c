#include "vga.h"
#include "io.h"

uint8_t get_register(uint8_t idx) {
	outb(0x3d4, idx);
	return inb(0x3d5);
}

void set_register(uint8_t idx, uint8_t val) {
	outb(0x3d4, idx);
	outb(0x3d5, val);
}

void set_cursor(uint16_t addr) {
	addr /= 2;
	set_register(0xe, addr >> 8);
	set_register(0xf, addr);
}

uint16_t get_cursor() {
	uint16_t res = 0;
	res |= get_register(0xe) << 8;
	res |= get_register(0xf);
	return res * 2;
}
