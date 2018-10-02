
section .text
bits 64
; ============================
; int32 KAPI hal_interlocked_exchange_32(int32 *target, int32 val)
global hal_interlocked_exchange_32
hal_interlocked_exchange_32:
lock xchg dword [rdi], esi
xor rax, rax
mov eax, esi
ret

; ============================
; int32 KAPI hal_interlocked_compare_exchange_32(int32 *dst, int32 test_node_compare, int32 val);
global hal_interlocked_compare_exchange_32
hal_interlocked_compare_exchange_32:
mov eax, esi; eax = test_node_compare
lock cmpxchg dword [rdi], edx ; edx = val, rdi = ptr to dst
ret

; ============================
; int32 KAPI hal_interlocked_increment_32(int32 *target, int32 increment);
global hal_interlocked_increment_32
hal_interlocked_increment_32:
lock xadd dword [rdi], esi ; [rdi] = [rdi] + esi, esi = old [rdi]
xor rax, rax
mov eax, esi
ret
