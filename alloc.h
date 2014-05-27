#ifndef ALLOC_H
#define ALLOC_H

#define NULL ((void *)0)

typedef unsigned long size_t;

void *alloc(size_t size);
void free(void *ptr);
int init_memory(void *addr, size_t size);

#endif
