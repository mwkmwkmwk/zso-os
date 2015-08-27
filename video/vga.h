#pragma once

#include <stdint.h>

struct vga_char {
	char ch;
	uint8_t attr;
};

extern struct vga_char framebuffer[25][80];

void set_cursor(uint16_t addr);
uint16_t get_cursor();
