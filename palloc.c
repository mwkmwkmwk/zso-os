#include "palloc_np.h"
#include "palloc.h"
#include "print.h"


extern uint8_t kernel_start[];

// pti in range (0, 1023 (4kB / sizeof (int) - 1)
static inline uint32_t get_page_table_address(int pti)
{
    return (0x3ff << 22) | (pti << 12);
}

static inline uint32_t get_page_directory_address(void)
{
    return get_page_table_address(0x3ff); 
}

uint32_t palloc(void) {
    uint32_t *pfree_list = (uint32_t *)get_page_table_address(0); 
    uint32_t *pfree_next = (uint32_t *)0;
    uint32_t res = *pfree_list;

    *pfree_list = *pfree_next | 0x1;

    return res;
}

void pfree(uint32_t addr) {
    uint32_t *pfree_list = (uint32_t *) get_page_table_address(0);
    uint32_t *pfree_next = (uint32_t *)0;
    uint32_t tail = *pfree_list & ~0xfff;

    *pfree_list = addr;
    *pfree_next = tail;
}

inline uint32_t get_page_table_entry_address(uint32_t virt)
{
	int pdi = virt >> 22 & 0x3ff;
	int pti = virt >> 12 & 0x3ff;
    uint32_t offset = pti * sizeof (uint32_t); //12 bit
    uint32_t *ppde = (uint32_t *)(get_page_directory_address() | pdi * sizeof(uint32_t));
    uint32_t pte = get_page_table_address(pdi) | offset;

	if (!*ppde) 
    {
		*ppde = palloc() | 0x1;
	}

    return pte;
}




uint32_t valloc(int cnt, uint32_t start, uint32_t end) {
    int current = 0;
    uint32_t page;

    for (page = start; page < end && current < cnt; page += 0x1000)
    {
        uint32_t *ppte = (uint32_t *) get_page_table_entry_address(page);
        if (*ppte == 0)
            ++current;
        else
            current = 0;
    }

    if (current == cnt)
    {
        for (; current > 0; --current)
        {
            uint32_t *ppte;
            
            page -= 0x1000;
            ppte = (uint32_t *) get_page_table_entry_address(page);

            *ppte = 2;
        }
    } else
        printf("couldn't reserve virtual address range pages %d\n", cnt);

    return page;
}

void vfree(uint32_t page, int cnt) {

    for (; cnt > 0; --cnt, page += 0x1000)
    {
        uint32_t *ppte = (uint32_t *) get_page_table_entry_address(page);

        if (*ppte == 2)
            *ppte = 0;
        else 
        {
            printf("PANIC\n");
            printf("attempting to unmap used page\n");
            asm("cli\nhlt\n");
            __builtin_unreachable();
        }
    }
}

void map_page(uint32_t phys, uint32_t virt, int flags) {
    uint32_t *ppte = (uint32_t *) get_page_table_entry_address(virt);
    
    if (*ppte == 2)
        *ppte = phys | flags;
    else 
    {
		printf("PANIC\n");
		printf("attempting to map page without reserving virtual memory first\n");
		asm("cli\nhlt\n");
		__builtin_unreachable();
    }
}

void unmap_page(uint32_t virt) {
    uint32_t *ppte = (uint32_t *) get_page_table_entry_address(virt);

    if (*ppte == 0 || *ppte == 2)
    {
		printf("PANIC\n");
		printf("attempting to unmap the unmapped page\n");
		asm("cli\nhlt\n");
		__builtin_unreachable();
    }

    *ppte = 2;
}

#define MAP_USER 0x2
#define MAP_RW   0x4

void *malloc(uint32_t size) {
    if (size > 0)
    {
        uint32_t real_size = (size + 0x1000) & ~0xfff;
        int num_pages = real_size >> 12;
        uint32_t virt;

        virt = valloc(num_pages, 0, (uint32_t) &kernel_start);

        if (virt < 0xb8000)
        {
            uint32_t *pvirt = (uint32_t *) virt;
            int i;

            for (i = 0; i < num_pages; ++i) {
                uint32_t page = palloc();
                map_page(page, virt + i * 0x1000, 0x1 | MAP_RW  | MAP_USER);
            }

            pvirt[0] = num_pages;

            return pvirt + 1;
        }
    }

    printf("malloc failed size %x\n", size);

    return 0;
}

void free(void *addr) {
    if (addr)
    {
        uint32_t *pvirt = ((uint32_t *) addr) - 1;
        uint32_t virt = (uint32_t) pvirt;
        int i;
        int num_pages = *pvirt;

        for (i = 0; i < num_pages; ++i)
        {
            uint32_t page = virt + i *0x1000;
            uint32_t pte = get_page_table_entry_address(page);
            uint32_t phys = *(uint32_t *)pte;
            
            unmap_page(page);
            pfree(phys);
        }

        vfree(virt, num_pages);
    }
}
