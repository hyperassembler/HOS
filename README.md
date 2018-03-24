# Building
### Environment
All platforms where the required packages are available.

### Required packages
NASM, Clang (6.0+), make

### Compiling
Run "make" in the root directory.

This will generate secxkrnl.elf (kernel executable) and secxkrnl.dmp (kernel dump).

Run "make clean" to clean a build.

# Running
secX requires bootloader [secboot](https://github.com/secXsQuared/secboot).

See secboot repository for more information.
