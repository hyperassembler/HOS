#include "k_atomic.h"

void KAPI k_spin_lock(k_spin_lock_t *lock)
{
    if (lock != NULL)
    {
        while (k_interlocked_exchange(&lock->val, 1) == 1);
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

k_irql_t KAPI k_spin_lock_irql_set(k_spin_lock_t *lock, k_irql_t irql)
{
    k_irql_t prev_irql = k_get_irql();
    if(lock != NULL)
    {
        k_set_irql(irql);
        k_spin_lock(lock);
    }
    return prev_irql;
}

void KAPI k_spin_unlock_irql_restore(k_spin_lock_t *lock, k_irql_t irql)
{
    if(lock != NULL)
    {
        k_spin_unlock(lock);
        k_set_irql(irql);
    }
    return;
}