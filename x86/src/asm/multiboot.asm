global kernel_stack
global kernel_addr
global hk_comp
extern hk_main
extern hk_print_str
extern hk_print_hex
extern hk_print_int
extern hk_enable_paging
extern hk_disable_paging
extern g_gdt_ptr_64

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

[SECTION .text]
[BITS 32]
hk_entry_comp:
; no, no interrupt please.
cli

; disable paging first
call hk_disable_paging

xchg bx,bx ; pure magic

; enable PAE
mov eax, cr4                 ; Set the A-register to control register 4.
or eax, 1 << 5               ; Set the PAE-bit, which is the 6th bit (bit 5).
mov cr4, eax                 ; Set control register 4 to the A-register.

; enable x86_64
mov ecx, 0xC0000080          ; Set the C-register to 0xC0000080, which is the EFER MSR.
rdmsr                        ; Read from the model-specific register.
or eax, 1 << 8               ; Set the LM-bit which is the 9th bit (bit 8).
wrmsr                        ; Write to the model-specific register.

; let cr3 point at l
mov eax,0x100000
mov cr3,eax

; enable paging, enter compatibility mode
call hk_enable_paging

; enter x64
lgdt [g_gdt_ptr_64]
jmp 8:hk_entry_x64


[SECTION .text]
[BITS 64]
hk_entry_x64:
cli
mov ax,24
mov ds,ax
mov es,ax
mov fs,ax
mov gs,ax
mov ss,ax
mov rax, 0x1F201F201F201F20
mov ecx, 500
rep movsq
hlt