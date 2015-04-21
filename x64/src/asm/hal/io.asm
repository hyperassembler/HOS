global hal_write_port
global hal_read_port
[SECTION .text]
[BITS 64]
hal_write_port:
mov rdx,rdi
mov rax,rsi
out dx,eax
nop
nop
ret

hal_read_port:
mov rdx,rdi
xor rax,rax
in eax,dx
nop
nop
ret