org 0x01000000

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

;Message
DUMMY_MSG:
db 'Loaded by multiboot1!',0

;stack
times 1024 db 0
_KERNEL_STACK:

GRUB_ENTRY_ADDR:
[BITS 32]
cli

cmp eax,GRUB_LOADED_FLAG
jmp LOADED_BY_GRUB
hlt
LOADED_BY_GRUB:

mov eax,_KERNEL_STACK
mov esp,eax

push dword 0
popfd

lgdt [GDT_PTR]
jmp SLCT_CODE_0:Reload_CS
Reload_CS:
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

end:
jmp end

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
mov byte [gs:esi],0x0c
inc edi
inc esi
jmp .begin
.end:
pop esi
pop edi
mov esp,ebp
pop ebp

