SECTIONS {
	ENTRY(_start)
	. = 0x100000;
	header : { *(header) }
	.text : { *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	.bss : { *(.bss) }
	.junk : { *(*) }
        heap = .;
}
