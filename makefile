ENTRYPOINT = 0x30000
ASM = nasm
ASMBOOTFLAGS = -I inc/ -I lib/
ASMLOADERFLAGS = -I inc/ -I lib/
ASMKERNELFLAGS = -f elf
CFLAGS = -I kclib/ -c -fno-builtin
LDFLAGS = -e _start -Ttext $(ENTRYPOINT)
CC = gcc
LD = ld
OPY = objcopy

#Add corresponding .o files from kclib and kalib
KERNELBINPRE = kernel/kernel.o kclib/KeKernelEntry.o kalib/_asm_KeMemory32.o kalib/_asm_KeGraph32.o kalib/_asm_KeIO32.o kalib/_asm_KeProcess32.o kclib/KeProcess32.o kclib/KeGraph32.o kclib/KeIO32.o kclib/KeMemory32.o

#Add all .o files 
OBJECTFILES = kernel/kernel.o kclib/KeKernelEntry.o kalib/_asm_KeMemory32.o kalib/_asm_KeGraph32.o kalib/_asm_KeIO32.o kalib/_asm_KeProcess32.o kclib/KeProcess32.o kclib/KeGraph32.o kclib/KeIO32.o kclib/KeMemory32.o

#Add all .bin files
BINFILES = boot/boot.bin boot/loader.bin kernel/kernel.bin

#Add all temp files
TEMPFILES = kernel/kernel.out

#Prerequisites
#Add new .h/.c files form kclib
KEKERNELENTRYPRE = kclib/KeKernelEntry.c kclib/KeGraph32.h kclib/KeDef.h kclib/KeGlobalVariables.h kclib/KeMemory32.h kclib/KeIO32.h kclib/KeProcess32.h kclib/KeProcess32.c kclib/KeGraph32.c kclib/KeMemory32.c kclib/KeProcess32.c kclib/KeIO32.c kclib/KeKernelStruct32.h

#No need to change
_ASM_KEGRAPH32PRE = kalib/_asm_KeGraph32.asm

_ASM_KEMEMORY32PRE = kalib/_asm_KeMemory32.asm

KERNELOPRE = kernel/kernel.asm

BOOTBINPRE = boot/boot.asm inc/FAT12Header.inc lib/fat12readfile.lib

LOADERBINPRE = boot/loader.asm inc/FAT12Header.inc inc/pm.inc lib/fat12readfile.lib lib/io16.lib lib/io32.lib lib/mem32.lib

_ASM_KEIO32PRE = kalib/_asm_KeIO32.asm

_ASM_KEPROCESS32PRE = kalib/_asm_KeProcess32.asm




 
.PHONY : everything clean all image buildimg
everything : $(BINFILES) $(OBJECTFILES) $(TEMPFILES)

all : clean everything

image: all buildimg 

clean:
	rm -rf $(BINFILES) $(OBJECTFILES) $(TEMPFILES)

buildimg: 
		dd if=boot/boot.bin of=a.img bs=512 count=1 conv=notrunc
		cp -f kernel/kernel.bin kernel.bin
		cp -f boot/loader.bin loader.bin
		rm -rf $(BINFILES) $(OBJECTFILES) $(TEMPFILES)

boot/boot.bin: $(BOOTBINPRE)
		$(ASM) $(ASMBOOTFLAGS) -o boot/boot.bin boot/boot.asm

boot/loader.bin: $(LOADERPRE)
		$(ASM) $(ASMBOOTFLAGS) -o boot/loader.bin boot/loader.asm
		
kernel/kernel.o: $(KERNELOPRE)
		$(ASM) $(ASMKERNELFLAGS) -o kernel/kernel.o kernel/kernel.asm
		
kalib/_asm_KeMemory32.o: $(_ASM_KEMEMORY32PRE)
		$(ASM) $(ASMKERNELFLAGS) -o kalib/_asm_KeMemory32.o kalib/_asm_KeMemory32.asm

kalib/_asm_KeGraph32.o: $(_ASM_KEGRAPH32PRE)
		$(ASM) $(ASMKERNELFLAGS) -o kalib/_asm_KeGraph32.o kalib/_asm_KeGraph32.asm
		
kalib/_asm_KeProcess32.o: $(_ASM_KEGRAPH32PRE)
		$(ASM) $(ASMKERNELFLAGS) -o kalib/_asm_KeProcess32.o kalib/_asm_KeProcess32.asm
		
kernel/kernel.bin: $(KERNELBINPRE)
		$(LD) $(OBJECTFILES) $(LDFLAGS) -o kernel/kernel.out
		$(OPY) kernel/kernel.out -O binary kernel/kernel.bin
		
kalib/_asm_KeIO32.o: $(_ASM_KEIO32PRE)
		$(ASM) $(ASMKERNELFLAGS) -o kalib/_asm_KeIO32.o kalib/_asm_KeIO32.asm
		
#C LIBS
kclib/KeKernelEntry.o: $(KEKERNELENTRYPRE)
		$(CC) $(CFLAGS) -o kclib/KeKernelEntry.o kclib/KeKernelEntry.c

kclib/KeIO32.o: $(KERNELBINPRE)
		$(CC) $(CFLAGS) -o kclib/KeIO32.o kclib/KeIO32.c

kclib/KeGraph32.o: $(KERNELBINPRE)
		$(CC) $(CFLAGS) -o kclib/KeGraph32.o kclib/KeGraph32.c

kclib/KeProcess32.o: $(KERNELBINPRE)
		$(CC) $(CFLAGS) -o kclib/KeProcess32.o kclib/KeProcess32.c

kclib/KeMemory32.o: $(KERNELBINPRE)
		$(CC) $(CFLAGS) -o kclib/KeMemory32.o kclib/KeMemory32.c


