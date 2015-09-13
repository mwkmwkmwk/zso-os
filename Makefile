
CFLAGS = -ffreestanding -m32 -nostdlib -std=gnu99 -fomit-frame-pointer -O1 \
         -g -I . -Wall #-D_DEBUG
USERMODE_CFLAGS = -D USERMODE $(CFLAGS)
MODULES = boot io mem stdlib syscalls usermode video utils threading

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

#OBJS = $(wildcard $(addsuffix /*.o,$(MODULES)))
CFILES = $(foreach dir, $(MODULES), $(wildcard $(dir)/*.c))
ASMFILES = $(foreach dir, $(MODULES), $(wildcard $(dir)/*.asm))
OBJS = $(foreach cfile, $(CFILES) $(ASMFILES), $(patsubst %.asm,%.o,$(patsubst %.c,%.o,$(cfile))))

kernel: $(OBJS) kernel.lds
	gcc $(CFLAGS) -o $@ $(OBJS) -Wl,-T,kernel.lds

clean:
	rm -f *.o $(addsuffix /*.o,$(MODULES)) kernel
