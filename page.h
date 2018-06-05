#include <stdint.h>

static inline void *P2V(uint64_t p) {
	return (void *)(0xffff800000000000ull + p);
}

static inline uint64_t read_cr3() {
	uint64_t res;
	__asm__ volatile(
		"movq %%cr3, %0"
		: "=r"(res)
		:
	);
	return res;
}

static inline void write_cr3(uint64_t val) {
	__asm__ volatile(
		"movq %0, %%cr3"
		:
		: "r"(val)
	);
}

uint64_t phys_alloc();
void init_pg();
void map_page(uint64_t virt, uint64_t phys, int flags);
