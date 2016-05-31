SECTIONS {
	ENTRY(_start)
	. = 0x80000000;
	entry : { *(entry) }
	.text : { *(.text) }
	.rodata : { *(.rodata*) }
	.data : { *(.data) }
	.bss : { *(.bss) }
}
