static inline void wrmsr(uint32_t reg, uint64_t val) {
	uint32_t lo = val;
	uint32_t hi = val >> 32;
	__asm__ volatile (
		"wrmsr"
		:
		: "a"(lo), "d"(hi), "c"(reg)
	);
}
