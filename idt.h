#include <stdbool.h>
#include <stdint.h>
void init_gdt();
void init_idt();
void set_idt_entry(int idx, uint64_t addr, bool user);
void set_tss_rsp(uint64_t rsp);
