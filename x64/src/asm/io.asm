global write_port
global read_port
[SECTION .text]
[BITS 64]
write_port:
mov rdx,rdi
mov rax,rsi
out dx,eax
nop
nop
ret

read_port:
mov rdx,rdi
xor rax,rax
in eax,dx
nop
nop
ret