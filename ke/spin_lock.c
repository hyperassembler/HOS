#include "ke/spin_lock.h"
#include "ke/atomic.h"

void
ke_spin_init(struct spin_lock *lock)
{
    if (lock != NULL)
    {
        lock->val = 0;
    }
}


void
ke_spin_lock(struct spin_lock *lock)
{
    if (lock != NULL)
    {
        while (ke_atmoic_cmpxchg_32(&lock->val, 0, 1) != 0)
        {}
    }
}


void
ke_spin_unlock(struct spin_lock *lock)
{
    if (lock != NULL)
    {
        lock->val = 0;
    }
}

