#include <stdint.h>

static void main_debug() {
	// print module testing
	if (0) {
		print("12\t2\t232\taax\n211\t231\t2135\t21111\n");

		printf("percent: %% incorrect: %! strings: %s %s\n", "string1", "string2");
		printf("257 %%d: %d %%x: %x\n", 257, 257);
		printf("-257 %%d: %d %%x: %x\n", -257, -257);
		printf("0 %%d: %d %%x: %x\n", 0, 0);

		printf("INT32_MAX %d INT32_MIN %d\n", INT32_MAX, INT32_MIN);
		printf("UINT32_MAX %u %x %p\n", UINT32_MAX, UINT32_MAX, (void *)UINT32_MAX);
		printf("UINT32_MIN=0 %u %x %p\n", 0, 0, (void *)0);
	}
}
