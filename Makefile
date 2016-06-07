SOURCES = boot.s kernel.c gdt.c string.c memory.c load.c proc.c proc_s.s
HEADERS = gdt.h string.h kernel.h io.h proc.h

USOURCES = user_start.s user.c
U2SOURCES = user_start.s user2.c

kernel: $(SOURCES) kernel.lds Makefile $(HEADERS) user user2
	clang --target=i386-unknown -o kernel $(SOURCES) -nostdlib -ffreestanding -Wl,-T,kernel.lds -Wl,--build-id=none -g

user: $(USOURCES) user.lds Makefile
	clang --target=i386-unknown -o user $(USOURCES) -nostdlib -ffreestanding -Wl,-T,user.lds -Wl,--build-id=none

user2: $(U2SOURCES) user.lds Makefile
	clang --target=i386-unknown -o user2 $(U2SOURCES) -nostdlib -ffreestanding -Wl,-T,user.lds -Wl,--build-id=none
