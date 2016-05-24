; Copyright 2016 secXsQuared
; Distributed under GPL license
; See COPYING under root for details

global hal_flush_gdt
global hal_flush_tlb
global hal_flush_idt
global hal_cpuid
global hal_read_page_base
global hal_write_page_base
;Functions preserve the registers rbx, rsp, rbp, r12, r13, r14, and 15
;rax, rdi, rsi, rdx, rcx, r8, r9, r10, r11 are scratch registers.
;function parameter: rdi,rsi,rdx,rcx,r8,r9

[SECTION .text]
[BITS 64]
hal_flush_gdt:
push rbp
mov rbp,rsp
lgdt [rdi]
;reload cs

push rdx ; data_slct : ss
push rbp ; rsp

pushfq
pop rax
push rax ; eflags

push rsi ; cs
push qword .reload ;eip
iretq
.reload:
mov es,dx
mov fs,dx
mov gs,dx
mov ds,dx
mov rsp,rbp
pop rbp
ret

;void flush_tlb(void)
hal_flush_tlb:
mov rax,cr3
mov cr3,rax
ret


;void get_cpuid(int64_t* rax, int64_t* rbx, int64_t* rcx, int64_t* rdx)
hal_cpuid:
mov rax,[rdi]
push rcx
mov rcx,[rdx]
cpuid
mov [rdi],rax
mov [rsi],rbx
mov [rdx],rcx
pop rcx
mov [rcx],rdx
ret

;flush_idt
hal_flush_idt:
lidt [rdi]
ret

;read_cr3
hal_read_page_base:
mov rax,cr3
mov r11,0xFFFFFFFFFF000
and rax,r11 ;keep bits 12-51
ret

;write_cr3
hal_write_page_base:
mov r11,0xFFFFFFFFFF000
and rdi,r11 ;keep bits 12-51
mov cr3,rdi
ret
