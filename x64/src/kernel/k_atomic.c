#include "k_atomic.h"
#include "s_hal.h"

void KAPI k_spin_lock(k_spin_lock_t *lock)
{
    if (lock != NULL)
    {
        while (hal_interlocked_exchange(&lock->val, 1) == 1);
    }
    return;
}

void KAPI k_spin_unlock(k_spin_lock_t *lock)
{
    if (lock != NULL)
    {
        lock->val = 0;
    }
    return;
}