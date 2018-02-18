# Building
### Environment
Linux distribution / Bash for Windows.

### Required packages

NASM (sudo apt-get install nasm)

GCC cross compiler (see the next section)

### GCC cross compiler
##### Method 1 - Compile from GCC source.

Follow the instructions from https://wiki.osdev.org/GCC_Cross-Compiler

Set the "TARGET" to "x86_64-elf" instead of "i686-elf" and build libgcc without redzone https://wiki.osdev.org/Libgcc_without_red_zone.

##### Method 2 - Precompiled binaries

VERSION: GCC 7.3 + Binutils 2.30

HOST: x86_64-pc-linux-gnu

CONFIGURED WITH: --target=x86_64-elf --disable-nls --enable-languages=c,c++ --without-headers

1. Download "cross.tar.gz" from the Github release page.

2. Run "tar -xvf cross.tar.gz -C $HOME/opt/cross"

### Compiling
Run "make" in the root directory.

This will generate secxkrnl.elf (kernel executable) and secxkrnl.dmp (kernel dump).

Run "make clean" to clean a build.

# Running

