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


%include "pm.inc"

;GDT to load
DESC_VOID: Descriptor 0,0,0
DESC_GRAPH: Descriptor 0b8000h,0xffff,DA_DRW | DA_DPL3
DESC_FLAT_C: Descriptor 0,0xfffff,DA_CR | DA_32 | DA_LIMIT_4K
DESC_FLAT_RW: Descriptor 0,0xfffff,DA_DRW | DA_32 | DA_LIMIT_4K
GDT_END:

GDT_LENGTH equ GDT_END - DESC_VOID - 1
GDT_PTR:
dw GDT_LENGTH
dd DESC_VOID

;SELECTORS
SLCT_CODE_0 equ DESC_FLAT_C - DESC_VOID
SLCT_GRAPH_0 equ DESC_GRAPH - DESC_VOID
SLCT_DATA_0 equ DESC_FLAT_RW - DESC_VOID

;stack
times 1024 db 0
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

lgdt [GDT_PTR]
jmp SLCT_CODE_0:reload_cs
reload_cs:
mov ax,SLCT_DATA_0
mov ss,ax
mov ds,ax
mov es,ax
mov fs,ax
mov ax,SLCT_GRAPH_0
mov gs,ax

push ebx
call hk_main
add esp,4