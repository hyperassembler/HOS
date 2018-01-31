extern _KeSystemInit
extern _KeGDTPtr
extern _KeSwitchGDT
extern _KeIDTPtr
global _start
;GDT
SLCT_GRAPH equ 8
SLCT_FLAT_C equ 16
SLCT_FLAT_RW equ 24
;Stack
[section .bss]
[BITS 32]
resb 2 * 1024
KernelStackTop:
[section .text]
[BITS 32]
_start:
mov esp,KernelStackTop

sgdt [_KeGDTPtr]
call _KeSwitchGDT
lgdt [_KeGDTPtr]
jmp SLCT_FLAT_C:GDT_COMPLETE
GDT_COMPLETE:

mov ax,SLCT_GRAPH
mov gs,ax
mov ax,SLCT_FLAT_RW
mov ds,ax
mov ss,ax

push 0
popfd


call _KeSystemInit
