#include <stdint.h>
#include <stdarg.h>
void putc(int c) {
	asm("int $0x30\n" : : "a"(1), "c"(c));
}

int getk(void) {
	int res;
	asm("int $0x30\n" : "=a"(res) : "a"(3));
	return res;
}

void print_num(int x, int y, uint32_t num) {
	asm("int $0x30\n" : : "a"(2), "c"(x), "d"(y), "b"(num));
}

void printf(char *format, ...) {
	int i;
	uint32_t n;
	va_list va;
	va_start(va, format);
	while (*format) {
		char c = *format++;
		if (c == '%') {
			c = *format++;
			switch (c) {
				case 'x':
					n = va_arg(va, uint32_t);
					putc('0');
					putc('x');
					for (i = 7; i >= 0; i--)
						putc("0123456789abcdef"[n >> (i * 4) & 0xf]);
					break;
			}
		} else {
			putc(c);
		}
	}
	va_end(va);
}

void main(void) {
	printf("Hello, user!\n");
	uint32_t cnt = 0;
	while (1) {
		int c = getk();
		printf("Key: %x\n", c);
	}
//	*(uint32_t*)0xb8000 = 0x1234;
}
