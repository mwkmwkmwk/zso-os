#pragma once

#include <stdint.h>

void init_kalloc(void);
void* aligned_kalloc(uint32_t size, uint32_t alignment);
void* kalloc(uint32_t size);
void kfree(void* ptr);
