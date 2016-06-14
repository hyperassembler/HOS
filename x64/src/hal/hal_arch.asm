; Copyright 2016 secXsQuared
; Distributed under GPL license
; See COPYING under root for details

;Functions preserve the registers rbx, rsp, rbp, r12, r13, r14, and 15
;rax, rdi, rsi, rdx, rcx, r8, r9, r10, r11 are scratch registers.
;function parameter: rdi,rsi,rdx,rcx,r8,r9

[SECTION .text]
[BITS 64]
;======================
global hal_flush_gdt
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
push qword .reload ;rip
iretq
.reload:
mov es,dx
mov fs,dx
mov gs,dx
mov ds,dx
pop rbp
ret

;======================
global hal_flush_tlb
;void flush_tlb(void)
hal_flush_tlb:
mov rax,cr3
mov cr3,rax
ret

;======================
global hal_flush_idt
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


; ============================
; uint64_t KAPI hal_interlocked_exchange(uint64_t* dst, uint64_t val);
global hal_interlocked_exchange
hal_interlocked_exchange:
lock xchg qword [rdi], rsi
mov rax, rsi
ret

; ============================
; extern void KAPI hal_cpuid(uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx);
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
global hal_write_port
hal_write_port:
mov rdx,rdi
mov rax,rsi
out dx,eax
nop
nop
ret

;====================
global hal_read_port
hal_read_port:
mov rdx,rdi
xor rax,rax
in eax,dx
nop
nop
ret

;====================
global hal_write_mem_32
; (void* target, uint32_t* data)
hal_write_mem_32:
mov dword [rdi], esi
ret

;====================
global hal_write_mem_64
; (void* target, uint64_t data)
hal_write_mem_64:
mov qword [rdi], rsi
ret

;====================
global hal_disable_interrupt
hal_disable_interrupt:
cli
ret

;====================
global hal_enable_interrupt
hal_enable_interrupt:
sti
ret

;====================
global hal_halt_cpu
hal_halt_cpu:
.loop:
hlt
jmp .loop

;====================
global hal_bochs_magic_breakpoint
hal_bochs_magic_breakpoint:
xchg bx,bx
ret

;====================
;(uint32_t *ecx, uint32_t* edx, uint32_t* eax)
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
;(uint32_t *ecx, uint32_t* edx, uint32_t* eax)
global hal_write_msr
hal_write_msr:
mov ecx, dword [rdi]
mov eax, dword [rdx]
mov edx, dword [rsi]
wrmsr
ret