global hk_flush_gdt
global hk_flush_tlb
;Functions preserve the registers rbx, rsp, rbp, r12, r13, r14, and 15
;rax, rdi, rsi, rdx, rcx, r8, r9, r10, r11 are scratch registers.

[SECTION .text]
[BITS 64]
hk_flush_gdt:
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
push qword hk_flush_gdt.reload ;eip
iretq
.reload:
mov es,dx
mov fs,dx
mov gs,dx
mov ds,dx
mov rsp,rbp
pop rbp
ret

;void hk_flush_tlb(void)
hk_flush_tlb:
mov rax,cr3
mov cr3,rax
ret