SECTIONS {
	ENTRY(_start)
	. = 0xffff800000200000 + SIZEOF_HEADERS;
	.text : { *(.text*) } :text
	.rodata : { *(.rodata*) } :text
	.data : { *(.data*) } :text
	kbss_start = .;
	.bss : { *(.bss*) *(COMMON) } :text
	kbss_end = .;
	/DISCARD/ : { *(.comment) }
}

PHDRS {
	text PT_LOAD FILEHDR PHDRS;
}
