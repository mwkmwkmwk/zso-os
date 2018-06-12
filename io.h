static inline void outb(uint16_t port, uint8_t val) {
	__asm__ volatile(
		"outb %0, %1\n"
		:
		: "a"(val), "d"(port)
	);
}

static inline uint8_t inb(uint16_t port) {
	uint8_t res;
	__asm__ volatile(
		"inb %1, %0\n"
		: "=a"(res)
		: "d"(port)
	);
	return res;
}
