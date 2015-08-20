#pragma once

#include <stdint.h>

void map_page(void *virt, uint32_t phys, uint32_t flags);

#define MAP_PRESENT 1

#define MAP_KERNEL MAP_PRESENT

#define PAGE_SIZE 0x1000

void init_paging();
