My Hobby x86-64 OS kernel.

# Building
### Environment
Any Linux distribution / Bash for Windows + GCC cross compiler.

### Required packages
Please use "sudo apt-get install" to install these:

nasm, xorriso, binutils-mingw-w64, gcc-mingw-w64, mtools

### Cross-compiler
##### Method 1 - Compile yourself.
Just follow the instructions here: https://wiki.osdev.org/GCC_Cross-Compiler

Remember to set the "TARGET" to x86_64-elf instead of i686-elf and build libgcc without redzone https://wiki.osdev.org/Libgcc_without_red_zone.

##### Method 2 - Precompiled binaries
VERSION: GCC 7.3 + Binutils 2.30

HOST: x86_64-pc-linux-gnu

CONFIGURED WITH: --target=x86_64-elf --disable-nls --enable-languages=c,c++ --without-headers


1. Download "cross.tar.gz" from the Github release page.


2. Run "tar -xvf cross.tar.gz -C $HOME/opt/cross"

### Compiling
Run "make all" in the root directory

The makefile generates secxkrnl.iso (kernel image), secxkrnl.elf(kernel executable), secxkrnl.dmp(kernel disassembly) at the end.

To clean all generated files, run "make clean"
