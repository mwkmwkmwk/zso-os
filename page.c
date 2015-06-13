#include "page.h"
#include "pmalloc.h"

extern char kernel_start;
extern char kernel_end;

static uint32_t *boot_pd;

void boot_map_page(void *virt, uint32_t phys, uint32_t flags) {
	uint32_t ivirt = (uint32_t)virt;
	int pdi = ivirt >> 22 & 0x3ff;
	int pti = ivirt >> 12 & 0x3ff;
	uint32_t *ppt;
	if (!boot_pd[pdi]) {
		uint32_t pt = boot_palloc();
		ppt = (void *)pt;
		for (int i = 0; i < 1024; i++)
			ppt[i] = 0;
		boot_pd[pdi] = pt | 1;
	}
	ppt = (void *)(boot_pd[pdi] & ~0xfff);
	ppt[pti] = phys | flags;
}

void map_range_id(void *start, void *end) {
	uint32_t istart = (uint32_t)start;
	uint32_t iend = (uint32_t)end;
	for (uint32_t addr = istart; addr < iend; addr += PAGE_SIZE) {
		boot_map_page((void *)addr, addr, MAP_KERNEL);
	}
}

void init_paging() {
	uint32_t pd = boot_palloc();
	asm volatile (
		"movl %0, %%cr3\n":
		:
		"r"(pd):
	);
	boot_pd = (void *)pd;
	for (int i = 0; i < 1024; i++)
		boot_pd[i] = 0;
	boot_pd[1023] = pd | 1;
	map_range_id(&kernel_start, &kernel_end);
	map_range_id((void *)0xb8000, (void *)0xc0000);
	uint32_t cr0;
	asm volatile (
		"movl %%cr0, %0\n":
		"=r"(cr0):
		:
	);
	cr0 |= 0x80000000;
	asm volatile (
		"movl %0, %%cr0\n":
		:
		"r"(cr0):
	);
}
