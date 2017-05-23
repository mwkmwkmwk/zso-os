#include <stdbool.h>
#include <stdint.h>
void init_idt();
void set_idt_entry(int idx, uint64_t addr, bool user);
