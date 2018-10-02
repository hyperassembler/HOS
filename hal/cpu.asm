;Functions preserve the registers rbx, rsp, rbp, r12, r13, r14, and 15
;rax, rdi, rsi, rdx, rcx, r8, r9, r10, r11 are scratch registers.
;function parameter: rdi,rsi,rdx,rcx,r8,r9

global hal_flush_gdt
global hal_flush_tlb
global hal_flush_idt
global hal_read_idt
global hal_read_cr3
global hal_write_cr3
global hal_read_cr8
global hal_write_cr8
global hal_cpuid
global hal_halt_cpu
global hal_read_msr
global hal_write_msr


section .text
bits 64

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
mov rax, .reload
push rax ;rip
iretq
.reload:
mov es,dx
mov fs,dx
mov gs,dx
mov ds,dx
pop rbp
ret


hal_flush_tlb:
mov rax,cr3
mov cr3,rax
ret


hal_flush_idt:
lidt [rdi]
ret

;======================
global hal_read_idt
hal_read_idt:
sidt [rdi]
ret

;======================
global hal_read_cr3
hal_read_cr3:
mov rax,cr3
ret

;======================
global hal_write_cr3
hal_write_cr3:
mov cr3,rdi
ret

;======================
global hal_read_cr8
hal_read_cr8:
mov rax,cr8
ret

;======================
global hal_write_cr8
hal_write_cr8:
mov cr8,rdi
ret

; ============================
; extern void KAPI hal_cpuid(uint32* eax, uint32* ebx, uint32* ecx, uint32* edx);
global hal_cpuid
hal_cpuid:
push rbp
mov rbp,rsp
; preserve rbx,rcx,rdx
push rbx
push rcx
push rdx
; cpuid parameters eax,ecx
mov eax, dword [rdi]
mov ecx, dword [rdx]
cpuid
; write results back to memory
mov dword [rdi], eax
mov dword [rsi], ebx
pop r11
mov dword [r11], ecx
pop r11
mov dword [r11], edx
pop rbx
mov rsp,rbp
pop rbp
ret

;====================
global hal_halt_cpu
hal_halt_cpu:
.loop:
hlt
jmp .loop

;====================
;(uint32 *ecx, uint32* edx, uint32* eax)
global hal_read_msr
hal_read_msr:
; preserve rdx
push rdx
mov ecx, dword [rdi]
rdmsr
mov dword [rdi], ecx
mov dword [rsi], edx
pop r11
mov dword [r11], eax
ret

;====================
;(uint32 *ecx, uint32* edx, uint32* eax)
global hal_write_msr
hal_write_msr:
mov ecx, dword [rdi]
mov eax, dword [rdx]
mov edx, dword [rsi]
wrmsr
ret