#include <stdint.h>
void putc(int c) {
	asm("int $0x30\n" : : "a"(1), "c"(c));
}

void printf(char *format, ...) {
	while (*format) {
		char c = *format++;
		putc(c);
	}
}

void main(void) {
	printf("Hello, user!\n");
//	*(uint32_t*)0xb8000 = 0x1234;
}
