extern hk_main
[SECTION .entry]
[BITS 64]
cli
;hard code for now
mov ax,24
mov ds,ax
mov es,ax
mov fs,ax
mov gs,ax
mov ss,ax
;well align 16 bytes like this for now
mov rax,rsp
and rax,0xFFFFFFFFFFFFFFF0
mov rsp,rax
;no params for now
call hk_main
hlt