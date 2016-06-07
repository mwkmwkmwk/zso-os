void init_gdt(void);

extern struct tss {
	uint32_t _0;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t _1[0x68/4 - 3];
} tss;
