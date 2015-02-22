global flush_gdt
global flush_tlb
;Functions preserve the registers rbx, rsp, rbp, r12, r13, r14, and 15
;rax, rdi, rsi, rdx, rcx, r8, r9, r10, r11 are scratch registers.

[SECTION .text]
[BITS 64]
flush_gdt:
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
push qword flush_gdt.reload ;eip
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
flush_tlb:
mov rax,cr3
mov cr3,rax
ret