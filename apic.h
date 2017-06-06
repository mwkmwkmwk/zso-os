#include <stdint.h>

static void write_lapic(uint32_t reg, uint32_t val) {
	*(volatile uint32_t*)(0xffff8000fee00000 + reg) = val;
}

static uint32_t read_lapic(uint32_t reg) {
	return *(volatile uint32_t*)(0xffff8000fee00000 + reg);
}

static void write_ioapic(uint32_t reg, uint32_t val) {
	*(volatile uint32_t*)0xffff8000fec00000 = reg;
	*(volatile uint32_t*)0xffff8000fec00010 = val;
}

static uint32_t read_ioapic(uint32_t reg) {
	*(volatile uint32_t*)0xffff8000fec00000 = reg;
	return *(volatile uint32_t*)0xffff8000fec00010;
}
