kernel.bin: real.bin pm.bin b64.bin main.elf
	cat real.bin pm.bin b64.bin main.elf > kernel.bin

real.bin: real.asm
	nasm -fbin real.asm -o real.bin
pm.bin: pm.asm
	nasm -fbin pm.asm -o pm.bin
b64.bin: b64.asm
	nasm -fbin b64.asm -o b64.bin

%.o: %.c
	gcc -m64 -ffreestanding -mcmodel=large -O3 -c $< -o $@

OBJS = main.o idt.o

main.elf: $(OBJS) main.lds
	ld -o main.elf $(OBJS) -T main.lds -z max-page-size=4096
