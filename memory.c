#include "kernel.h"
#include <stdint.h>

#define PAGE_SIZE 0x1000
#define MEM_PAGES 0x1000

uint8_t memory[PAGE_SIZE * MEM_PAGES] __attribute__((aligned(PAGE_SIZE)));
uint8_t memory_used[MEM_PAGES / 8];
int first_free = 0;

uint32_t palloc(void) {
	unsigned i;
	for (i = first_free; i < MEM_PAGES; i++) {
		if (!(memory_used[i/8] >> (i & 7) & 1)) {
			memory_used[i/8] |= 1 << (i & 7);
			first_free = i + 1;
			return (uint32_t)memory + i * PAGE_SIZE;
		}
	}
	panic("out of memory\n");
}

void init_paging(void) {
	uint32_t pd = palloc();
	uint32_t *pdp = (uint32_t *)pd;
	int i;
	for (i = 0; i < PAGE_SIZE/4; i++)
		pdp[i] = 0;
	for (i = 0 ; i < 0x100; i++)
		pdp[i] = i << 22 | 0x183;
	pdp[0x3ff] = 0x183;
	asm volatile ("movl %0, %%cr3" : : "r"(pd));
	uint32_t cr4;
	asm volatile ("movl %%cr4, %0" : "=r"(cr4) :);
	cr4 |= 0x00000010;
	asm volatile ("movl %0, %%cr4" : : "r"(cr4));
	uint32_t cr0;
	asm volatile ("movl %%cr0, %0" : "=r"(cr0) :);
	cr0 |= 0x80000000;
	asm volatile ("movl %0, %%cr0" : : "r"(cr0));
}

void map_user(uint32_t virt, uint32_t phys, _Bool write) {
	int pdi = virt >> 22 & 0x3ff;
	int pti = virt >> 12 & 0x3ff;
	uint32_t *pdp;
	uint32_t *ptp;
	uint32_t pt;
	printf("map %x %x\n", virt, phys);
	asm volatile ("movl %%cr3, %0" :"=r"(pdp) :);
	if (!pdp[pdi]) {
		pt = palloc();
		ptp = (void*)pt;
		int i;
		for (i = 0; i < 0x400; i++)
			ptp[i] = 0;
		pdp[pdi] = pt | 7;
	} else {
		pt = pdp[pdi] & ~0xfff;
		ptp = (void*)pt;
	}
	if (ptp[pti]) {
		panic("page already mapped");
	}
	ptp[pti] = phys | 0x5 | write << 1;
}
