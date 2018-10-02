section .text
bits 64

global hal_write_port_16
global hal_write_port_32
global hal_write_port_8
global hal_read_port_8
global hal_read_port_16
global hal_read_port_32

hal_write_port_32:
mov rdx,rdi
mov rax,rsi
out dx,eax
nop
nop
nop
ret


hal_write_port_16:
mov rdx,rdi
mov rax,rsi
out dx,ax
nop
nop
nop
ret


hal_write_port_8:
mov rdx,rdi
mov rax,rsi
out dx,al
nop
nop
nop
ret


hal_read_port_8:
mov rdx,rdi
xor rax,rax
in al,dx
nop
nop
nop
ret

hal_read_port_16:
mov rdx,rdi
xor rax,rax
in ax,dx
nop
nop
nop
ret


hal_read_port_32:
mov rdx,rdi
xor rax,rax
in eax,dx
nop
nop
nop
ret