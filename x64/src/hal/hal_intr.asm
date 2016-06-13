; Copyright 2016 secXsQuared
; Distributed under GPL license
; See COPYING under root for details

%macro PUSHAQ 0
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

%macro POPAQ 0
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

%macro hal_interrupt_handler 1
global hal_interrupt_handler_%1
hal_interrupt_handler_%1:
; save top of stack
; NOW STACK:
; +40 SS
; +32 RSP
; +24 RFLAGS
; +16 CS
; +8  RIP
; +0  RBP
push rbp
mov rbp,rsp
PUSHAQ
cld
mov rdi, %1  ; INT VEC #
mov rsi, rbp ; PTR to RIP
add rsi, 8
mov rdx, 0   ; ERROR = 0, in this case
call hal_interrupt_dispatcher
POPAQ
pop rbp
iretq
%endmacro

%macro hal_interrupt_err_handler 1
global hal_interrupt_handler_%1
hal_interrupt_handler_%1:
; save top of stack
; NOW STACK:
; +48 SS
; +40 RSP
; +32 RFLAGS
; +24 CS
; +16 RIP
; +8  ERROR CODE
; +0  RBP
push rbp
mov rbp,rsp
PUSHAQ
cld
mov rdi, %1 ; INT VEC #
mov rsi, rbp ; PTR to RIP
add rsi, 16
mov rdx, qword [rbp + 8] ; ERRPO CODE
call hal_interrupt_dispatcher
POPAQ
pop rbp
add rsp, 8 ; skip the error code
iretq
%endmacro

[SECTION .text]
[BITS 64]

extern hal_interrupt_dispatcher

hal_interrupt_handler 0
hal_interrupt_handler 1
hal_interrupt_handler 2
hal_interrupt_handler 3
hal_interrupt_handler 4
hal_interrupt_handler 5
hal_interrupt_handler 6
hal_interrupt_handler 7
hal_interrupt_err_handler 8
hal_interrupt_err_handler 10
hal_interrupt_err_handler 11
hal_interrupt_err_handler 12
hal_interrupt_err_handler 13
hal_interrupt_err_handler 14
hal_interrupt_handler 16
hal_interrupt_err_handler 17
hal_interrupt_handler 18
hal_interrupt_handler 19
hal_interrupt_handler 20

; user defined
hal_interrupt_handler 32
hal_interrupt_handler 33
hal_interrupt_handler 34
hal_interrupt_handler 35
hal_interrupt_handler 36
hal_interrupt_handler 37
hal_interrupt_handler 38
hal_interrupt_handler 39