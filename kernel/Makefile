AS =nasm
CC =gcc
LD =ld
CFLAGS =-Wall -O2 -fomit-frame-pointer -c -I../include -m32
ASFLAGS =-f elf -I../include
LDFLAGS =-Ttext 0x10000 -N -e _start --oformat binary -m elf_i386

KERNEL_SYS :=kernel.sys
KERNEL_SRC :=kernel.asm

.PHONY : default, clean

default : $(KERNEL_SYS)

$(KERNEL_SYS) : protect.o start.o kernel.o
	$(LD) $(LDFLAGS) kernel.o start.o protect.o ../lib/string.o -o $@

protect.o : protect.c ../include/protect.h ../include/global.h ../include/type.h
	$(CC) protect.c $(CFLAGS) -c

start.o : start.c ../include/protect.h ../include/global.h ../include/type.h
	$(CC) start.c $(CFLAGS) -c

kernel.o : kernel.asm
	$(AS) $< $(ASFLAGS) -o kernel.o

clean :
	rm $(KERNEL_SYS) -f
	rm *.o -f
