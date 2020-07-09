;Functions preserve the registers rbx, rsp, rbp, r12, r13, r14, and 15
;rax, rdi, rsi, rdx, rcx, r8, r9, r10, r11 are scratch registers.
;function parameter: rdi,rsi,rdx,rcx,r8,r9

global arch_flush_gdt
global arch_flush_tlb
global arch_flush_idt
global arch_read_idt
global arch_read_cr3
global arch_write_cr3
global arch_read_cr8
global arch_write_cr8
global archp_cpuid
global arch_halt_cpu
global arch_read_msr
global arch_write_msr


section .text
bits 64

; uint64* arch_random_int64(void)
; Returns a random 64-bit integer
global arch_random_int64
arch_random_int64:
rdrand rax
ret

; uint32* arch_random_int32(void)
; Returns a random 32-bit integer
global arch_random_int32
arch_random_int32:
rdrand eax
ret



arch_flush_gdt:
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


arch_flush_tlb:
mov rax,cr3
mov cr3,rax
ret


arch_flush_idt:
lidt [rdi]
ret

;======================
global arch_read_idt
arch_read_idt:
sidt [rdi]
ret

;======================
global arch_read_cr3
arch_read_cr3:
mov rax,cr3
ret

;======================
global arch_write_cr3
arch_write_cr3:
mov cr3,rdi
ret

;======================
global arch_read_cr8
arch_read_cr8:
mov rax,cr8
ret

;======================
global arch_write_cr8
arch_write_cr8:
mov cr8,rdi
ret

;======================
global archp_halt
archp_halt:
hlt


; ============================
; extern void KAPI archp_cpuid(uint32* eax, uint32* ebx, uint32* ecx, uint32* edx);
global archp_cpuid
archp_cpuid:
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
global arch_halt_cpu
arch_halt_cpu:
.loop:
hlt
jmp .loop

;====================
;(uint32 *ecx, uint32* edx, uint32* eax)
global arch_read_msr
arch_read_msr:
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
global arch_write_msr
arch_write_msr:
mov ecx, dword [rdi]
mov eax, dword [rdx]
mov edx, dword [rsi]
wrmsr
ret


global arch_write_port_16
global arch_write_port_32
global arch_write_port_8
global arch_read_port_8
global arch_read_port_16
global arch_read_port_32

arch_write_port_32:
mov rdx,rdi
mov rax,rsi
out dx,eax
nop
nop
nop
ret


arch_write_port_16:
mov rdx,rdi
mov rax,rsi
out dx,ax
nop
nop
nop
ret


arch_write_port_8:
mov rdx,rdi
mov rax,rsi
out dx,al
nop
nop
nop
ret

arch_read_port_8:
mov rdx,rdi
xor rax,rax
in al,dx
nop
nop
nop
ret

arch_read_port_16:
mov rdx,rdi
xor rax,rax
in ax,dx
nop
nop
nop
ret

arch_read_port_32:
mov rdx,rdi
xor rax,rax
in eax,dx
nop
nop
nop
ret