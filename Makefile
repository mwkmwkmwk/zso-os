kernel.bin: real.bin pm.bin b64.bin main.elf
	cat real.bin pm.bin b64.bin main.elf > kernel.bin

real.bin: real.asm
	nasm -fbin real.asm -o real.bin
pm.bin: pm.asm
	nasm -fbin pm.asm -o pm.bin
b64.bin: b64.asm
	nasm -fbin b64.asm -o b64.bin

user.bin: user.asm
	nasm -fbin user.asm -o user.bin
user2.bin: user2.asm
	nasm -fbin user2.asm -o user2.bin

%.o: %.c user.bin user2.bin
	gcc -m64 -ffreestanding -mcmodel=large -O0 -c $< -o $@

OBJS = main.o idt.o malloc.o utils.o stdio.o serial.o

main.elf: $(OBJS) main.lds
	ld -o main.elf $(OBJS) -T main.lds -z max-page-size=4096
