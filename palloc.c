#include "palloc_np.h"
#include "palloc.h"
#include "print.h"

static inline uint32_t get_page_directory(void)
{
    return 0x3ff << 22;
}

uint32_t palloc(void) {
    uint32_t *pfree_list = 0;
    uint32_t res = *pfree_list;

    printf("palloc phys %x virt %x\n", res, res << 22);

    if (res)
        *pfree_list = *(uint32_t *)(res << 22);
    return res;
}

inline uint32_t get_page_table_entry_address(uint32_t pd, uint32_t virt)
{
	int pdi = virt >> 22 & 0x3ff; //1024 values
	int pti = virt >> 12 & 0x3ff; //1024 values
    uint32_t pt = (pd + pdi) & ~0xfff; //TODO o co tu chodzi?

    printf("virt %x pd %x pt %x pdi %d pti %d\n", virt, pd, pt, pdi, pti);

    return pt + sizeof(uint32_t) * pti;
}


// with virtual address
void pfree(uint32_t addr) {
    uint32_t pd = get_page_directory();
    uint32_t pte = get_page_table_entry_address(pd, addr);

    /*
    *(uint32_t *)temp = addr; //create mapping for page addr at virtual address 2

    printf("pfree phys %x virt %x\n", addr, 2);
	*(uint32_t *) (2 << 12) = *(uint32_t *)0; //TODO phys to virt
	*(uint32_t *) 0 = addr | 0x1;
    */
    *(uint32_t *)addr = *(uint32_t *)0;
    *(uint32_t *)0 = *(uint32_t *)pte & ~0xfff;
}


uint32_t valloc(int cnt, uint32_t start, uint32_t end) {
    uint32_t pd = get_page_directory();
    int current = 0;
    uint32_t page;

	printf("from cr3 PD %x pd %x\n", pd, *(uint32_t *)pd);


    for (page = start; page < end && current < cnt; page += 0x1000)
    {
        uint32_t pte = get_page_table_entry_address(pd, page);
        if (*(uint32_t *)pte == 0)
            ++current;
        else
            current = 0;
    }
    if (current == cnt)
    {
        end = current;
        for (; current > 0; --current, page -=0x1000)
        {
            uint32_t pte = get_page_table_entry_address(pd, page);

            printf("reserving page %x\n", page);
            *(uint32_t *) pte = 2;
        }
        page += 0x1000;
    } else
        printf("couldn't reserve virtual address range");

    return page;
}

void vfree(uint32_t page, int cnt) {
    uint32_t pd = get_page_directory();

    for (; cnt > 0; --cnt, page +=0x1000)
    {
        uint32_t pte = get_page_table_entry_address(pd, page);

        printf("releasing  page %x\n", page);
        * (uint32_t *) pte = 0;
    }
}

void map_page(uint32_t phys, uint32_t virt, int flags) {
    uint32_t pd = get_page_directory();
    uint32_t pte = get_page_table_entry_address(pd, virt);
    
    printf("map page phys %x virt %x flags %x ppte %x\n", phys, virt, flags, pte);

    if (*(uint32_t *)pte == 2)
        *(uint32_t *)pte = phys | flags;
    else {
		printf("PANIC\n");
		printf("attempting to map page without reserving virtual memory first\n");
		asm("cli\nhlt\n");
		__builtin_unreachable();
    }
}

void unmap_page(uint32_t virt) {
    uint32_t pd = get_page_directory();
    uint32_t pte = get_page_table_entry_address(pd, virt);

    if (*(uint32_t *)pte == 0 || *(uint32_t *)pte == 2)
    {
		printf("PANIC\n");
		printf("attempting to unmap unmapped page\n");
		asm("cli\nhlt\n");
		__builtin_unreachable();
    }

    *(uint32_t *)pte = 2;
}

#define MAP_USER 0x2
#define MAP_RW   0x4

void *malloc(uint32_t size) {
    uint32_t real_size = (size + 0x1000) & ~0xfff;
    int num_pages = real_size >> 12;

    if (size > 0)
    {
        int i;
        uint32_t virt;

        virt = valloc(num_pages, 0x1000, 0xb8000);

        if (virt > 0xb8000)
            return 0;

        for (i = 0; i < num_pages; ++i) {
            uint32_t page = palloc();
            map_page(page, virt + i * 0x1000, 1 | MAP_RW );
        }

        *(int*)virt = num_pages;

        printf("malloc size %u real_size %u num_pages %u real_addr %x res %x\n", size, real_size, num_pages, virt, virt + sizeof(int));

        return (void *)(virt + sizeof(int));
    }

    return 0;
}

void free(void *addr) {
    uint32_t virt = ((uint32_t) addr) - sizeof(int);

    if (addr)
    {
        uint32_t pd = get_page_directory();
        int i;
        int num_pages = *(int *)virt; //TODO fails here
        printf("free addr %x pages %d\n", addr, num_pages);

        for (i = 0; i < num_pages; ++i)
        {
            uint32_t page = virt + i *0x1000;
            uint32_t pte = get_page_table_entry_address(pd, page);
            uint32_t phys = * (uint32_t *)pte;
            
            pfree(page);
            unmap_page(page);
            printf("removing page virt %x phys %x\n", page, phys);
        }
        //vfree(virt, num_pages);
    }
}
