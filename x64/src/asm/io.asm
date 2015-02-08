global hk_write_port
global hk_read_port
[SECTION .text]
[BITS 64]
hk_write_port:
mov rdx,rdi
mov rax,rsi
out dx,eax
nop
nop
ret

hk_read_port:
mov rdx,rdi
xor rax,rax
in eax,dx
nop
nop
ret