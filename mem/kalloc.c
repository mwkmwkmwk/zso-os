#include "kalloc.h"

#include <stdbool.h>
#include <stddef.h>

#include "mem/page.h"
#include "panic.h"
#include "stdlib/math.h"
#include "stdlib/string.h"

// A totally unoptimal heap allocator

// Heap block layout:
// 		[heap_block stuct]
//      [free buffer with heap_block.size bytes]

struct heap_block {
	struct heap_block* prev;
	struct heap_block* next;
	uint32_t size;
	bool free; // TODO: this bool wastes 4 bytes for every allocation
};

// Free blocks list head, should never be unlinked
// The list is always sorted asceding (prev < this < next, excluding the head an its neighbours)
struct heap_block heap_blocks_head = {
	prev: &heap_blocks_head,
	next: &heap_blocks_head,
	size: 0
};

void init_kalloc(void) {
	struct heap_block* block = (struct heap_block*)KERNEL_HEAP_START;
	block->size = KERNEL_HEAP_SIZE - sizeof(struct heap_block);
	block->prev = block->next = &heap_blocks_head;
	block->free = true;
	heap_blocks_head.next = heap_blocks_head.prev = block;
}

void* kalloc(uint32_t size) {
	struct heap_block* it;

	size = align_up(size, 4);

	// Find a suitable block
	for (it = heap_blocks_head.next; it != &heap_blocks_head; it = it->next) {
		if (it->free && it->size >= size)
			break;
	}
	if (it == &heap_blocks_head)
		panic("No more free space on kernel heap to satisfy allocation request!");
	
	it->free = false;

	// Split the space, if it makes any sense
	if (it->size > size + sizeof(struct heap_block)) {
		struct heap_block* block = (struct heap_block*)((char*)(it + 1) + size);
		block->size = it->size - size - sizeof(struct heap_block);
		block->free = true;
		block->prev = it;
		block->next = it->next;
		it->next = block;
		block->next->prev = block;
		it->size = size;
		//kfree(block + 1);
	}

	#ifdef _DEBUG
	memset(it + 1, 0xDD, it->size);
	#endif

	return it + 1;
}

// ptr points to user buffer (so, after the heap_block struct)
void kfree(void* ptr) {
	struct heap_block* block = ((struct heap_block*)ptr) - 1;

	block->free = true;
	
	// Merge with neighbours if possible
	if (block->next->free) {
		block->size += block->next->size + sizeof(struct heap_block);
	}
	if (block->prev->free) {
		block->prev->size += block->size + sizeof(struct heap_block);
		block = block->prev;
	}

	#ifdef _DEBUG
	memset(ptr, 0xCC, block->size);
	#endif
}
