global hal_write_port
global hal_read_port
global hal_enable_interrupt
global hal_disable_interrupt
global hal_interrupt_handler_wrapper
extern hal_interrupt_handler_dummy

%macro pushaq 0
   push rax      ;save current rax
   push rbx      ;save current rbx
   push rcx      ;save current rcx
   push rdx      ;save current rdx
   push rbp      ;save current rbp
   push rdi      ;save current rdi
   push rsi      ;save current rsi
   push r8         ;save current r8
   push r9         ;save current r9
   push r10      ;save current r10
   push r11      ;save current r11
   push r12      ;save current r12
   push r13      ;save current r13
   push r14      ;save current r14
   push r15      ;save current r15
%endmacro

%macro popaq 0
    pop r15         ;restore current r15
    pop r14         ;restore current r14
    pop r13         ;restore current r13
    pop r12         ;restore current r12
    pop r11         ;restore current r11
    pop r10         ;restore current r10
    pop r9         ;restore current r9
    pop r8         ;restore current r8
    pop rsi         ;restore current rsi
    pop rdi         ;restore current rdi
    pop rbp         ;restore current rbp
    pop rdx         ;restore current rdx
    pop rcx         ;restore current rcx
    pop rbx         ;restore current rbx
    pop rax         ;restore current rax
%endmacro

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
xchg bx,bx
iretq