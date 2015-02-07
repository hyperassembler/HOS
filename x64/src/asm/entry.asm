extern hk_main
global HLT_CPU
global BOCHS_MAGIC_BREAKPOINT

;IMPORTANT: Before entering this, CPU should be in protected mode.
;IMPORTANT: This module should be 4k-page aliened
[SECTION .entry]
[BITS 32]
;on stack: multiboot_info*
;skip data definition
jmp start
; here we need to construct a dummy gdt as well as a dummy page table(As simple as possible, maps 1G page sounds good)
; for page table we only need 4 gigs since that's the maximum mem one can access in protected mode(without PAE)
; flags are hard-coded... highly not recommended but for our purpose it's enough
; little-endian assumed
times (4096 - 5) db 0; skip the first jmp as well as making it kernel stack, 16 bytes aliened :D
KERNEL_STACK:
PML4_BASE:
times 512 dq 0 ;reserved the rest for page entries
PDPT_BASE:
times 512 dq 0 ;reserved the rest for page entries


GDT64:                           ; Global Descriptor Table (64-bit).
    .NULL: equ $ - GDT64         ; The null descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0                         ; Access.
    db 0                         ; Granularity.
    db 0                         ; Base (high).
    .CODE: equ $ - GDT64         ; The code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10011000b                 ; Access.
    db 00100000b                 ; Granularity.
    db 0                         ; Base (high).
    .DATA: equ $ - GDT64         ; The data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10010000b                 ; Access.
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
    .GDT64_PTR:                  ; The GDT-pointer.
    dw $ - GDT64 - 1             ; Limit.
    dq GDT64                     ; Base.

start:
cli
; disable paging first
mov eax, cr0                                   ; Set the A-register to control register 0.
and eax, 01111111111111111111111111111111b     ; Clear the PG-bit, which is bit 31.
mov cr0, eax                                   ; Set control register 0 to the A-register.

;pure magic
xchg bx,bx

; write values for pml4
mov eax,PML4_BASE
mov dword [eax], PDPT_BASE + 3

; write values for pdpt
xor ecx, ecx
add ecx, 131

mov eax, PDPT_BASE
mov dword [eax], ecx

add eax,8
add ecx,0x40000000 ;1G
mov dword [eax], ecx

add eax,8
add ecx,0x40000000 ;1G
mov dword [eax], ecx

add eax,8
add ecx,0x40000000 ;1G
mov dword [eax], ecx

; enable PAE
mov eax, cr4                 ; Set the A-register to control register 4.
or eax, 1 << 5               ; Set the PAE-bit, which is the 6th bit (bit 5).
mov cr4, eax                 ; Set control register 4 to the A-register.

; enable x86_64
mov ecx, 0xC0000080          ; Set the C-register to 0xC0000080, which is the EFER MSR.
rdmsr                        ; Read from the model-specific register.
or eax, 1 << 8               ; Set the LM-bit which is the 9th bit (bit 8).
wrmsr                        ; Write to the model-specific register.

; let cr3 point at page table
mov eax, PML4_BASE
mov cr3,eax

; enable paging, enter compatibility mode
mov eax, cr0                                   ; Set the A-register to control register 0.
or eax, 1 << 31                                ; Set the PG-bit, which is bit 31.
mov cr0, eax                                   ; Set control register 0 to the A-register.

; enter x64
;lgdt [GDT64.GDT64_PTR]
jmp GDT64.CODE:entry

[SECTION .text]
[BITS 64]
entry:
cli
mov ax,GDT64.DATA
mov ds,ax
mov es,ax
mov fs,ax
mov gs,ax
mov ss,ax

; align 16 bytes like this for now
mov rsp,KERNEL_STACK
call hk_main
hlt

HLT_CPU:
hlt

BOCHS_MAGIC_BREAKPOINT:
xchg bx,bx
ret