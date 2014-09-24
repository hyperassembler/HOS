org 0x01000000
;void HkSystemInit(PMULTIBOOT_INFO info)
GRUB_LOADED_FLAG equ 0x36d76289
GRUB_MAGIC_NUMBER equ 0xE85250D6
GRUB_ARCHITECTURE equ 0x0
GRUB_CHECK_SUM equ - (GRUB_MAGIC_NUMBER + GRUB_ARCHITECTURE + GRUB_HEADER_LENGTH)
GRUB_HEADER_LENGTH equ (GRUB_HEADER_END - GRUB_HEADER_START)
GRUB_HEADER_START:
dd GRUB_MAGIC_NUMBER
dd GRUB_ARCHITECTURE
dd GRUB_HEADER_LENGTH
dd GRUB_CHECK_SUM
;===================
;Address_tag
GRUB_LOAD_BASE equ 0x01000000 ;1MB
GRUB_ADDRESS_TAG_SIZE equ GRUB_ADDRESS_TAG_END - GRUB_ADDRESS_TAG_START
GRUB_ADDRESS_TAG_START:
dw 0x2 ;type=2
dw 0x0 ;flag=0
dd GRUB_ADDRESS_TAG_SIZE; size
dd GRUB_HEADER_START; Since at the beginning of the file
dd GRUB_ENTRY
dd 0 ;load end 
dd 0 ;bss
GRUB_ADDRESS_TAG_END:
;====================
;Entry_tag
GRUB_ENTRY_TAG_SIZE equ GRUB_ENTRY_TAG_END - GRUB_ENTRY_TAG_START
GRUB_ENTRY_TAG_START:
dw 0x3; type=3
dw 0x0; flag=0
dd GRUB_ENTRY_TAG_SIZE
dd GRUB_ENTRY
GRUB_ENTRY_TAG_END:
;====================
;End_tag
dw 0x0
dw 0x0
dd 8
;====================
GRUB_HEADER_END:

%include "pm.inc"

;GDT to load
GDT_DUMMY:
;GDT
DESC_VOID: Descriptor 0,0,0
DESC_GRAPH: Descriptor 0b8000h,0xffff,DA_DRW | DA_DPL3
DESC_FLAT_C: Descriptor 0,0xfffff,DA_CR | DA_32 | DA_LIMIT_4K
DESC_FLAT_RW: Descriptor 0,0xfffff,DA_DRW | DA_32 | DA_LIMIT_4K
GDT_END:

GDT_LENGTH equ GDT_END - GDT_DUMMY - 1
;GDT PTR
GDT_PTR:
dw GDT_LENGTH
dd GDT_DUMMY

;SELECTORS
SLCT_CODE_0 equ DESC_FLAT_C - DESC_VOID
SLCT_GRAPH_0 equ DESC_GRAPH - DESC_VOID
SLCT_DATA_0 equ DESC_FLAT_RW - DESC_VOID

DUMMY_MSG:
db 'Loaded by multiboot2!',0

;stack
times 1024 db 0
_KERNEL_STACK:

GRUB_ENTRY:
[BITS 32]
cli

cmp eax,GRUB_LOADED_FLAG
je LOADED_BY_GRUB
hlt
LOADED_BY_GRUB:

mov eax,_KERNEL_STACK
mov esp,eax

push dword 0
popfd

lgdt [GDT_PTR]
jmp SLCT_CODE_0:Reload_CS
Reload_CS:
mov ax,cs
mov ax,SLCT_DATA_0
mov ss,ax
mov ds,ax
mov es,ax
mov fs,ax
mov ax,SLCT_GRAPH_0
mov gs,ax

push DUMMY_MSG
call _printf
add esp,4

_printf:
;void printf(char* str)
;EAX,ECX,EDX
push ebp
mov ebp,esp
push edi
push esi

mov edi,dword [ss:ebp+8]
xor esi,esi
xor ecx,ecx

.begin:
mov al,byte [ds:edi]
cmp al,0 ;0 ended
je .end
mov byte [gs:esi],al
inc esi
mov byte [gs:esi],0xFF
inc edi
inc esi
jmp .begin
.end:
pop esi
pop edi
mov esp,ebp
pop ebp

