#include "palloc_np.h"
#include <stdbool.h>
#include <stdint.h>
#include "multiboot.h"
#include "print.h"
#include "io.h"

extern multiboot_info_t *multiboot_info;

uint32_t free_list = 0;

extern uint8_t kernel_start[];
extern uint8_t kernel_end[];

void pfree_np(uint32_t addr) {
	*(uint32_t *)addr = free_list;
	free_list = addr;
}

uint32_t palloc_np() {
	if (!free_list) {
		printf(default_window, "PANIC\n");
		asm("cli\nhlt\n");
		__builtin_unreachable();
	} else {
		uint32_t res = free_list;
		free_list = *(uint32_t *)res;
		return res;
	}
}

void map_page_np(uint32_t *pd, uint32_t phys, uint32_t virt) {
	int pdi = virt >> 22 & 0x3ff;
	int pti = virt >> 12 & 0x3ff;
	if (!pd[pdi]) {
		uint32_t *pt = (uint32_t *)palloc_np();
		pd[pdi] = (uint32_t)pt | 1;
		int i;
		for (i = 0; i < 0x400; i++)
			pt[i] = 0;
	}
	uint32_t *pt = (uint32_t *)(pd[pdi] & ~0xfff);
	pt[pti] = phys | 1;
}

static inline bool page_ok_range(uint32_t page, void *start, void *end) {
	uint32_t page_end = page + 0x1000;
	return (page_end <= (uint32_t)start
		|| page >= (uint32_t)end);
}

bool page_ok(uint32_t page) {
	if (!page_ok_range(page, &kernel_start, &kernel_end))
		return false;
	if (!page_ok_range(page, multiboot_info, multiboot_info+1))
		return false;
	if (!page_ok_range(page, (void*)multiboot_info->mmap_addr, (void*)(multiboot_info->mmap_addr + multiboot_info->mmap_length)))
		return false;
	return true;
}

void init_palloc() {
	multiboot_memory_map_t *mmap;
	for(mmap = (void*)multiboot_info->mmap_addr; (uint32_t)mmap < multiboot_info->mmap_addr + multiboot_info->mmap_length; mmap = (void*)((uint32_t)mmap + mmap->size + sizeof mmap->size)) {
		uint64_t start = mmap->addr;
		uint64_t end = mmap->addr + mmap->len;
		if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE)
			continue;
		if (start > (1ull << 32))
			continue;
		if (end > (1ull << 32))
			end = 1ull << 32;
		if (start & 0xfff) {
			start &= ~0xfff;
			start += 0x1000;
		}
		end &= ~0xfff;
		uint32_t page;
		for (page = start; page < end; page += 0x1000) {
			if (page_ok(page))
				pfree_np(page);
		}
	}
	uint32_t *pd = (uint32_t *)palloc_np();
	int i;
	for (i = 0; i < 0x400; i++)
		pd[i] = 0;
	pd[0x3ff] = (uint32_t)pd | 1;
	uint32_t page;
	uint32_t start = (uint32_t)&kernel_start;
	uint32_t end = (uint32_t)&kernel_end;
	start &= ~0xfff;
	if (end & 0xfff) {
		end &= ~0xfff;
		end += 0x1000;
	}
	for (page = start; page < end; page += 0x1000) {
		map_page_np(pd, page, page);
	}
	for (page = 0xb8000; page < 0xc0000; page += 0x1000) {
		map_page_np(pd, page, page);
	}
	map_page_np(pd, free_list, 0);
	set_cr3((uint32_t)pd);
	set_cr0(get_cr0() | 1 << 31);
	printf(default_window, "PD %x\n", pd);
	printf(default_window, "PD PD %x\n", *(uint32_t *)0xfffffffc);
}
