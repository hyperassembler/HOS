#define ASM_FILE
#include <arch/mlayout.h>
#include "multiboot2.h"

%define GET_PADDR(x) ((x) - KERNEL_IMG_VADDR)
%define GET_PML4(vaddr) (((vaddr) >> 39 ) & 0x1FF)
%define GET_PDPT(vaddr) (((vaddr) >> 30 ) & 0x1FF)

global arch_init_32
extern arch_main

section .text
bits 32
arch_init_32:
    cli
    cld
    cmp eax, MULTIBOOT2_BOOTLOADER_MAGIC
    jne .end

    ; save multiboot info
    mov dword [GET_PADDR(multiboot_info_ptr)], ebx

    ; setup stack
    call check_long_mode ; check support for long mode 
    cmp eax, 1
    jne .end

    ; disable paging first
    mov eax, cr0          ; Set the A-register to control register 0.
    and eax, ~(1 << 31) & 0xFFFFFFFF   ; Clear the PG-bit, which is bit 31, and hack to get rid of warning
    mov cr0, eax          ; Set control register 0 to the A-register.

    ; point the first PML4 entry to the identity pdpt
    mov eax, GET_PADDR(init_pml4)
    mov dword [eax], GET_PADDR(init_pdpt_iden) + 11b ; write the lower bits, higher = 0
    ; point the nth PML4 entry to the kernel pdpt
    add eax, GET_PML4(KERNEL_SPACE_VADDR) * 8
    mov dword [eax], GET_PADDR(init_pdpt_kern) + 11b

    ; identity map the first 4GB
    mov eax, GET_PADDR(init_pdpt_iden)
    mov ebx, 10000011b ; R/W + SU + 1G page
    mov ecx, 4 ; loop 4 times
.l0:
    mov dword [eax], ebx
    add ebx, 1*1024*1024*1024 ; 1G
    add eax, 8
    loop .l0

    ; map the first 1 GB, which contains the kernel, to KERNEL_BASE_VADDR
    mov eax, GET_PADDR(init_pdpt_kern)
    ; extract the PML4 entry
    add eax, GET_PDPT(KERNEL_SPACE_VADDR) * 8
    mov ebx, 10000011b ; R/W + SU + 1G page
    mov dword [eax], ebx

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
    mov eax, GET_PADDR(init_pml4)
    mov cr3, eax

    ; enable paging, enter compatibility mode
    mov eax, cr0                                   ; Set the A-register to control register 0.
    or eax, 1 << 31                                ; Set the PG-bit, which is bit 31.
    mov cr0, eax                                   ; Set control register 0 to the A-register.

    ; now we are in compat mode

    ; load the long mode GDT
    lgdt [GET_PADDR(init_gdt.ptr)]

	; switch to long mode
    jmp init_gdt.code:GET_PADDR(arch_init_64)
.end:
    jmp $

check_long_mode:
    push ebp
    mov ebp,esp
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    xor eax, ecx
    jz .not_supported
    mov eax, 0x80000000    ; Set the A-register to 0x80000000.
    cpuid                  ; CPU identification.
    cmp eax, 0x80000001    ; Compare the A-register with 0x80000001.
    jb .not_supported      ; It is less, there is no long mode.
    mov eax, 0x80000001    ; Set the A-register to 0x80000001.
    cpuid                  ; CPU identification.
    test edx, 1 << 29      ; Test if the LM-bit, which is bit 29, is set in the D-register.
    jz .not_supported      ; They arent, there is no long mode.
    mov eax,1
    jmp .end
.not_supported:
    xor eax,eax
.end:
    mov esp,ebp
    pop ebp
    ret

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
    mov ax,init_gdt.data
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ss,ax

    ; initial kernel stack, 4k
    mov rsp, init_stack
    xor rdi, rdi
    mov edi, dword [multiboot_info_ptr]
    ; init arch
    call arch_main
.end:
    hlt


section .data
bits 64
align KERNEL_PAGE_SIZE
    times KERNEL_PAGE_SIZE db 0
init_stack:

init_pml4:
align KERNEL_PAGE_SIZE
    times KERNEL_PAGE_SIZE db 0

init_pdpt_iden:
align KERNEL_PAGE_SIZE
    times KERNEL_PAGE_SIZE db 0

init_pdpt_kern:
align KERNEL_PAGE_SIZE
    times KERNEL_PAGE_SIZE db 0

init_gdt:                        ; Global Descriptor Table (long mode).
.null: equ $ - init_gdt         ; The null descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0                         ; Access.
    db 0                         ; Granularity.
    db 0                         ; Base (high).
.code: equ $ - init_gdt         ; The code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10011010b                 ; Access (exec/read).
    db 00100000b                 ; Granularity.
    db 0                         ; Base (high).
.data: equ $ - init_gdt         ; The data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10010010b                 ; Access (read/write).
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
.ptr:
    ; GDT PTR
    dw $ - init_gdt - 1             ; Limit.
    dq GET_PADDR(init_gdt)          ; Base.
