#include <stdint.h>
uint32_t palloc(void);
void printf(char *format, ...);
_Noreturn void panic(char *reason);
void init_paging(void);
void map_user(uint32_t virt, uint32_t phys, _Bool write);
uint32_t load_user(const uint8_t *elf);

