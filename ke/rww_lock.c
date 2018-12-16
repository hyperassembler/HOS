#include "ke/rww_lock.h"

void
ke_rww_init(struct rww_lock *lock)
{
    if (lock != NULL)
    {
        ke_spin_init(&lock->w_mutex);
        ke_spin_init(&lock->r_mutex);
        ke_spin_init(&lock->res_lock);
        ke_spin_init(&lock->r_try);
        lock->reader_ct = 0;
        lock->writer_ct = 0;
    }
}

void
ke_rww_r_lock(struct rww_lock *lock)
{
    if (lock != NULL)
    {
        ke_spin_lock(&lock->r_try);
        ke_spin_lock(&lock->r_mutex);
        lock->reader_ct++;
        if (lock->reader_ct == 1)
        {
            ke_spin_lock(&lock->res_lock);
        }
        ke_spin_unlock(&lock->r_mutex);
        ke_spin_unlock(&lock->r_try);
    }
}

void
ke_rww_r_unlock(struct rww_lock *lock)
{
    if (lock != NULL)
    {
        ke_spin_lock(&lock->r_mutex);
        lock->reader_ct--;
        if (lock->reader_ct == 0)
        {
            ke_spin_unlock(&lock->res_lock);
        }
        ke_spin_unlock(&lock->r_mutex);
    }
}

void
ke_rww_w_lock(struct rww_lock *lock)
{
    ke_spin_lock(&lock->w_mutex);
    lock->writer_ct++;
    if (lock->writer_ct == 1)
    {
        ke_spin_lock(&lock->r_try);
    }
    ke_spin_unlock(&lock->w_mutex);
    ke_spin_lock(&lock->res_lock);
}

void
ke_rww_w_unlock(struct rww_lock *lock)
{
    ke_spin_unlock(&lock->res_lock);
    ke_spin_lock(&lock->w_mutex);
    lock->writer_ct--;
    if (lock->writer_ct == 0)
    {
        ke_spin_unlock(&lock->r_try);
    }
    ke_spin_unlock(&lock->w_mutex);
}
