SECTIONS {
	ENTRY(_start)
	. = 0x100000;
	kernel_start = .;
	header : { *(header) }
	.text : { *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	.bss : { *(.bss) }
	.junk : { *(*) }
	kernel_end = .;
}
