SECTIONS {
	. = 0xffff800000101000;
	ekran = 0xffff8000000b8000;
	ENTRY(_start)
	wszystko : { *(*) }
	. = ALIGN(0x1000);
	end = .;
}
