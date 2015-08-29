#pragma once

#include <stdint.h>

void init_kalloc(void);
void* kalloc(uint32_t size);
void kfree(void* ptr);
