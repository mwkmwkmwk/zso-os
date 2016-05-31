SOURCES = boot.s kernel.c gdt.c string.c memory.c load.c
HEADERS = gdt.h string.h kernel.h io.h

USOURCES = user_start.s user.c

kernel: $(SOURCES) kernel.lds Makefile $(HEADERS) user
	clang --target=i386-unknown -o kernel $(SOURCES) -nostdlib -ffreestanding -Wl,-T,kernel.lds -Wl,--build-id=none -g

user: $(USOURCES) user.lds Makefile
	clang --target=i386-unknown -o user $(USOURCES) -nostdlib -ffreestanding -Wl,-T,user.lds -Wl,--build-id=none
