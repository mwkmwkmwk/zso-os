#include <stdint.h>
#include "gdt.h"
#include "page.h"
char hello[] = "Hello, C world!";

__asm__(
	".text\n"
	"go_user:\n"
	"pushq $0x1b\n"
	"pushq $0\n"
	"pushq $0x2\n"
	"pushq $0x23\n"
	"pushq %rdi\n"
	"iretq\n"
);

_Noreturn void go_user(void *);

void main() {
	init_gdt();
	init_pg();
	map_page(0x31337000, phys_alloc(), 7);
	uint8_t *upage = (void *)0x31337000ull;
	upage[0] = 0xeb;
	upage[1] = 0xfe;
	go_user(upage);
	for (int i = 0; i < sizeof hello; i++) {
		*(uint16_t *)(0x31337000 + i * 2) = hello[i] | 0x0f00;
	}
}
