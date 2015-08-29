#include "kalloc.h"

#include <stddef.h>

#include "mem/page.h"
#include "stdlib/string.h"

// A totally unoptimal heap allocator

// Free chunk layout:
// 		[free_block stuct]
//      [free buffer with free_block.size bytes]
// Allocated chunk:
// 		[free_block stuct] <- only "size" field is valid!
//		[user buffer]

struct free_block {
	struct free_block* prev;
	struct free_block* next;
	uint32_t size;
};

// Free blocks list head, should never be unlinked
struct free_block free_blocks_head = {
	prev: &free_blocks_head,
	next: &free_blocks_head,
	size: 0
};

void init_kalloc(void) {
	struct free_block* block = (struct free_block*)KERNEL_HEAP_START;
	block->size = KERNEL_HEAP_SIZE - sizeof(struct free_block);
	kfree(block + 1);
}

void* kalloc(uint32_t size) {
	struct free_block* it;

	// Find a suitable block
	for (it = free_blocks_head.next; it != &free_blocks_head; it = it->next) {
		if (it->size >= size)
			break;
	}
	
	// Unlink from the free list
	it->prev->next = it->next;
	it->next->prev = it->prev;

	// Split the space, if it makes any sense
	if (it->size > size + sizeof(struct free_block)) {
		struct free_block* block = (struct free_block*)((char*)(it + 1) + size);
		block->size = it->size - size - sizeof(struct free_block);
		it->size = size;
		kfree(block + 1);
	}

	#ifdef _DEBUG
	memset(it + 1, 0xDD, it->size);
	#endif

	return it + 1;
}

// ptr points to user buffer (so, after the free_block struct)
void kfree(void* ptr) {
	struct free_block* block = ((struct free_block*)ptr) - 1;

	// Add the block to the free list
	block->prev = &free_blocks_head;
	block->next = free_blocks_head.next;
	free_blocks_head.next = block;
	block->next->prev = block;
	
	#ifdef _DEBUG
	memset(ptr, 0xCC, block->size);
	#endif
}