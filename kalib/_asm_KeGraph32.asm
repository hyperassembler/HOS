;===================================================================32 BITS=============================================================

global __asm_KePrintHex32 ;VOID __asm_KePrintHex32(ULONG32 Hex)
global __asm_KePrintStr32 ;VOID __asm_KePrintStr32(PCHAR *OffsetStr)
global __asm_KeSetGraphPosition32; ULONG32 __asm_KeSetGraphPosition32(ULONG32 Row,ULONG32 Column)
global __asm_KeClearGraph32;VOID __asm_KeClearGraph32()
global __asm_KeGraphRowOverflow;ULONG32 _asm_KeGraphRowOverflow32(ULONG32 GraphPosition)
global __asm_KeGetCurrentGraphPosition32; ULONG32 _asm_KeGetCurrentGraphPosition32(VOID);

extern _KeCheckGraphRowForPrint
extern _KeCheckGraphRowForReturn

SLCT_GRAPH equ 8
SLCT_FLAT_C equ 16
SLCT_FLAT_RW equ 24

KeGraphPosition_32 dd 0

DOSGRAPHROW equ 25
DOSGRAPHCOLUMN equ 80

__asm_KeGetCurrentGraphPosition32:
mov eax, dword [ds:KeGraphPosition_32]
ret

__asm_KeSetGraphPosition32:
push ebp
mov ebp,esp
push esi
push edi
push ebx
push edx

mov eax,dword [ss:ebp+8]
mov ebx,dword [ss:ebp+12]
cmp eax,DOSGRAPHROW-1
ja .fault
cmp ebx,DOSGRAPHCOLUMN-1
ja .fault

mov dl,2*DOSGRAPHCOLUMN
mul dl

mov esi,eax

xor eax,eax

mov al,bl
mov dl,2
mul dl

add eax,esi

mov dword [ds:KeGraphPosition_32],eax
jmp .return
.fault:
mov dword [ds:KeGraphPosition_32],0
xor eax,eax
.return:
pop edx
pop ebx
pop edi
pop esi
mov esp,ebp
pop ebp
ret

__asm_KeClearGraph32:
push ecx
push esi
mov ecx,2*DOSGRAPHROW*DOSGRAPHCOLUMN-1
xor esi,esi
.loop:
mov byte [gs:esi],0
dec ecx
inc esi
cmp ecx,0
jne .loop
mov dword [KeGraphPosition_32],0
pop esi
pop ecx
ret 



__asm_KePrintHex32:
;Entrance: push HEX
push ebp
mov ebp,esp
push eax

mov eax,dword [ss:ebp+8]
shr eax,24
mov ah,0Fh
push ax
call near DISPINTAL32
pop ax

mov eax,dword [ss:ebp+8]
shr eax,16
mov ah,0Fh
push ax
call near DISPINTAL32
pop ax

mov eax,dword [ss:ebp+8]
shr eax,8
mov ah,0Fh
push ax
call near DISPINTAL32
pop ax

mov eax,dword [ss:ebp+8]
mov ah,0Fh
push ax
call near DISPINTAL32
pop ax

pop eax
mov esp,ebp
pop ebp
ret

__asm_KePrintStr32:
;Entrance: push offset
;ds:offset = String
push ebp
mov ebp,esp
push esi
push edi
push eax
push ebx
push gs

mov ax,SLCT_GRAPH
mov gs,ax
mov esi,dword [ss:ebp+8]
mov edi,dword [ds:KeGraphPosition_32]
.loop1:
mov al,byte [ds:esi]
cmp al,0
je .end
cmp al,0Ah
jne .loop2
;Check For OverFlow
push eax
push edi
call _KeCheckGraphRowForReturn
add esp,4
mov edi,eax
pop eax
;let edi point to the next row
;edi/(80*2) - every row:80 chars, each char takes 2 bytes.
push eax
mov eax,edi
mov bl,2*DOSGRAPHCOLUMN
div bl
;only keep the low 8 of eax(al,aka quotion)
and eax, 0FFh
inc eax
;calculate the edi now
mov bl,2*DOSGRAPHCOLUMN
mul bl
mov edi,eax
pop eax
inc esi
jmp .loop1
.loop2:
;Check For OverFlow
push eax

push edi
call _KeCheckGraphRowForPrint
add esp,4

mov edi,eax
pop eax
;======
mov ah,0Fh
mov word [gs:edi],ax
add edi,2
inc esi
jmp .loop1
.end:
mov dword [ds:KeGraphPosition_32],edi

pop gs
pop ebx
pop eax
pop edi
pop esi
mov esp,ebp
pop ebp
ret 

DISPINTAL32:
;Entrance: push ax ,AH=ATTR, AL=Char
push ebp
mov ebp,esp
push eax
push gs
push ecx
push edi
push ebx

mov edi,dword [ds:KeGraphPosition_32]
mov ax,SLCT_GRAPH
mov gs,ax
xor eax,eax
mov ax, word [ss:ebp+8]
mov bx,ax
;keep high 4 bits of al
and al,0F0H
mov cl,4
shr al,cl
call near CHANGEAL
;Check For OverFlow
push eax

push edi
call _KeCheckGraphRowForPrint
add esp,4

mov edi,eax
pop eax
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
mov word [gs:edi],ax
add edi,2
mov dword [ds:KeGraphPosition_32],edi
;keep low 4 bits of al
mov ax,bx
and al,0Fh
call near CHANGEAL
mov ah,bh
mov word [gs:edi],ax
add edi,2
mov dword [ds:KeGraphPosition_32],edi

pop ebx
pop edi
pop ecx
pop gs
pop eax
pop ebp
ret

CHANGEAL:
;Entrance:void
cmp al,09h
jna .l2
add al,37h
jmp .end
.l2:
add al,30h
.end:
ret

__asm_KeGraphRowOverflow:
push ebp
mov ebp,esp
push esi
push edi
push edx
push ecx
push ebx

mov eax,[ss:ebp+8]
xor esi,esi
xor edi,edi

mov esi,2*DOSGRAPHCOLUMN

mov ebx,DOSGRAPHROW

.loop1:
mov ecx,2*DOSGRAPHCOLUMN-1
.loop2:
mov dl,byte [gs:esi]
mov byte [gs:edi],dl
inc edi
inc esi
dec ecx
cmp ecx,0
jne .loop2

dec ebx
cmp ebx,0
jne .loop1

push 0
push 24
call __asm_KeSetGraphPosition32
add esp,8

mov ecx,2*DOSGRAPHCOLUMN
.loop3:
mov byte [gs:eax],0
inc eax
dec ecx
cmp ecx,0
jne .loop3


push 0
push 24
call __asm_KeSetGraphPosition32
add esp,8

pop ebx
pop ecx
pop edx
pop edi
pop esi
mov esp,ebp
pop ebp
ret


