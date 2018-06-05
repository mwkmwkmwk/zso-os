#include <stdint.h>
char hello[] = "Hello, C world!";

void main() {
	for (int i = 0; i < sizeof hello; i++) {
		*(uint16_t *)(0xffff8000000b8000 + i * 2) = hello[i] | 0x0f00;
	}
}
