#ifndef VGA_H
#define VGA_H

#include <stdint.h>

void vga_set_cursor_addr(uint16_t addr);
uint16_t vga_get_cursor_addr();

#endif
