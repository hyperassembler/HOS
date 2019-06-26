#include <kern/cdef.h>
#include <kern/spin_lock.h>
#include <arch/atomic.h>

void
spin_init(struct spin_lock *lock)
{
    if (lock != NULL)
    {
        lock->val = 0;
    }
}


void
spin_lock(struct spin_lock *lock)
{
    if (lock != NULL)
    {
        while (arch_cmp_swp_32(&lock->val, 0, 1) != 0)
        {}
    }
}


void
spin_unlock(struct spin_lock *lock)
{
    if (lock != NULL)
    {
        lock->val = 0;
    }
}

