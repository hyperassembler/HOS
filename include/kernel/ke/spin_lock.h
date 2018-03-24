#ifndef KERNEL_KE_SPIN_LOCK_H
#define KERNEL_KE_SPIN_LOCK_H

#include "type.h"
#include "kernel/ke/intr.h"

typedef struct
{
	int32 val;
} k_spin_lock_t;

void SXAPI ke_spin_lock_init(k_spin_lock_t *lock);

void SXAPI ke_spin_lock(k_spin_lock_t *lock);

void SXAPI ke_spin_unlock(k_spin_lock_t *lock);

k_irql SXAPI ke_spin_lock_raise_irql(k_spin_lock_t *lock, k_irql irql);

void SXAPI ke_spin_unlock_lower_irql(k_spin_lock_t *lock, k_irql irql);

#endif

