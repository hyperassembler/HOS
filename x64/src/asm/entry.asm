extern hk_main
[SECTION .entry]
[BITS 32] ;on stack: multiboot_info*
; no, no interrupt please.
cli

; disable paging first
mov eax, cr0                                   ; Set the A-register to control register 0.
and eax, 01111111111111111111111111111111b     ; Clear the PG-bit, which is bit 31.
mov cr0, eax                                   ; Set control register 0 to the A-register.
ret

xchg bx,bx ; pure magic

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
mov eax,0;page table addr
mov cr3,eax

; enable paging, enter compatibility mode
mov eax, cr0                                   ; Set the A-register to control register 0.
or eax, 1 << 31                                ; Set the PG-bit, which is bit 31.
mov cr0, eax                                   ; Set control register 0 to the A-register.
ret

; enter x64
;lgdt [g_gdt_ptr_64]
jmp 8:entry_64

[SECTION .text]
[BITS 64]
entry_64:
cli
;hard code for now
mov ax,24
mov ds,ax
mov es,ax
mov fs,ax
mov gs,ax
mov ss,ax
;well align 16 bytes like this for now
mov rax,rsp
and rax,0xFFFFFFFFFFFFFFF0
mov rsp,rax
;no params for now
call hk_main
hlt