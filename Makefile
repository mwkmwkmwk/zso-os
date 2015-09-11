
CFLAGS = -ffreestanding -m32 -nostdlib -std=gnu99 -fomit-frame-pointer -O1 \
         -g -I . -Wall #-D_DEBUG
USERMODE_CFLAGS = -D USERMODE $(CFLAGS)
MODULES = boot io mem stdlib

.PHONY: clean

usermode/%.o: usermode/%.c usermode/*.h
	gcc $(USERMODE_CFLAGS) -o $@ -c $<

usermode/%.o: usermode/%.asm
	nasm -f elf32 $< -o $@ 

usermode/%.o: usermode/%.s
	gcc $(CFLAGS) -o $@ -c $<

%.o: %.asm
	nasm -f elf32 $< -o $@ 

%.o: %.s
	gcc $(CFLAGS) -o $@ -c $<

%.o: %.c *.h
	gcc $(CFLAGS) -o $@ -c $<

OBJS = boot/main.o 		        \
       boot/start.o 	        \
       io/interrupts.o 	        \
       io/int_entry.o 	        \
       io/io.o 			        \
       io/keyboard.o 	        \
       io/pic.o 		        \
       mem/kalloc.o 	        \
       mem/gdt.o 		        \
       mem/page.o 		        \
       mem/pmalloc.o 	        \
       stdlib/math.o 	        \
       stdlib/printf.o 	        \
       stdlib/string.o 	        \
       syscalls/syscalls.o      \
       usermode/user.o          \
       usermode/user_syscalls.o \
       video/vga.o

kernel: $(OBJS) kernel.lds
	gcc $(CFLAGS) -o $@ $(OBJS) -Wl,-T,kernel.lds

clean:
	rm -f *.o $(addsuffix /*.o,$(MODULES)) kernel
