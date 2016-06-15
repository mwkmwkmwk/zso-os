#include <stdint.h>

static volatile void *const ioapic = (volatile void *)0x7fc00000;
static volatile void *const lapic = (volatile void *)0x7fe00000;

uint32_t palloc(void);
void printf(char *format, ...);
_Noreturn void panic(char *reason);
void init_paging(void);
void map_user(uint32_t virt, uint32_t phys, _Bool write);
uint32_t load_user(const uint8_t *elf);

void write_ioapic(int idx, uint32_t data);
uint32_t read_ioapic(int idx);
void write_lapic(int idx, uint32_t data);
uint32_t read_lapic(int idx);

