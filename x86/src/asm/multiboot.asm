global kernel_stack
extern hk_main
[SECTION .multiboot]
[BITS 32]
GRUB_LOADED_FLAG equ 0x2BADB002
GRUB_MAGIC_NUMBER equ 0x1BADB002
GRUB_FLAGS equ 0x10003
GRUB_CHECK_SUM equ -(GRUB_MAGIC_NUMBER + GRUB_FLAGS)
GRUB_HEADER_ADDR:
dd GRUB_MAGIC_NUMBER
dd GRUB_FLAGS
dd GRUB_CHECK_SUM

dd GRUB_HEADER_ADDR
dd GRUB_HEADER_ADDR
dd 0
dd 0
dd GRUB_ENTRY_ADDR

times 4096 db 0
kernel_stack:

GRUB_ENTRY_ADDR:
cli

cmp eax,GRUB_LOADED_FLAG
jmp LOADED_BY_GRUB
hlt
LOADED_BY_GRUB:

mov eax,kernel_stack
mov esp,eax

push dword 0
popfd

push ebx
;jmp since we are not coming back here :D
call hk_main
add esp,4