#include "bifrost_rwlock.h"
#include "bifrost_intr.h"

void ke_rwlock_init(hw_rwlock_t *lock)
{
    if(lock != NULL)
    {
        ke_spin_lock_init(&lock->w_mutex);
        ke_spin_lock_init(&lock->r_mutex);
        ke_spin_lock_init(&lock->res_lock);
        ke_spin_lock_init(&lock->r_try);
        lock->reader_ct = 0;
        lock->writer_ct = 0;
    }
    return;
}

void ke_reader_lock(hw_rwlock_t *lock)
{
    if(lock != NULL)
    {
        ke_spin_lock(&lock->r_try);
        ke_spin_lock(&lock->r_mutex);
        lock->reader_ct++;
        if(lock->reader_ct == 1)
        {
            ke_spin_lock(&lock->res_lock);
        }
        ke_spin_unlock(&lock->r_mutex);
        ke_spin_unlock(&lock->r_try);
    }
    return;
}

void ke_reader_unlock(hw_rwlock_t *lock)
{
    if(lock != NULL)
    {
        ke_spin_lock(&lock->r_mutex);
        lock->reader_ct--;
        if(lock->reader_ct == 0)
        {
            ke_spin_unlock(&lock->res_lock);
        }
        ke_spin_unlock(&lock->r_mutex);
    }
    return;
}

hw_irql_t ke_reader_lock_raise_irql(hw_rwlock_t *lock, hw_irql_t irql)
{
    hw_irql_t msk = ke_raise_irql(irql);
    if(lock != NULL)
    {
        ke_reader_lock(lock);
    }
    return msk;
}

void ke_reader_unlock_lower_irql(hw_rwlock_t *lock, hw_irql_t irq)
{
    if(lock != NULL)
    {
        ke_reader_unlock(lock);
    }
    ke_lower_irql(irq);
    return;
}

void ke_writer_lock(hw_rwlock_t *lock)
{
    ke_spin_lock(&lock->w_mutex);
    lock->writer_ct++;
    if(lock->writer_ct == 1)
    {
        ke_spin_lock(&lock->r_try);
    }
    ke_spin_unlock(&lock->w_mutex);
    ke_spin_lock(&lock->res_lock);
}

void ke_writer_unlock(hw_rwlock_t *lock)
{
    ke_spin_unlock(&lock->res_lock);
    ke_spin_lock(&lock->w_mutex);
    lock->writer_ct--;
    if(lock->writer_ct == 0)
    {
        ke_spin_unlock(&lock->r_try);
    }
    ke_spin_unlock(&lock->w_mutex);
}

hw_irql_t ke_writer_lock_raise_irql(hw_rwlock_t *lock, hw_irql_t irql)
{
    hw_irql_t msk = ke_raise_irql(irql);
    if(lock != NULL)
    {
        ke_reader_lock(lock);
    }
    return msk;
}

void ke_writer_unlock_lower_irql(hw_rwlock_t *lock, hw_irql_t irq)
{
    if(lock != NULL)
    {
        ke_reader_unlock(lock);
    }
    ke_lower_irql(irq);
    return;
}