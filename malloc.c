#include <stddef.h>

#include "malloc.h"


extern char end;
static void *malloc_pool = &end;

static void *free_list_head = NULL;

static void *find_free(size_t sz) {
    void *this = free_list_head,
         *prev = NULL;
    
    while (this != NULL) {
        //TODO maybe change to >= ?
        if (MALLOC_GET_SIZE(this) == sz) {
            if (prev == NULL) {
                free_list_head = MALLOC_GET_NEXT_PTR_ADDR(this);
            } else {
                MALLOC_GET_NEXT_PTR_ADDR(prev) = MALLOC_GET_NEXT_PTR_ADDR(this);
            }
            MALLOC_GET_NEXT_PTR_ADDR(this) = NULL;
            return this;
        }
        prev = this;
        this = MALLOC_GET_NEXT_PTR_ADDR(this);
    }

    return NULL;
}

void *malloc(size_t sz) {
    void *ptr = NULL;
    sz = (sz + MALLOC_ALIGN_BITS) & ~MALLOC_ALIGN_BITS;
    ptr = find_free(sz);
    if (ptr == NULL) {
        // if end is aligned then this can be removed
        malloc_pool = (void *)(((uint64_t)malloc_pool + MALLOC_ALIGN_BITS) & ~MALLOC_ALIGN_BITS);
        ptr = ADD_OFF(malloc_pool, MALLOC_HEADER_SIZE, void);
        malloc_pool += MALLOC_HEADER_SIZE + sz;
        MALLOC_GET_NEXT_PTR_ADDR(ptr) = NULL;
        MALLOC_GET_SIZE(ptr) = sz;
    }
    return ptr;
}

void free(void *ptr) {
    void *next = free_list_head;

    if (ptr == NULL) {
        return;
    }

    if (ADD_OFF(ptr, MALLOC_GET_SIZE(ptr), void *) == malloc_pool) {
        malloc_pool -= MALLOC_GET_SIZE(ptr) + MALLOC_HEADER_SIZE;
        return;
    }

    if (free_list_head == NULL) {
        free_list_head = ptr;
        return;
    }

    while (MALLOC_GET_NEXT_PTR_ADDR(next) != NULL) {
        next = MALLOC_GET_NEXT_PTR_ADDR(next);
    }
    MALLOC_GET_NEXT_PTR_ADDR(next) = ptr;
}

// uhm, I should be killed for writting this
void *alloc_pages(size_t count) {
    uint64_t res = (uint64_t)malloc ((count << 12) + 0xfff);
    return (void *)((res + 0xfff) & ~0xfff);
#if 0
    void *ptr = NULL,
         *ret = NULL;
    uint64_t size_to_page_end = (((uint64_t)malloc_pool + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1)) - (uint64_t)malloc_pool;

    if (size_to_page_end != MALLOC_HEADER_SIZE) {
        if (size_to_page_end == 0) {
            size_to_page_end = PAGE_SIZE;
        }
        ptr = malloc(size_to_page_end - 2 * MALLOC_HEADER_SIZE);
    }
    ret = malloc(PAGE_SIZE * count);
    free(ptr);
    return ret;
#endif
}
