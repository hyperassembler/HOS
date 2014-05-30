org 0100h
jmp BEGIN
%include "pm.inc"
;GDT
DESC_VOID: Descriptor 0,0,0
DESC_GRAPH: Descriptor 0b8000h,0ffffh,DA_DRW | DA_DPL3
DESC_FLAT_C: Descriptor 0,0fffffh,DA_CR | DA_32 | DA_LIMIT_4K
DESC_FLAT_RW: Descriptor 0,0fffffh,DA_DRW | DA_32 | DA_LIMIT_4K
DESC_FLAT_USER_C: Descriptor 0,0fffffh,DA_CR | DA_32| DA_LIMIT_4K | DA_DPL3
DESC_FLAT_USER_RW: Descriptor 0,0fffffh,DA_DRW | DA_32 | DA_LIMIT_4K | DA_DPL3

GDTLEN equ $ - DESC_VOID

GDTPTR: dw GDTLEN - 1
dd LOADERINFLAT + DESC_VOID

;Paging
PDEBASE equ 100000h
PTEBASE equ 101000h

;kernel
LOADERBASE equ 09000h
LOADEROFFSET equ 0100h
LOADERINFLAT equ LOADERBASE*10h

KERNELFILENAME db 'KERNEL  BIN',0
KERNELBASE equ 03000h
KERNELOFFSET equ 0000h
KERNELINFLAT equ KERNELBASE*10h +  KERNELOFFSET

;Selectors
SLCT_FLAT_C equ DESC_FLAT_C - DESC_VOID
SLCT_GRAPH equ DESC_GRAPH - DESC_VOID
SLCT_FLAT_RW equ DESC_FLAT_RW - DESC_VOID

;==================Section .DATA==============================
;Variables
;Operating System Information
;OS:
OSINFO1: db 'Hyper-Assembler Operating System 1.0',0
OSINFO2: db "Loader written by Orange'S, Kernel written by Hyper-Assembler",0
KERNELFILELOADCOMPLETE db 'Kernel.bin Loaded to 0x30000',0

;GETMEMORY
RAMINFO1: db "Memory Information:",0
RAMTABLE16: db ' BASE L   BASE H  Length L Length H   Type  ',0
RAMSIZE: db 'RAM SIZE:',0
GETMEMFAIL: db 'Failed to Get The Memory',0
RAMAVAILABLE: db ' - Available',0
RAMRESERVED: db ' - Reserved',0

;Protected Mode:
PMINFO: db 'Protected Mode Entered...',0
PMINFO32 equ PMINFO + LOADERINFLAT

;PAGING:
PGINFO: db 'Paging Started...',0
PGINFO32 equ PGINFO + LOADERINFLAT
PTEINFO: db 'PTE Table BASE: ',0
PTEINFO32 equ PTEINFO + LOADERINFLAT
PDEINFO: db 'PDE Table BASE: ',0
PDEINFO32 equ PDEINFO + LOADERINFLAT
CR3INFO: db 'cr3: ',0
CR3INFO32 equ CR3INFO + LOADERINFLAT

;Variables Used in Functions
IO16_SPACE: db ' ',0
IO32_SPACE equ IO16_SPACE + LOADERINFLAT
IO16_RETURN: db 0Ah,0
IO32_RETURN equ IO16_RETURN + LOADERINFLAT
GRAPHPOS: dd 0
GRAPHPOS_32 equ GRAPHPOS + LOADERINFLAT
GETMEMBUFF: times 1024 db 0
GETMEMNUM: dd 0
GETMEMNUM_32 equ GETMEMBUFF + LOADERINFLAT
DISPMEMTOTAL: dd 0
DISPMEMTOTAL_32 equ DISPMEMTOTAL + LOADERINFLAT
KERNELLOADED: db 'KERNEL LOADED IN MEM.',0
KERNELLOADED_32 equ KERNELLOADED + LOADERINFLAT
%include "FAT12Header.inc"
;=========================STACK32=============================
[bits 32]
STACK32BASE:
times 1024 db 0
TOP:
STACK32TOP equ LOADERINFLAT + TOP

;==========================CODE16==============================
[bits 16]
%include "io16.lib"
%include "fat12readfile.lib"
BEGIN:
mov ax,cs
mov ds,ax
mov ss,ax
mov es,ax

mov ax,0003h
int 10h ;clear

xor eax,eax
mov eax,OSINFO1
push eax
call DISPSTR16
add esp,4

call DISPRETURN16

xor eax,eax
mov eax,OSINFO2
push eax
call DISPSTR16
add esp,4

call DISPRETURN16

call DISPRETURN16

xor eax,eax
mov eax,RAMINFO1
push eax
call DISPSTR16
add esp,4

call DISPRETURN16

call GETMEM16

mov eax,dword [ds:GETMEMNUM]
cmp eax,0
jne GETMEMSUCCESSFUL
xor eax,eax
mov eax,GETMEMFAIL
push eax
call DISPSTR16
pop eax

GETMEMSUCCESSFUL:

call DISPMEM16
call DISPRETURN16

;FAT12ReadFile(ULONG16 Base,ULONG16 Offset,ULONG16 OffsetFileName)
push KERNELFILENAME
push KERNELOFFSET
push KERNELBASE
call FAT12ReadFile
add esp,6

xor eax,eax
mov eax,KERNELFILELOADCOMPLETE
push eax
call DISPSTR16
add esp,4

call DISPRETURN16

;load gdt
lgdt [GDTPTR]

;close interupt
cli

;open A20
in al,92h
or al,00000010b
out 92h,al

;prepare to switch to the protected mode
mov eax,cr0
or eax,1
mov cr0,eax

jmp dword SLCT_FLAT_C:(CODE32_START + LOADERINFLAT)



;===================CODE32======================
[bits 32]
%include "mem32.lib"
%include "io32.lib"
CODE32_START:
mov ax,SLCT_FLAT_RW
mov ss,ax
mov ds,ax
mov es,ax
mov gs,ax

mov ax,SLCT_GRAPH
mov gs,ax

mov sp,STACK32TOP

mov eax,PMINFO32
push eax
call near DISPSTR32
pop eax


;Setup paging
call PAGINGINIT32

call DISPRETURN32

mov eax,PGINFO32
push eax
call near DISPSTR32
pop eax

call DISPRETURN32
;Print PDE Table Base
mov eax,PDEINFO32
push eax
call DISPSTR32
pop eax

mov eax,PDEBASE
push eax
call DISPINTEAX32
pop eax

call DISPRETURN32
;Print PTE table base
mov eax,PTEINFO32
push eax
call DISPSTR32
pop eax

mov eax,PTEBASE
push eax
call DISPINTEAX32
pop eax

call DISPRETURN32
;Print cr3
mov eax,CR3INFO32
push eax
call DISPSTR32
pop eax

mov eax,cr3
push eax
call DISPINTEAX32
pop eax

;Jump to The Kernel

jmp SLCT_FLAT_C:KERNELINFLAT
