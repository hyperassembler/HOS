global hal_flush_gdt
global hal_flush_tlb
global hal_flush_idt
global hal_cpuid
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


