#pragma once

#include "cdef.h"

struct spin_lock
{
    int32 val;
};

void
ke_spin_init(struct spin_lock *lock);

void
ke_spin_lock(struct spin_lock *lock);

void
ke_spin_unlock(struct spin_lock *lock);
