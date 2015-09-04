global load_gdt
global disable_paging
global enable_paging
[SECTION .text]
[BITS 32]
;void load_gdt(gdt_ptr* ptr, uint16 SLCT_CODE, uint16 SLCT_DATA)
load_gdt:
push ebp
mov ebp,esp
push eax
mov eax,[ss:ebp+8]
lgdt [eax]
;reload cs
mov eax,[ss:ebp+12]
push eax
push dword .reload
retf
.reload:
mov eax,[ss:ebp+16]
mov ss,ax
mov es,ax
mov fs,ax
mov gs,ax
mov ds,ax
pop eax
mov esp,ebp
pop ebp
ret

;void disable_paging(void)
disable_paging:
mov eax, cr0                                   ; Set the A-register to control register 0.
and eax, 01111111111111111111111111111111b     ; Clear the PG-bit, which is bit 31.
mov cr0, eax                                   ; Set control register 0 to the A-register.
ret

;void enable_paging(void)
enable_paging:
mov eax, cr0                                   ; Set the A-register to control register 0.
or eax, 1 << 31                                ; Set the PG-bit, which is bit 31.
mov cr0, eax                                   ; Set control register 0 to the A-register.
ret

;void flush_tlb(void)
flush_tlb:
mov eax,cr3
mov cr3,eax
ret