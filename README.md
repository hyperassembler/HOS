My Hobby x86-64 OS kernel.

# Building
### Environment
Any Linux distribution / Bash for Windows + GCC cross compiler.

### Required packages
To compile the kernel, the following modules are required on linux system, please install these:

nasm, xorriso, grub-common, grub-pc-bin

### Cross-compiler
##### Method 1 - Compile one yourself.
Just follow the instructions here: https://wiki.osdev.org/GCC_Cross-Compiler

Remember to set the "TARGET" to x86_64-elf instead of i686-elf and build libgcc without redzone https://wiki.osdev.org/Libgcc_without_red_zone.

##### Method 2 - Precompiled binaries
Here is my precompiled cross compiler. Feel free to use it.

VERSION: GCC 7.3 + Binutils 2.30

HOST: x86_64-pc-linux-gnu

CONFIGURED WITH: --target=x86_64-elf --disable-nls --enable-languages=c,c++ --without-headers


1. Download "cross.tar.gz" from https://drive.google.com/file/d/1ssQblBLk7I3ZqFPoeViVlsBvDMXcJFa1/view?usp=sharing


2. Run "tar -xvf cross.tar.gz -C $HOME/opt/cross"

### Compiling
Run "make all" in the root directory

The makefile generates secxkrnl.iso (kernel image), secxkrnl.elf(kernel executable), secxkrnl.dmp(kernel disassembly) at the end.

To clean all generated files, run "make clean"

# Running
On windows run "qemu.bat"

On linux run "./qemu.bat"
