#include "hal_atomic.h"

void KAPI hal_spin_lock(uint64_t *lock)
{
    if (lock != NULL)
    {
        while (hal_interlocked_exchange(lock, 1) == 1);
    }
    return;
}

void KAPI hal_spin_unlock(uint64_t *lock)
{
    if (lock != NULL)
    {
        *lock = 0;
    }
    return;
}