global hk_load_gdt
global hk_support_x64
global hk_disable_paging
global hk_enable_paging
[SECTION .text]
[BITS 32]
;void hk_load_gdt(gdt_ptr* ptr, uint16 SLCT_CODE, uint16 SLCT_DATA)
hk_load_gdt:
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

;int hk_support_x64(void)
hk_support_x64:
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


;void hk_disable_paging(void)
hk_disable_paging:
mov eax, cr0                                   ; Set the A-register to control register 0.
and eax, 01111111111111111111111111111111b     ; Clear the PG-bit, which is bit 31.
mov cr0, eax                                   ; Set control register 0 to the A-register.
ret

;void hk_enable_paging(void)
hk_enable_paging:
mov eax, cr0                                   ; Set the A-register to control register 0.
or eax, 1 << 31                                ; Set the PG-bit, which is bit 31.
mov cr0, eax                                   ; Set control register 0 to the A-register.
ret
