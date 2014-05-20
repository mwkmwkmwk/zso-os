kernel: kernel.c kernel.lds
	gcc -m32 -ffreestanding -nostdlib kernel.c -o kernel -Wl,-T,kernel.lds

install: kernel
	sudo mount fd.img fd
	sudo cp kernel fd/
	sudo umount fd
