SLCT_GRAPH equ 8
SLCT_FLAT_C equ 16
SLCT_FLAT_RW equ 24
global __asm_KeMemoryCopy;PVOID _asm_KeMemoryCopy(PVOID src,PVOID dst,ULONG32 size)
__asm_KeMemoryCopy:
push ebp
mov ebp,esp
push esi
push edi
push ecx

mov ax,SLCT_FLAT_RW
mov es,ax
mov esi,dword [ss:ebp+8]
mov edi,dword [ss:ebp+12]
mov ecx,dword [ss:ebp+16]

.loop:
mov al,byte [es:esi]
mov byte [ds:edi],al
inc esi
inc edi
dec ecx
cmp ecx,0
je .end
jmp .loop
.end:
mov eax,dword [ss:ebp+8]

pop ecx
pop edi
pop esi
pop ebp
ret 