#pragma once

#include "cdef.h"
#include "ke/spin_lock.h"

struct rww_lock
{
    struct spin_lock w_mutex;
    struct spin_lock r_mutex;
    struct spin_lock res_lock;
    struct spin_lock r_try;
    uint32 reader_ct;
    uint32 writer_ct;
};

void
ke_rww_init(struct rww_lock *lock);

void
ke_rww_r_lock(struct rww_lock *lock);

void
ke_rww_r_unlock(struct rww_lock *lock);

void
ke_rww_w_lock(struct rww_lock *lock);

void
ke_rww_w_unlock(struct rww_lock *lock);
