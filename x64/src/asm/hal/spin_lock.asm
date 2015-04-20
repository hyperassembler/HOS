global hal_spin_lock;
global hal_spin_unlock;

;void hal_spin_lock(uint32_t * lock)
hal_spin_lock:
xor rcx,rcx
inc rcx
.spin:
xor rax,rax
lock cmpxchg dword [rdi],ecx
jnz .spin
ret

;void hal_spin_unlock(uint32_t * lock)
hal_spin_unlock:
mov dword [rdi],0
ret