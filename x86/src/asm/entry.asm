global kernel_stack
global kernel_addr
global BOCHS_MAGIC_BREAKPOINT
global HLT_CPU
global init_x64
extern kmain
extern enable_paging
extern disable_paging

[SECTION .entry]
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
dd entry_32

times 4096 db 0
kernel_stack:

entry_32:
cli

cmp eax,GRUB_MAGIC
jmp entry_32.loaded_by_grub
hlt
.loaded_by_grub:

mov eax,kernel_stack
mov esp,eax

push dword 0
popfd

push ebx
call kmain
add esp,4 ; We are actually not coming back here. But out of courtesy...
hlt

BOCHS_MAGIC_BREAKPOINT:
xchg bx,bx
ret

HLT_CPU:
hlt

;multiboot_info on stack
init_x64:
push ebp
mov ebp,esp
cli
mov edi,[ss:ebp+8] ;System V ABI
jmp 0x100000 ;hard-coded
mov esp,ebp
pop ebp
ret

