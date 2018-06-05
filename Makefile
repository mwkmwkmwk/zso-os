all: kernel loader

OBJS := main.o

kernel: kernel.lds kernel.o $(OBJS)
	ld -Tkernel.lds kernel.o -o kernel $(OBJS)

kernel.o: kernel.s
	gcc -m64 kernel.s -c -o kernel.o

loader: loader.lds loader.o
	ld -m elf_i386 -Tloader.lds loader.o -o loader

loader.o: loader.s kernel
	gcc -m32 loader.s -c -o loader.o

%.o: %.c
	clang $< -o $@ -c -ffreestanding -O3 -Wall
