#include <stdint.h>
#include <stddef.h>
#include "idt.h"
#include "io.h"

asm(
	".global _start\n"
	"_start:\n"
	"movabsq $stack+0x1000, %rsp\n"
	"call main\n"
	"cli\n"
	"hlt\n"
   );

extern uint16_t ekran[25][80];
int row = 2;
int col = 0;

extern char end;
void *mem = &end;

void *malloc(size_t sz) {
	void *res = mem;
	mem += sz;
	return res;
}

void printf(const char *p) {
	while (*p) {
		char c = *p++;
		if (c == '\n')
			row++, col = 0;
		else
			ekran[row][col++] = c | 0x0e00;
	}
}

void div0() {
	printf("Division by 0!\n");
	printf("System halted!\n");
	asm ("cli; hlt");
}

int getc() {
	while (!(inb(0x64) & 1));
	return inb(0x60);
}

int main() {
	init_idt();
	set_idt_entry(0, (uint64_t)&div0, true);
	printf("Hello, 64-bit world!\n");
#if 0
	volatile int a = 1;
	volatile int b = 0;
	a /= b;
#endif
	while (1) {
		uint8_t c = getc();
		char buf[] = "Byte 0x?? received\n";
		buf[8] = "0123456789abcdef"[c & 0xf];
		buf[7] = "0123456789abcdef"[c >> 4];
		printf(buf);
	}
	return 0;
}

uint8_t stack[0x1000];
