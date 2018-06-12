#include "page.h"

extern void kbss_end;
uint64_t phys_next = (uint64_t)(&kbss_end - 0xffff800000000000ull);

uint64_t phys_alloc() {
	if (phys_next & 0xfff) {
		phys_next |= 0xfff;
		phys_next++;
	}
	uint64_t res = phys_next;
	phys_next += 0x1000;
	return res;
}

void map_page(uint64_t virt, uint64_t phys, int flags) {
	uint64_t p_ptl4 = read_cr3() & ~0xfffull;
	uint64_t *v_ptl4 = P2V(p_ptl4);
	int idx1 = virt >> 12 & 0x1ff;
	int idx2 = virt >> 21 & 0x1ff;
	int idx3 = virt >> 30 & 0x1ff;
	int idx4 = virt >> 39 & 0x1ff;
	if (!v_ptl4[idx4]) {
		v_ptl4[idx4] = phys_alloc() | 7;
		uint64_t p_ptl3 = v_ptl4[idx4] & ~0xfffull;
		uint64_t *v_ptl3 = P2V(p_ptl3);
		memset(v_ptl3, 0, 0x1000);
	}
	uint64_t p_ptl3 = v_ptl4[idx4] & ~0xfffull;
	uint64_t *v_ptl3 = P2V(p_ptl3);
	if (!v_ptl3[idx3]) {
		v_ptl3[idx3] = phys_alloc() | 7;
		uint64_t p_ptl2 = v_ptl3[idx3] & ~0xfffull;
		uint64_t *v_ptl2 = P2V(p_ptl2);
		memset(v_ptl2, 0, 0x1000);
	}
	uint64_t p_ptl2 = v_ptl3[idx3] & ~0xfffull;
	uint64_t *v_ptl2 = P2V(p_ptl2);
	if (!v_ptl2[idx2]) {
		v_ptl2[idx2] = phys_alloc() | 7;
		uint64_t p_ptl1 = v_ptl2[idx2] & ~0xfffull;
		uint64_t *v_ptl1 = P2V(p_ptl1);
		memset(v_ptl1, 0, 0x1000);
	}
	uint64_t p_ptl1 = v_ptl2[idx2] & ~0xfffull;
	uint64_t *v_ptl1 = P2V(p_ptl1);
	v_ptl1[idx1] = phys | flags;
}

void init_pg() {
	uint64_t p_ptl4 = read_cr3() & ~0xfffull;
	uint64_t *v_ptl4 = P2V(p_ptl4);
	v_ptl4[0] = 0;
	write_cr3(read_cr3());
}
