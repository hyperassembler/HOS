#include "kernel/ke/atomic.h"
#include "kernel/ke/spin_lock.h"

void KABI ke_spin_lock_init(k_spin_lock_t *lock)
{
	if (lock != NULL)
	{
		lock->val = 0;
	}
}

void KABI ke_spin_lock(k_spin_lock_t *lock)
{
	if (lock != NULL)
	{
		while (ke_interlocked_compare_exchange_32(&lock->val, 0, 1) != 0)
		{}
	}
	return;
}

void KABI ke_spin_unlock(k_spin_lock_t *lock)
{
	if (lock != NULL)
	{
		lock->val = 0;
	}
	return;
}

irql_t KABI ke_spin_lock_raise_irql(k_spin_lock_t *lock, irql_t irql)
{
	irql_t prev_irql = ke_get_irql();
	if (lock != NULL)
	{
		ke_raise_irql(irql);
		ke_spin_lock(lock);
	}
	return prev_irql;
}

void KABI ke_spin_unlock_lower_irql(k_spin_lock_t *lock, irql_t irql)
{
	if (lock != NULL)
	{
		ke_spin_unlock(lock);
		ke_lower_irql(irql);
	}
	return;
}