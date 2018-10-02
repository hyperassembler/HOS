# Building
### Environment
All platforms where the required packages are available.

### Required packages
For compiling kernel only (make compile):

nasm, clang, lld, llvm

To make bootable ISO (make all):

xorriso; grub-pc-bin for bios; grub-efi-amd64-bin, mtools for UEFI.

### Compiling
Run "make all" or "make compile" in the root directory.

This will generate secxkrnl.elf, secxkrnl.dmp, (and secxkrnl.iso) in "out" folder

Run "make clean" to clean a build.


# Running
Load the iso with your favorite simulator or use "-kernel" option with QEMU.

For UEFI simulation, use qemu_bios.bin in the root dir with QEMU.

