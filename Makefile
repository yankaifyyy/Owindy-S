AS =nasm
CC =gcc
LD =ld
CFLAGS =-g -Wall -O2 -fomit-frame-pointer -m32
LDFLAGS =-Ttext 0 -N -e _start --oformat binary -m elf_i386

IMG :=a.img
MOUNT_POINT :=./mount/

BOOT_BIN =bootloader/ipl.sys
LDR_BIN =bootloader/loader.sys
KERNEL_BIN =kernel/kernel.sys

.PHONY : default, clean, bootloader, kerenel, lib

default : $(BOOT_BIN) $(LDR_BIN) $(KERNEL_BIN)
	dd if=$(BOOT_BIN) of=$(IMG) bs=512 count=1 conv=notrunc
	sudo mount -o loop $(IMG) $(MOUNT_POINT)
	sudo cp $(LDR_BIN) $(MOUNT_POINT) -v
	sudo cp $(KERNEL_BIN) $(MOUNT_POINT) -v
	sudo umount $(MOUNT_POINT)

bootloader : $(BOOT_BIN) $(LDR_BIN)
	dd if=$(BOOT_BIN) of=$(IMG) bs=512 count=1 conv=notrunc
	sudo mount -o loop $(IMG) $(MOUNT_POINT)
	sudo cp $(LDR_BIN) $(MOUNT_POINT) -v
	sudo umount $(MOUNT_POINT)

lib :
	(cd lib; make)

kernel : $(KERNEL_BIN)
	sudo mount -o loop $(IMG) $(MOUNT_POINT)
	sudo cp $(KERNEL_BIN) $(MOUNT_POINT) -v
	sudo umount $(MOUNT_POINT)

$(BOOT_BIN) : bootloader/boot_ipl.asm include/asm/boot.inc include/asm/fat12.inc include/asm/fun32.inc include/asm/pm.inc
	$(AS) $< -o $@ -Iinclude/

$(LDR_BIN) : bootloader/loader.asm include/asm/boot.inc include/asm/fat12.inc include/asm/fun32.inc include/asm/pm.inc
	$(AS) $< -o $@ -Iinclude/

$(KERNEL_BIN) :  lib
	(cd kernel; make)

clean :
	sudo mount -o loop $(IMG) $(MOUNT_POINT)
	sudo rm $(MOUNT_POINT)/* -rf
	sudo umount $(MOUNT_POINT)
	rm -rf $(BOOT_BIN) $(LDR_BIN)
	rm -rf bootloader/*.o
	(cd kernel; make clean)
	(cd lib; make clean)
