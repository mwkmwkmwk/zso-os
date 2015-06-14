#include "mb.h"

void init_pmalloc(struct mb_header *mbhdr);
uint32_t boot_palloc();

#define FREE_END 0xffffffff
