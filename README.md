# Building
### Environment
Linux distribution / Bash for Windows.

### Required packages
NASM (sudo apt-get install nasm)
Clang 6.0+ (see the next section)

### Compiling
Run "make" in the root directory.

This will generate secxkrnl.elf (kernel executable) and secxkrnl.dmp (kernel dump).

Run "make clean" to clean a build.

# Running
secX requires bootloader [secboot](https://github.com/secXsQuared/secboot).

See secboot repository for more information.
