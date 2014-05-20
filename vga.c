#include "io.h"
#include "vga.h"

static inline void crtc_wr(uint8_t idx, uint8_t val) {
	outb(0x3d4, idx);
	outb(0x3d5, val);
}

static inline uint8_t crtc_rd(uint8_t idx) {
	outb(0x3d4, idx);
	return inb(0x3d5);
}

void vga_set_cursor_addr(uint16_t addr) {
	crtc_wr(0xe, addr >> 8);
	crtc_wr(0xf, addr);
}

uint16_t vga_get_cursor_addr() {
	uint16_t res = crtc_rd(0xf);
	res |= crtc_rd(0xe) << 8;
	return res;
}
