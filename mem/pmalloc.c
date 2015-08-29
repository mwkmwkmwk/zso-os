#include <stdbool.h>

#include "pmalloc.h"
#include "panic.h"

#define PAGE_SIZE 0x1000

uint32_t free_root = FREE_END;

extern char user_start;
extern char user_end;
extern char kernel_start;
extern char kernel_end;

void boot_pfree(uint32_t page) {
	*(uint32_t *)page = free_root;
	free_root = page;
}

uint32_t boot_palloc() {
	if (free_root == FREE_END) {
		panic("OOM");
	}
	uint32_t res = free_root;
	free_root = *(uint32_t *)res;
	return res;
}

static bool collide(uint32_t page, void *start, void *end) {
	uint32_t pstart = page;
	uint32_t pend = page + PAGE_SIZE;
	uint32_t ostart = (uint32_t)start;
	uint32_t oend = (uint32_t)end;
	if (pstart >= oend || ostart >= pend)
		return false;
	return true;
}

void init_pmalloc(struct mb_header *mbhdr) {
	struct mb_mmap *mmap;
	for (
		mmap = mbhdr->mmap_addr;
		(char *)mmap < (char *)mbhdr->mmap_addr + mbhdr->mmap_length;
		mmap = (void *)((char *)mmap + mmap->size + 4)
	) {
		if (mmap->type != 1)
			continue;
		if (mmap->base_hi)
			continue;
		uint32_t start = mmap->base_lo;
		uint32_t end = mmap->base_lo + mmap->length_lo;
		if (start & (PAGE_SIZE - 1)) {
			start &= ~(PAGE_SIZE - 1);
			start += PAGE_SIZE;
		}
		end &= ~(PAGE_SIZE - 1);
		uint32_t page;
		for (page = start; page < end; page += PAGE_SIZE) {
			if (collide(page, mbhdr, mbhdr+1))
				continue;
			if (collide(page, mbhdr->mmap_addr, mbhdr->mmap_addr + mbhdr->mmap_length))
				continue;
			if (collide(page, &user_start, &user_end))
				continue;
			if (collide(page, &kernel_start, &kernel_end))
				continue;
			boot_pfree(page);
		}
	}
}
