#include "bifrost_lock.h"
#include "bifrost_intr.h"
#include "bifrost_mem.h"

hw_irql_t ke_spin_lock_raise_irql(hw_spin_lock_t *lock, hw_irql_t irql)
{
    hw_irql_t old = ke_raise_irql(irql);
    ke_spin_lock(lock);
    return old;
}

void ke_spin_unlock_lower_irql(hw_spin_lock_t *lock, hw_irql_t irql)
{
    ke_spin_unlock(lock);
    ke_lower_irql(irql);
    return;
}

void ke_spin_lock(hw_spin_lock_t *lock)
{
    if (lock != NULL)
    {
        while (ke_interlocked_compare_exchange(&lock->lock, 0, 1) != 0);
    }
}

void ke_spin_unlock(hw_spin_lock_t *lock)
{
    if (lock != NULL)
    {
        lock->lock = 0;
    }
}

void ke_spin_lock_init(hw_spin_lock_t *lock)
{
    if (lock != NULL)
    {
        lock->lock = 0;
    }
}

