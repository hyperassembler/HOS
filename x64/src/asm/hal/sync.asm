; Copyright 2016 secXsQuared
; Distributed under GPL license
; See COPYING under root for details

global hal_interlocked_exchange;

;uint64_t _KERNEL_ABI hal_interlocked_exchange(_IN _OUT uint64_t* dst, _IN uint64_t val);
hal_interlocked_exchange:
lock xchg qword [rdi], rsi
mov rax, rsi
ret