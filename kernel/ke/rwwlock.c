
#include "kernel/ke/rwwlock.h"

void ke_rwwlock_init(k_rwwlock_t *lock)
{
	if (lock != NULL)
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

void ke_rwwlock_reader_lock(k_rwwlock_t *lock)
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
	return;
}

void ke_rwwlock_reader_unlock(k_rwwlock_t *lock)
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
	return;
}

void ke_rwwlock_writer_lock(k_rwwlock_t *lock)
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

void ke_rwwlock_writer_unlock(k_rwwlock_t *lock)
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

irql_t ke_rwwlock_reader_lock_raise_irql(k_rwwlock_t *lock, irql_t irql)
{
	irql_t old_irql = ke_raise_irql(irql);
	ke_rwwlock_reader_lock(lock);
	return old_irql;
}

void ke_rwwlock_reader_unlock_lower_irql(k_rwwlock_t *lock, irql_t irql)
{
	ke_rwwlock_reader_unlock(lock);
	ke_lower_irql(irql);
	return;
}

irql_t ke_rwwlock_writer_lock_raise_irql(k_rwwlock_t *lock, irql_t irql)
{
	irql_t old_irql = ke_raise_irql(irql);
	ke_rwwlock_writer_lock(lock);
	return old_irql;
}

void ke_rwwlock_writer_unlock_lower_irql(k_rwwlock_t *lock, irql_t irql)
{
	ke_rwwlock_writer_unlock(lock);
	ke_lower_irql(irql);
	return;
}