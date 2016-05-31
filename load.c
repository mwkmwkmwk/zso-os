#include <elf.h>
#include "kernel.h"
#include "string.h"

uint32_t load_user(const uint8_t *elf) {
	Elf32_Ehdr *ehdr = (void*)elf;
	uint32_t res = ehdr->e_entry;
	Elf32_Phdr *phdr = (void *)(elf + ehdr->e_phoff);
	int i;
	for (i = 0; i < ehdr->e_phnum; i++, phdr++) {
		if (phdr->p_type != PT_LOAD)
			continue;
		uint32_t start = phdr->p_paddr;
		uint32_t fend = start + phdr->p_filesz;
		uint32_t mend = start + phdr->p_memsz;
		start &= ~0xfff;
		uint32_t addr;
		for (addr = start; addr < mend; addr += 0x1000) {
			map_user(addr, palloc(), !!(phdr->p_flags & PF_W));
		}
		int j;
		memcpy((void*)phdr->p_paddr, elf + phdr->p_offset, phdr->p_filesz);
		memset((void*)fend, 0, mend-fend);
	}
	return res;
}
