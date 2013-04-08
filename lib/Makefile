CC =gcc
LD =ld
AS =nasm
ASFLAGS =-f elf -I../include
CFLAGS =-Wall -O2 -fomit-frame-pointer -c -I../include

.PHONY : clean

default : string.o

string.o : string.asm
	$(AS) $< $(ASFLAGS) -o string.o

clean :
	rm *.o -f

