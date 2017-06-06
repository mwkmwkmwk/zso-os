static inline void wrmsr(uint32_t msr, uint64_t val) {
	uint32_t lo = val;
	uint32_t hi = val >> 32;
	asm volatile ("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

static inline uint64_t rdmsr(uint32_t msr) {
	uint32_t lo, hi;
	asm volatile ("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr));
	return (uint64_t)hi << 32 | lo;
}
