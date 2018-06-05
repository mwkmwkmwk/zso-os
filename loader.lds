SECTIONS {
	ENTRY(_start)
	. = 0x100000 + SIZEOF_HEADERS;
	mb_hdr : { *(mb_hdr) } :text
	.text : { *(.text*) } :text
	.rodata : { *(.rodata*) } :text
	.data : { *(.data*) } :text
	ptables : { *(ptables) } :text
	. = 0x200000;
	kernel : { *(kernel) } :text
	.bss : { *(.bss*) *(COMMON) } :text
	/DISCARD/ : { *(.comment) }
}

PHDRS {
	text PT_LOAD FILEHDR PHDRS;
}
