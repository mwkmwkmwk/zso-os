#pragma once

#include <stdint.h>

struct mb_header {
	uint32_t flags;
	uint32_t mem_lower;
	uint32_t mem_upper;
	uint32_t boot_device;
	char *cmdline;
	uint32_t mods_count;
	void *mods_addr;
	uint32_t syms[4];
	uint32_t mmap_length;
	void *mmap_addr;
};

#define MB_FLAGS_MMAP	0x00000040

struct mb_mmap {
	uint32_t size;
	uint32_t base_lo;
	uint32_t base_hi;
	uint32_t length_lo;
	uint32_t length_hi;
	uint32_t type;
};
