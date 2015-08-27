
CFLAGS = -ffreestanding -m32 -nostdlib -std=gnu99 -fomit-frame-pointer -O1 \
         -g -I .
MODULES = boot io mem stdlib

.PHONY: clean

%.o: %.asm
	nasm -f elf32 $< -o $@ 

%.o: %.s
	gcc $(CFLAGS) -o $@ -c $<

%.o: %.c *.h
	gcc $(CFLAGS) -o $@ -c $<

OBJS = boot/main.o boot/start.o io/io.o video/vga.o mem/gdt.o int.o \
       io/pic.o mem/pmalloc.o mem/page.o stdlib/printf.o stdlib/string.o \
       user.o

kernel: $(OBJS) kernel.lds
	gcc $(CFLAGS) -o $@ $(OBJS) -Wl,-T,kernel.lds

clean:
	rm -f *.o $(addsuffix /*.o,$(MODULES)) kernel
