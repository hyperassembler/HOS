global hk_load_gdt
SELECTOR_DATA_0 equ 3*8 + 0
SELECTOR_DATA_3 equ 4*8 + 3
SELECTOR_CODE_0 equ 1*8 + 0
SELECTOR_CODE_3 equ 2*8 + 3
[SECTION .text]
[BITS 32]
;void hk_load_gdt(gdt_ptr* ptr)
hk_load_gdt:
push ebp
mov ebp,esp
push eax
mov eax,[ss:ebp+8]
lgdt [eax]
;reload cs
jmp SELECTOR_CODE_0:.reload
.reload
mov eax,SELECTOR_DATA_0
mov ss,ax
mov es,ax
mov fs,ax
mov gs,ax
mov ds,ax
pop eax
mov ebp,esp
pop ebp
ret
