
CFLAGS = -ffreestanding -m32 -nostdlib -std=gnu99 -fomit-frame-pointer -O2

.PHONY: clean

%.o: %.s
	gcc $(CFLAGS) -o $@ -c $<

%.o: %.c *.h
	gcc $(CFLAGS) -o $@ -c $<

OBJS = main.o start.o io.o vga.o gdt.o int.o pic.o

kernel: $(OBJS)
	gcc $(CFLAGS) -o $@ $(OBJS)

clean:
	rm -f *.o