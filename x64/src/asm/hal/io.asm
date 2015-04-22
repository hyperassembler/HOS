%include "../common/sys.asm"

global hal_write_port
global hal_read_port
global hal_enable_interrupt
global hal_disable_interrupt
extern hal_interrupt_handler_dummy

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

hal_disable_interrupt:
cli
ret

hal_enable_interrupt:
sti
ret

hal_interrupt_handler_wrapper:
pushaq
cld
call hal_interrupt_handler_dummy
popaq
iretq