#include <stdint.h>
#include <sys/queue.h>
#include "alloc.h"

#define __round_mask(x, y) ((__typeof__(x))((y)-1))
#define round_up(x, y) ((((x)-1) | __round_mask(x, y))+1)
#define round_down(x, y) ((x) & ~__round_mask(x, y))
#define ALIGN 4

struct chunk {
	TAILQ_ENTRY(chunk) chunks;
	unsigned char used;
	union {
		LIST_ENTRY(chunk) free_chunks;
		char data[0];
	};
};

TAILQ_HEAD(chunk_tailq, chunk) chunks;
LIST_HEAD(chunk_list, chunk) free_chunks;

static inline size_t chunk_size(struct chunk *chunk)
{
	char *end = TAILQ_NEXT(chunk, chunks);
	return (end - (char *)(&chunk->data));
}

void *alloc(size_t size)
{
	struct chunk *chunk;
	size = round_up(size, ALIGN);
	LIST_FOREACH(chunk, &free_chunks, free_chunks) {
		size_t csize = chunk_size(chunk);
		if (csize >= size) {
			LIST_REMOVE(chunk, free_chunks);
			if (csize > size + sizeof(struct chunk)) {
				struct chunk *new;
				new = (struct chunk *)((char *)(&chunk->data)
				                       + size);
				new->used = 0;
				TAILQ_INSERT_AFTER(&chunks, chunk, new, chunks);
				LIST_INSERT_HEAD(&free_chunks, new,
				                 free_chunks);
			}
			chunk->used = 1;
			return &chunk->data;
		}
	}
	return NULL;
}

void free(void *ptr)
{
	struct chunk *chunk, *next, *prev;
	chunk = (struct chunk *)((char *)ptr
	                         - (char *)((struct chunk *)0)->data);
	next = TAILQ_NEXT(chunk, chunks);
	prev = TAILQ_PREV(chunk, chunk_tailq, chunks);
	if (next->used = 0) {
		LIST_REMOVE(next, free_chunks);
		TAILQ_REMOVE(&chunks, next, chunks);
	}
	if (prev->used = 0) {
		TAILQ_REMOVE(&chunks, chunk, chunks);
	} else {
		chunk->used = 0;
		LIST_INSERT_HEAD(&free_chunks, chunk, free_chunks);
	}
}

int init_memory(void *addr, size_t size)
{
	char *start = (char *)round_up((uintptr_t)addr, ALIGN);
	char *end = (char *)addr + round_down(size, ALIGN);
	struct chunk *front_guard, *chunk, *end_guard;
	if ((uintptr_t)(end - start) <= 3 * sizeof(struct chunk))
		return -1;
	front_guard = (struct chunk *)start;
	chunk = (struct chunk *)start + 1;
	end_guard = (struct chunk *)end - 1;
	front_guard->used = 1;
	front_guard->data[0] = 0x44;
	chunk->used = 0;
	end_guard->used = 1;
	TAILQ_INSERT_TAIL(&chunks, front_guard, chunks);
	TAILQ_INSERT_AFTER(&chunks, front_guard, chunk, chunks);
	TAILQ_INSERT_AFTER(&chunks, chunk, end_guard, chunks);
	LIST_INSERT_HEAD(&free_chunks, chunk, free_chunks);
}
