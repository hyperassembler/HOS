#define ASM_FILE
#include <arch/mlayout.h>
#include "multiboot2.h"
%define BOCHS_BREAK xchg bx, bx
%define GET_PADDR(x) ((x) - KERN_BASE_START)
%define GET_PML4(vaddr) (((vaddr) >> 39 ) & 0x1FF)
%define GET_PDPT(vaddr) (((vaddr) >> 30 ) & 0x1FF)
%define GET_PDE(vaddr) (((vaddr) >> 21 ) & 0x1FF)

; make arch_init_32 visiable
global arch_init_32

; arch_main is the C long mode entry point
extern arch_main

section .text
bits 32
; the bootloader calls this dude, we switch to long mode with some basic setup:
; Identity map the first 4G memory, where the kernel binary and multiboot info is
; Map the first 4G memory to KERN_PMAP temporarily so we have access to printf
; Map the first 1G memory, which contains the kernel, to KERN_BASE_START
; Map the nth PML4 to itself for recursive page tables
arch_init_32:
    cli ; close interrupt
    cld ; set direction
    cmp eax, MULTIBOOT2_BOOTLOADER_MAGIC ; compare with multiboot2 magic
    jne .end ; if not loaded by multiboot2 compliant bootloader, loop infinitely

    BOCHS_BREAK

    ; save multiboot info
    mov dword [GET_PADDR(multiboot_info_ptr)], ebx

    ; disable paging first
    mov eax, cr0          ; Set the A-register to control register 0.
    and eax, ~(1 << 31) & 0xFFFFFFFF   ; Clear the PG-bit, which is bit 31, and hack to get rid of warning
    mov cr0, eax          ; Set control register 0 to the A-register.

    ; identity map the first 4G page
    mov eax, GET_PADDR(kern_early_pml4)
    add eax, GET_PML4(0) * 8 ; eax = offset of pml4e in pml4 for the 0th GB
    mov dword [eax], GET_PADDR(kern_early_ident_pdpt) + 11b ; let the corresponding pml4e point to the kern_early_ident_pdpt

    mov eax, GET_PADDR(kern_early_ident_pdpt)
    add eax, GET_PDPT(0) * 8
    mov ebx, 10000011b ; ebx lower bits is attribute = R/W + SU + 1G page, high bits = physical 0th GB
    mov ecx, 4 ; 4 times = 4GB
    .l0:
        mov dword [eax], ebx ; set the corresponding pdpte to map 1GB pages in ebx
        add ebx, 1*1024*1024*1024 ; add another 1G to ebx higher bits
        add eax, 8 ; increment to next pdpte
        loop .l0

    ; map the first 4G to pmap
    mov eax, GET_PADDR(kern_early_pml4)
    add eax, GET_PML4(KERN_PMAP_START) * 8 ; eax = offset of pml4e in pml4 for PMAP region
    mov dword [eax], GET_PADDR(kern_early_pmap_pdpt) + 11b ; let the corresponding pml4e point to the kern_early_pmap_pdpt

    mov eax, GET_PADDR(kern_early_pmap_pdpt)
    add eax, GET_PDPT(KERN_PMAP_START) * 8
    mov ebx, 10000011b ; ebx lower bits is attribute = R/W + SU + 1G page, high bits = physical 0th GB
    mov ecx, 4 ; 4 times = 4GB
    .l1:
        mov dword [eax], ebx ; set the corresponding pdpte to map 1GB pages in ebx
        add ebx, 1*1024*1024*1024 ; add another 1G to ebx higher bits
        add eax, 8 ; increment to next pdpte
        loop .l1

    ; map the first 1G to kern_base
    ; point the first PML4 entry to the identity pdpt
    mov eax, GET_PADDR(kern_early_pml4)
    add eax, GET_PML4(KERN_BASE_START) * 8
    mov dword [eax], GET_PADDR(kern_early_img_pdpt) + 11b ; let the corresponding pml4e point to the kern_early_img_pdpt

    mov eax, GET_PADDR(kern_early_img_pdpt)
    add eax, GET_PDPT(KERN_BASE_START) * 8
    mov dword [eax], 10000011b ; ebx lower bits is attribute = R/W + SU + 1G page, high bits = physical 0th GB

    ; map the recursive mapping
    mov eax, GET_PADDR(kern_early_pml4)
    add eax, GET_PML4(KERN_RPT_START) * 8
    mov dword [eax], GET_PADDR(kern_early_pml4) + 11b

    BOCHS_BREAK

    ; enable PAE
    mov eax, cr4                 ; Set the A-register to control register 4.
    or eax, 1 << 5               ; Set the PAE-bit, which is the 6th bit (bit 5).
    mov cr4, eax                 ; Set control register 4 to the A-register.

    ; enable long mode
    mov ecx, 0xC0000080          ; Set the C-register to 0xC0000080, which is the EFER MSR.
    rdmsr                        ; Read from the model-specific register.
    or eax, 1 << 8               ; Set the LM-bit which is the 9th bit (bit 8).
    wrmsr                        ; Write to the model-specific register.

    ; let cr3 point at page table
    mov eax, GET_PADDR(kern_early_pml4)
    mov cr3, eax

    xchg bx, bx
    ; enable paging, enter compatibility mode
    mov eax, cr0                                   ; Set the A-register to control register 0.
    or eax, 1 << 31                                ; Set the PG-bit, which is bit 31.
    mov cr0, eax                                   ; Set control register 0 to the A-register.

    ; now we are in compat mode
    ; load the long mode GDT
    lgdt [GET_PADDR(kern_early_gdt.ptr)]

	; switch to long mode
    jmp kern_early_gdt.code:GET_PADDR(arch_init_64)
    ; should not reach this point
.end:
    jmp $

section .data
bits 32
multiboot_info_ptr: 
    dd 0

section .text
bits 64
arch_init_64:
	; note that we are in long mode but rip is still lower
	; switch to high address
	mov rax, .high
	jmp rax
.high:
    ; set proper segment registers
    mov ax,kern_early_gdt.data
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ss,ax

    ; initial kernel stack, 4k
    mov rsp, kern_early_stack
    xor rdi, rdi
    mov edi, dword [multiboot_info_ptr]
    ; init arch
    call arch_main
    ; should not reach this point
    jmp $


section .data
bits 64
align 0x1000
    times KERN_PAGE_SZ db 0
kern_early_stack:

align 0x1000
kern_early_pml4:
    times 0x1000 db 0

align 0x1000
kern_early_ident_pdpt:
    times 0x1000 db 0

align 0x1000
kern_early_pmap_pdpt:
    times 0x1000 db 0

align 0x1000
kern_early_img_pdpt:
    times 0x1000 db 0

kern_early_gdt:                        ; Global Descriptor Table (long mode).
.null: equ $ - kern_early_gdt         ; The null descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0                         ; Access.
    db 0                         ; Granularity.
    db 0                         ; Base (high).
.code: equ $ - kern_early_gdt    ; The code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10011010b                 ; Access (exec/read).
    db 00100000b                 ; Granularity.
    db 0                         ; Base (high).
.data: equ $ - kern_early_gdt    ; The data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10010010b                 ; Access (read/write).
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
.ptr:
    ; GDT PTR
    dw $ - kern_early_gdt - 1       ; Limit.
    dq GET_PADDR(kern_early_gdt)    ; Base.
