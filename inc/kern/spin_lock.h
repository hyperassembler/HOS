#pragma once

#include <kern/cdef.h>

struct spin_lock
{
    int32 val;
};

void
spin_init(struct spin_lock *lock);

void
spin_lock(struct spin_lock *lock);

void
spin_unlock(struct spin_lock *lock);
