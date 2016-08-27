#include "k_atomic.h"

void KAPI ke_spin_lock(k_spin_lock_t *lock)
{
    if (lock != NULL)
    {
        while (ke_interlocked_exchange(&lock->val, 1) == 1);
    }
    return;
}

void KAPI ke_spin_unlock(k_spin_lock_t *lock)
{
    if (lock != NULL)
    {
        lock->val = 0;
    }
    return;
}

k_irql_t KAPI ke_spin_lock_raise_irql(k_spin_lock_t *lock, k_irql_t irql)
{
    k_irql_t prev_irql = ke_get_irql();
    if(lock != NULL)
    {
        ke_set_irql(irql);
        ke_spin_lock(lock);
    }
    return prev_irql;
}

void KAPI ke_spin_unlock_lower_irql(k_spin_lock_t *lock, k_irql_t irql)
{
    if(lock != NULL)
    {
        ke_spin_unlock(lock);
        ke_set_irql(irql);
    }
    return;
}