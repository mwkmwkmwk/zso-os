#include <stdint.h>

int user_main() {
	asm volatile ("int $0x20 \n":::);
	asm volatile ("int $0x20 \n":::);
	while (1);
}

uint8_t user_stack[4096] = { 0 };
