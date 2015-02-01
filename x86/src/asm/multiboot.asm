global kernel_stack
extern hk_main
global kernel_addr
[SECTION .multiboot]
[BITS 32]
GRUB_MAGIC equ 0x2BADB002
MULTIBOOT_MAGIC_NUMBER equ 0x1BADB002
MULTIBOOT_FLAGS equ 0x10003
MULTIBOOT_CHECK_SUM equ -(MULTIBOOT_MAGIC_NUMBER + MULTIBOOT_FLAGS)
MULTIBOOT_HEADER:
dd MULTIBOOT_MAGIC_NUMBER
dd MULTIBOOT_FLAGS
dd MULTIBOOT_CHECK_SUM

dd MULTIBOOT_HEADER
dd MULTIBOOT_HEADER
dd 0
dd 0
dd hk_grub_main

times 4096 db 0
kernel_stack:

hk_grub_main:
cli

cmp eax,GRUB_MAGIC
jmp hk_grub_main.loaded_by_grub
hlt
.loaded_by_grub:

mov eax,kernel_stack
mov esp,eax

push dword 0
popfd

push ebx
call hk_main
add esp,4 ; We are actually not coming back here. But out of courtesy...