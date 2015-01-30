This is a really simple operating system(still being developed). The image file is HOS.iso. Please use bochs to emulate

- Uses grub2 and is multiboot-complaint.
- Architecture i386.
- Supports x86, x86_64.
- Calling convention: x86 cdecl, x64 System V ABI.
- Complete refactoring of previous version.
- Compiled by gcc, nasm. Linked by ld.