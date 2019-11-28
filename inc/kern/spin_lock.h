#pragma once

#include <kern/cdef.h>

// implements a simple ticket lock
struct spin_lock {
    //  LOW 16 bits: cur ticket
    //  HIGH 16 bits: cur owner
    DECL_ATOMIC(uint32) val;
};

#define SPIN_LOCK_INITIALIZER {.val = ATOMIC_VAR_INIT(0)}

STATIC_ASSERT(sizeof(struct spin_lock) == sizeof(uint32), "spin_lock size isn't 32 bits");

void
spin_lock_init(struct spin_lock *lock);

void
spin_lock_acq(struct spin_lock *lock);

void
spin_lock_rel(struct spin_lock *lock);

// returns non-zero on success otherwise zero
int
spin_lock_try_acq(struct spin_lock *lock);
