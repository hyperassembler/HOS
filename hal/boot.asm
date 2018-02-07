; Copyright 2016 secXsQuared
; Distributed under GPL license
; See COPYING under root for details

%include "hal/addr.inc"

MULTIBOOT_TAG_ALIGNMENT equ 8
MULTIBOOT_HEADER_ALIGNMENT equ 8
MULTIBOOT_LOADED_MAGIC equ 0x36d76289
MULTIBOOT_MAGIC_NUMBER equ 0xE85250D6
MULTIBOOT_ARCH equ 0
; NASM does not like 
MULTIBOOT_CHECK_SUM equ (0xFFFFFFFF - (MULTIBOOT_MAGIC_NUMBER + MULTIBOOT_HEADER_SIZE + MULTIBOOT_ARCH) + 1)
MULTIBOOT_REQ_LOADERNAME equ 2
MULTIBOOT_REQ_MMAP equ 6
MULTIBOOT_REQ_ACPI_RSDP equ 15

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
dd MULTIBOOT_REQ_LOADERNAME
dd MULTIBOOT_REQ_MMAP
dd MULTIBOOT_REQ_ACPI_RSDP
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
global hal_entry_32
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
mov esp, GET_PADDR(kernel_stack_end)

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
and eax, 0x7FFFFFFF                            ; Clear the PG-bit, which is bit 31.
mov cr0, eax                                   ; Set control register 0 to the A-register.

; for pages mapped here, attributes are always:
; PRESENT - 1 << 0
; RW - 1 << 1
; RING0 - 0 << 2
; Page Level Write Back - 0 << 3
; Page Cache Enabled - 0 << 4
; Not Accessed - 0 << 5
; Not dirty - 0 << 6 
; No PAT - 0 << 7 (must be 0 for recursive)
; Not Global - 0 << 8
; Execution Enabled - 0 << 63
; To summary - 3 + ADDR
; since 4k aligned, we can just add the attribute
PAGE_TABLE_ATTRIBUTE equ 0011b

; need to map the PDE (1GB region) of the identity mapping + 2 * PTE (identity + vaddr)
; also need to map the kernel itself to the VIRT_ADDR + PHYS_ADDR
; calculate the Nth 2MB within the first 1GB
mov eax, HAL_KERNEL_BASE_PADDR
mov ecx, 1*1024*1024*1024;
xor edx, edx
div ecx ; we need the remainder here (EDX)
mov eax, edx
mov ecx, 2*1024*1024 ; 2MB
xor edx, edx
div ecx ; divide ecx:eax by ecx EAX = quotient, EDX = remainder, 0 in this case
mov edx, eax

; map the identity PDE
mov eax, GET_PADDR(pde_base_kernel_static_identity)
mov dword [eax + edx * 8], GET_PADDR(pte_base_kernel_static) + PAGE_TABLE_ATTRIBUTE

; map the kernel static PDE
mov eax, GET_PADDR(pde_base_kernel_static)
; map the first 2MB of the last 1GB to kernel static pte
mov dword [eax + edx * 8], GET_PADDR(pte_base_kernel_static) + PAGE_TABLE_ATTRIBUTE

; map everything from the start of the kernel (0MB)
mov eax, GET_PADDR(pte_base_kernel_static)
mov edx, 0 ;512 times
mov ecx, HAL_KERNEL_BASE_PADDR + PAGE_TABLE_ATTRIBUTE
.pte_static_loop:
mov dword [eax + edx * 8], ecx
add ecx, 4096 ;4k per entry
inc edx
cmp edx, 512
jne .pte_static_loop

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
mov eax, GET_PADDR(pml4_base)
mov cr3, eax

; enable paging, enter compatibility mode
mov eax, cr0                                   ; Set the A-register to control register 0.
or eax, 1 << 31                                ; Set the PG-bit, which is bit 31.
mov cr0, eax                                   ; Set control register 0 to the A-register.

; enter x64
lgdt [GET_PADDR(GDT64.GDT64_PTR)]
jmp SLCT_CODE:GET_PADDR(halp_entry_64)
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
extern hal_main
halp_entry_64:
; note that we are still at the identity mapping
mov rax, .entry_64
jmp rax
.entry_64:
mov ax,SLCT_DATA
mov ds,ax
mov es,ax
mov fs,ax
mov gs,ax
mov ss,ax

; set correct rsp to the new kernel stack vaddr, the top of which is the start of the kernel 
mov rsp, HAL_KERNEL_BASE_VADDR 
mov rdi, rsi ; multiboot_info*
add rdi, HAL_KERNEL_BASE_VADDR
call hal_main
hlt

[SECTION .data]
[BITS 64]
; we only map two sections: the last 512 GB for kernel and the second last 512 GB for 
; recursive page table. YA BOI GUZMA
; 0xFFFF 8000 0000 0000 unmapped kernel
; 0xFFFF FF00 0000 0000 512 GB Recursive page table (MAP THIS)
; 0xFFFF FF80 0000 0000 510 GB Kernel Dynamic
; inside dynamic, the lowest addr is for heap and the highest is for the stack
; 0xFFFF FFFF 8000 0000 to 0xFFFF FFFF FFFF FFFF 2 GB Kernel static (MAP THIS)

align 4096
pml4_base:
; 512 GB per entry, we need 4 ENTRIES, BUT ONE IS MAPPED TO ITSELF
; within the first 512G (Definitely since grub does not load to more than 4G), identity map the kernel
dq GET_PADDR(pdpt_base_identity) + PAGE_TABLE_ATTRIBUTE
times 509 dq 0
; 511th entry for 2nd last 512G
dq GET_PADDR(pml4_base) + PAGE_TABLE_ATTRIBUTE
; 512th entry for last 512G (kernel space)
dq GET_PADDR(pdpt_base) + PAGE_TABLE_ATTRIBUTE
 
align 4096
; 1GB per entry
pdpt_base:
; this pdpt is for the pml4 last 512 GB entry 
; we need 3 ENTRIES, 1 for kernel and 2 for kernel dynamics (first 1GB heap + last 1GB stack)
; map the 1st GB to kernel dynamic heap
dq GET_PADDR(pde_base_kernel_dynamic_heap) + PAGE_TABLE_ATTRIBUTE
times 508 dq 0
; map the 510th GB to kernel dynamic stack
dq GET_PADDR(pde_base_kernel_dynamic_stack) + PAGE_TABLE_ATTRIBUTE
; map the 511th GB to kernel static space
dq GET_PADDR(pde_base_kernel_static) + PAGE_TABLE_ATTRIBUTE
; the 512th GB is unmapped
dq 0

pdpt_base_identity:
; this pdpt is for the kernel identity mapping
; the kernel is definitely loaded to the first 1GB
dq GET_PADDR(pde_base_kernel_static_identity) + PAGE_TABLE_ATTRIBUTE
times 511 dq 0

align 4096
; 2MB per entry
pde_base_kernel_dynamic_heap:
; this pde is for the kernel dynamic pdpt entry (kernel heap), we need 1 entry to map the initial 2M
; map the first 2MB of the 510 GB to kernel heap
dq GET_PADDR(pte_base_kernel_dynamic_heap) + PAGE_TABLE_ATTRIBUTE
times 511 dq 0

pde_base_kernel_dynamic_stack:
; this pde is for the kernel dynamic pdpt entry (kernel stack), we need 1 entry to map the last 2M
times 511 dq 0
; map the last 2MB of the 510 GB to kernel stack
dq GET_PADDR(pte_base_kernel_dynamic_stack) + PAGE_TABLE_ATTRIBUTE

pde_base_kernel_static:
; this pde is for the kernel static pdpt entry (kernel core), we need 1 entry to map the initial 2M
; map the first 2MB of the last 1GB to kernel static pte
times 512 dq 0

pde_base_kernel_static_identity:
; this pde is for the kernel static identity pdpt entry (kernel core), we need 1 entry to map the initial 2M
times 512 dq 0

align 4096
; 4KB per entry
pte_base_kernel_dynamic_heap:
; this pte is for the kernel dynamic heap pde entry, we need 1 entry to map the kernel heap (4k)
; map the heap segment (first 4k)
dq GET_PADDR(kernel_heap) + PAGE_TABLE_ATTRIBUTE
times 511 dq 0

pte_base_kernel_dynamic_stack:
; this pte is for the kernel dynamic stack pde entry, we need 1 entry to map the kernel stack (4k)
times 511 dq 0
; map the stack segment (last 4k)
dq GET_PADDR(kernel_stack) + PAGE_TABLE_ATTRIBUTE

pte_base_kernel_static:
; this pte is for the kernel static pde entry, we need 512 entries, map the complete 1st 2MB (TODO: calculate kernel size)
times 512 dq 0

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
    db 00100000b                 ; Granularity.
    db 0                         ; Base (high).
    .GDT64_PTR:                  ; The GDT-pointer.
    dw $ - GDT64 - 1             ; Limit.
    dq GET_PADDR(GDT64)          ; Base.

align 4096
kernel_stack:
times 4096 db 0
kernel_stack_end:

align 4096
kernel_heap:
times 4096 db 0