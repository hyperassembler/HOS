; Copyright 2016 secXsQuared
; Distributed under GPL license
; See COPYING under root for details

extern hal_main

HAL_KERNEL_BASE_VADDR equ 0xFFFFFFFF80000000
global HAL_KERNEL_BASE_VADDR 

HAL_KERNEL_BASE_PADDR equ 0x4000000
global HAL_KERNEL_BASE_PADDR

HAL_ENTRY32_PADDR equ hal_entry_32 - HAL_KERNEL_BASE_VADDR
global HAL_ENTRY32_PADDR

MULTIBOOT_TAG_ALIGNMENT equ 8
MULTIBOOT_HEADER_ALIGNMENT equ 8
MULTIBOOT_LOADED_MAGIC equ 0x36d76289
MULTIBOOT_MAGIC_NUMBER equ 0xE85250D6
MULTIBOOT_ARCH equ 0
MULTIBOOT_CHECK_SUM equ (0 - (MULTIBOOT_MAGIC_NUMBER + MULTIBOOT_HEADER_SIZE + MULTIBOOT_ARCH))
MULTIBOOT_REQ_MINFO equ 4
MULTIBOOT_REQ_MMAP equ 6
MULTIBOOT_REQ_APM equ 10

[SECTION .multiboot_header]
[BITS 32]
;====================
;header tag
align MULTIBOOT_HEADER_ALIGNMENT
multiboot_header_tag:
dd MULTIBOOT_MAGIC_NUMBER
dd MULTIBOOT_ARCH
dd MULTIBOOT_HEADER_SIZE
dd MULTIBOOT_CHECK_SUM
;====================
;INFO_REQUEST_TAG
align MULTIBOOT_TAG_ALIGNMENT
multiboot_info_tag:
dw 0x1 ; type=1
dw 0x0 ; flag=0
dd MULTIBOOT_INFO_TAG_SIZE
;dd MULTIBOOT_REQ_MINFO
dd MULTIBOOT_REQ_MMAP
dd MULTIBOOT_REQ_APM
MULTIBOOT_INFO_TAG_SIZE equ ($ - multiboot_info_tag)
;====================
;MODULE ALIGNMENT TAG
align MULTIBOOT_TAG_ALIGNMENT
dw 0x6; type=6
dw 0x0; flag=0
dd 0x8
;====================
;End_tag
align MULTIBOOT_TAG_ALIGNMENT
dw 0x0
dw 0x0
dd 0x8
;====================
MULTIBOOT_HEADER_SIZE equ ($ - multiboot_header_tag)

[SECTION .text]
[BITS 32]

align 4096
hal_entry_32:
; close interrupt
cli
cld

; check loaded by grub
cmp eax,MULTIBOOT_LOADED_MAGIC
je .loaded_by_grub
hlt
.loaded_by_grub:

; set stack pointer
mov esp, 0

; save multiboot_info*
mov esi,ebx

; check x64 support
call halp_ensure_support_x64
cmp eax,1
je .init_x64
hlt

.init_x64:
; disable paging first
mov eax, cr0                                   ; Set the A-register to control register 0.
and eax, 01111111111111111111111111111111b     ; Clear the PG-bit, which is bit 31.
mov cr0, eax                                   ; Set control register 0 to the A-register.

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
lgdt [GDT64.GDT64_PTR]
jmp SLCT_CODE:halp_entry_64
hlt

halp_ensure_support_x64:
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
jz .not_supported      ; They aren't, there is no long mode.
mov eax,1
jmp .end
.not_supported:
xor eax,eax
.end:
mov esp,ebp
pop ebp
ret

[SECTION .text]
[BITS 64]
halp_entry_64:
cli
mov ax,SLCT_DATA
mov ds,ax
mov es,ax
mov fs,ax
mov gs,ax
mov ss,ax

; align 16 bytes like this for now
mov rsp, 0
mov rdi,rsi ; multiboot_info*
call hal_main
hlt

[SECTION .data]
[BITS 64]
KERNEL_HEAP_SIZE equ 8192
KERNEL_STACK_SIZE equ 8192

align 4096 ;4k alignment
times KERNEL_HEAP_SIZE db 0 ; initially 8k heap

align 4096 ;4k alignment
times KERNEL_STACK_SIZE db 0 ; initially 8k stack

align 4096
; temporary page table
PML4_BASE:
times 512 dq 0 ;reserved the rest for page entries

align 4096
PDPT_BASE:
times 512 dq 0 ;reserved the rest for page entries

align 4096
; long mode gdt
GDT64:                           ; Global Descriptor Table (64-bit).
    ; NULL
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0                         ; Access.
    db 0                         ; Granularity.
    db 0                         ; Base (high).
    SLCT_CODE equ $ - GDT64      ; The code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10011010b                 ; Access.
    db 00100000b                 ; Granularity.
    db 0                         ; Base (high).
    SLCT_DATA equ $ - GDT64      ; The data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10010010b                 ; Access.
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
    .GDT64_PTR:                  ; The GDT-pointer.
    dw $ - GDT64 - 1             ; Limit.
    dq GDT64                     ; Base.
