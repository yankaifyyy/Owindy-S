AS =nasm
CC =gcc
LD =ld
CFLAGS =-g -Wall -O2 -fomit-frame-pointer -c -I../include -m32 -std=gnu99 -fno-stack-protector
ASFLAGS =-f elf -I../include/
LDFLAGS =-Ttext 0x10000 -N -e _start --oformat binary -m elf_i386

KERNEL_SYS :=kernel.sys
KERNEL_SRC :=kernel.asm

.PHONY : default, clean

default : $(KERNEL_SYS)

$(KERNEL_SYS) : protect.o start.o kernel.o main.o proc.o
	$(LD) $(LDFLAGS) kernel.o start.o protect.o proc.o main.o ../lib/alib.o ../lib/util.o -o $@

protect.o : protect.c ../include/protect.h ../include/global.h ../include/type.h ../include/util.h ../include/proc.h
	$(CC) protect.c $(CFLAGS) -c

start.o : start.c ../include/protect.h ../include/global.h ../include/type.h ../include/util.h ../include/proc.h
	$(CC) start.c $(CFLAGS) -c

main.o : main.c ../include/protect.h ../include/global.h ../include/type.h ../include/util.h ../include/proc.h
	$(CC) main.c $(CFLAGS) -c

proc.o : proc.c ../include/protect.h ../include/global.h ../include/type.h ../include/util.h ../include/proc.h

kernel.o : kernel.asm ../include/asm/kconst.inc
	$(AS) $< $(ASFLAGS) -o kernel.o

clean :
	rm $(KERNEL_SYS) -f
	rm *.o -f
