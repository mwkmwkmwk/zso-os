
CFLAGS = -ffreestanding -m32 -nostdlib -std=gnu99 -fomit-frame-pointer -O1 -g

.PHONY: clean

%.o: %.s
	gcc $(CFLAGS) -o $@ -c $<

%.o: %.c *.h
	gcc $(CFLAGS) -o $@ -c $<

OBJS = main.o start.o io.o vga.o gdt.o int.o pic.o pmalloc.o page.o

kernel: $(OBJS) kernel.lds
	gcc $(CFLAGS) -o $@ $(OBJS) -Wl,-T,kernel.lds

clean:
	rm -f *.o kernel
