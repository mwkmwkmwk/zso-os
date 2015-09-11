ENTRY(_start)
SECTIONS {
	. = 0x100000;
	mb_hdr : {
		*(mb_hdr)
	}
	.user.text : ALIGN(0x1000) {
		user_start = .;
		usermode/*.o(.text)
		usermode/*.o(.rodata*)
	}
	.user.data : ALIGN(0x1000) {
		usermode/*.o(.data)
		usermode/*.o(.bss)
	}
	.text : ALIGN(0x1000) {
		user_end = .;
		kernel_start = .;
		*(.text)
		*(.rodata*)
	}
	.data : {
		*(.data)
	}
	.bss : {
		*(.bss)
	}
	.junk : ALIGN(0x1000){
	kernel_end = .;
	}
}
