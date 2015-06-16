#include <stdbool.h>
#include <stdint.h>

void init_gdt();
void set_idt(int idx, uint32_t addr, bool block, int pl);
