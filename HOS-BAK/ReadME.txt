This is a simple operating system(still being developed). The image file is HOS.img. Please use bochs to see the real effect. What it enters protected mode, starts paging and runs a process. I did not have enough time to write elaborate comments on that. Therefore, just run and selectively read some codes. Please put the provided bochsrc file and the image into the directory of bochs and directly run bochs.exe and you will see the effect.
DIRECTORY:
-boot
Includes the bootloader
-boot.asm 
Load the loader into memory(The computer starts executing here).
-loader.asm 
Enter protected mode and init paging then load the kernel into memory
-inc 
Include files for constants
-Fat12 Header.inc
For FAT12 file system.
-PM.inc
Constants for protected mode.
-kalib
Library in assembly language for protected mode.
-kclib
Library in C for protected mode.
-kernel
Codes executed as OS enters kernel
-lib
Library in assembly language for real mode.
-temp
Some developing functions.
-HOS.img
The OS image.
-makefile
Make command for building the system.
**Compiled by GNU GCC and NASM.
