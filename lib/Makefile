CC =gcc
LD =ld
AS =nasm
ASFLAGS =-f elf -I../include
CFLAGS =-g -Wall -O2 -fomit-frame-pointer -c -I../include -m32 -std=gnu99

.PHONY : clean

default : util.o string.o

util.o : util.c
	$(CC) $< $(CFLAGS) -o util.o

string.o : string.asm
	$(AS) $< $(ASFLAGS) -o string.o

clean :
	rm *.o -f

