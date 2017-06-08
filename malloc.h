#pragma once

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 0x1000

#define MALLOC_ALIGN_SIZE 0x10
#define MALLOC_ALIGN_BITS (MALLOC_ALIGN_SIZE - 1)
#define MALLOC_HEADER_SIZE 0x10
#define MALLOC_NEXT_PTR_OFF 0x10
#define MALLOC_SIZE_OFF 0x8

#define ADD_OFF(ptr, off, type) ((type *)((uint64_t)(ptr) + (off)))
#define MALLOC_GET_NEXT_PTR_ADDR(ptr) (*ADD_OFF(ptr, -MALLOC_NEXT_PTR_OFF, void *))

#define MALLOC_GET_SIZE(ptr) (*ADD_OFF(ptr, -MALLOC_SIZE_OFF, uint64_t))

void *malloc(size_t);
void free(void *);
void *alloc_pages(size_t);
