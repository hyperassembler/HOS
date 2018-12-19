section .text
bits 64

; Performs compare and swap
; ARGS:
; 1 int32*: target ptr
; 2 int32: old value
; 3 int32: new value
; RETURNS uint32 value read
global cmp_swp_32
cmp_swp_32:
mov eax, esi
lock cmpxchg dword [rdi], edx
ret

; Performs fetch and add
; ARGS:
; 1 int32*: target ptr
; 2 int32: increment value
; 3 int32: new value
; RETURNS uint32 value read
global fetch_add_32:
fetch_add_32:
lock xadd dword [rdi], esi
xor rax, rax
mov eax, esi
ret
