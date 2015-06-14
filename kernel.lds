ENTRY(_start)
SECTIONS {
	. = 0x100000;
	kernel_start = .;
	.text : {
		*(mb_hdr)
		*(.text)
		*(.rodata)
	}
	.data : {
		*(.data)
		*(.bss)
	}
	kernel_end = .;
}
