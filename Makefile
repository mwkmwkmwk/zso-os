SOURCES = boot.s kernel.c gdt.c

kernel: $(SOURCES) kernel.lds Makefile gdt.h
	clang --target=i386-unknown -o kernel $(SOURCES) -nostdlib -ffreestanding -Wl,-T,kernel.lds -Wl,--build-id=none
