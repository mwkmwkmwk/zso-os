#ifndef PALLOC_H
#define PALLOC_H
#include <stdint.h>

uint32_t palloc(void);

void pfree(uint32_t);

uint32_t valloc(int cnt, uint32_t start, uint32_t end);

void vfree(uint32_t start, int cnt);

void map_page(uint32_t phys, uint32_t virt, int flags);
void unmap_page(uint32_t virt);

void *malloc(uint32_t size);
void free(void *);

#endif /*PALLOC_H*/
