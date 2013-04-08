AS =nasm
CC =gcc
LD =ld
CFLAGS =-g -Wall -O2 -fomit-frame-pointer -c
ASFLAGS =-f elf64 
LDFLAGS =-Ttext 0 -N -e _start --oformat binary

KERNEL_SYS :=kernel.sys
KERNEL_SRC :=kernel.asm

.PHONY : default, clean

default : $(KERNEL_SYS)

$(KERNEL_SYS) : $(KERNEL_SRC)
	$(AS) $< $(ASFLAGS) -o kernel.o
	$(LD) $(LDFLAGS) kernel.o -o $@

clean :
	rm $(KERNEL_SYS) -f
	rm *.o -f
