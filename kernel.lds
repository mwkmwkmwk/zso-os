SECTIONS {
	ENTRY(_start)
	. = 0x100000;
	mb_header : { *(mb_header) }
	.text : { *(.text) }
	.rodata : { *(.rodata*) }
	.data : { *(.data) }
	.bss : { *(.bss) }
}
