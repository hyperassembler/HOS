#pragma once

#include <common/cdef.h>

// implements a simple ticket lock
struct ke_spin_lock {
    //  LOW 16 bits: cur ticket
    //  HIGH 16 bits: cur owner
    DECL_ATOMIC(uint32) val;
};

#define KE_SPIN_LOCK_INITIALIZER {.val = ATOMIC_VAR_INIT(0)}

STATIC_ASSERT(sizeof(struct ke_spin_lock) == sizeof(uint32), "ke_spin_lock size isn't 32 bits");

void
ke_spin_lock_init(struct ke_spin_lock *lock);

void
ke_spin_lock_acq(struct ke_spin_lock *lock);

void
ke_spin_lock_rel(struct ke_spin_lock *lock);

// returns non-zero on success otherwise zero
int
ke_spin_lock_try_acq(struct ke_spin_lock *lock);
