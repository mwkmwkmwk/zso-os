#include <stdint.h>
void putc(int c) {
	asm("int $0x30\n" : : "a"(1), "c"(c));
}

void print_num(int x, int y, uint32_t num) {
	asm("int $0x30\n" : : "a"(2), "c"(x), "d"(y), "b"(num));
}

void printf(char *format, ...) {
	while (*format) {
		char c = *format++;
		putc(c);
	}
}

void main(void) {
	printf("Hello, user!\n");
	uint32_t cnt = 0;
	while(1) {
		print_num(60, 10, cnt++);
	}
//	*(uint32_t*)0xb8000 = 0x1234;
}
