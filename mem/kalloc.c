#include "kalloc.h"

#include <stdbool.h>
#include <stddef.h>

#include "mem/page.h"
#include "panic.h"
#include "stdlib/assert.h"
#include "stdlib/list.h"
#include "stdlib/math.h"
#include "stdlib/printf.h"
#include "stdlib/string.h"

// A totally unoptimal heap allocator

// Heap block layout:
// 		[heap_block stuct]
//      [free buffer with heap_block.size bytes]

struct heap_block {
	struct list list_node; // has to be the first field so we don't need container_of()
	uint32_t size;
	bool free; // TODO: this bool wastes 4 bytes for every allocation
	char marker[3];
};

// Free blocks list head, should never be unlinked
// The list is always sorted asceding (prev < this < next, excluding the head an its neighbours)
struct heap_block heap_blocks_head;

void init_kalloc(void) {
	heap_blocks_head.size = 0;
	list_init(&heap_blocks_head.list_node);

	struct heap_block* block = (struct heap_block*)KERNEL_HEAP_START;
	block->size = KERNEL_HEAP_SIZE - sizeof(struct heap_block);
	block->free = true;
	block->marker[2] = 'X';
	list_insert_after(&heap_blocks_head.list_node, &block->list_node);
}

uint32_t align_offset(struct heap_block* block, uint32_t alignment) {
	return (align_up((uint32_t)(block+1), alignment) - (uint32_t)(block+1));
}

void* aligned_kalloc(uint32_t size, uint32_t alignment) {
	struct heap_block* it;

	size = align_up(size, 4);

	// Find a suitable block
	for (it = (struct heap_block*)heap_blocks_head.list_node.next; 
		 it != &heap_blocks_head;
		 it = (struct heap_block*)it->list_node.next)
	{
		if (it->free && it->size - align_offset(it, alignment) >= size)
			break;
	}
	if (it == &heap_blocks_head)
		panic("No more free space on kernel heap to satisfy allocation request!");
	
	it->free = false;
	uint32_t padding_size = align_offset(it, alignment);

	// Split the space, if it makes any sense
	if (it->size > padding_size + size + sizeof(struct heap_block)) {
		struct heap_block* block = (struct heap_block*)((char*)(it + 1) + padding_size + size);
		block->size = it->size - padding_size - size - sizeof(struct heap_block);
		block->free = true;
		block->marker[2] = 'X';
		it->size = padding_size + size;
		list_insert_after(&it->list_node, &block->list_node);
	}
	memset(it+1, 'P', padding_size);

	void* res = (char*)(it + 1) + padding_size;
	#ifdef _HEAP_DEBUG
	memset(res, 0xDD, it->size - padding_size);
	#endif

	assert(((uint32_t)res) % alignment == 0);

	return res;
}

void* kalloc(uint32_t size) {
	return aligned_kalloc(size, 4);
}

// ptr points to user buffer (so, after the heap_block struct)
void kfree(void* ptr) {
	char* ptr_it = ptr;
	while (*(ptr_it - 1) != 'X') // Skip alignment padding
		ptr_it--;
	struct heap_block* block = ((struct heap_block*)ptr_it) - 1;

	block->free = true;
	
	// Merge with neighbours if possible
	struct heap_block* next = ((struct heap_block*)block->list_node.next);
	struct heap_block* prev = ((struct heap_block*)block->list_node.prev);
	if (next->free) {
		list_unlink(&next->list_node);
		block->size += next->size + sizeof(struct heap_block);
	}
	if (prev->free) {
		list_unlink(&prev->list_node);
		prev->size += block->size + sizeof(struct heap_block);
		block = (struct heap_block*)prev;
	}

	#ifdef _HEAP_DEBUG
	memset(ptr_it, 0xCC, block->size);
	#endif
}
