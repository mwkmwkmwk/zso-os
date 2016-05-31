SECTIONS {
	ENTRY(_start)
	. = 0x1000;
	entry : { *(entry) }
	.text : { *(.text) }
	.rodata : { *(.rodata*) }
	.data : { *(.data) }
	.bss : { *(.bss) }
}
