#ifndef _KERNEL_KE_SPIN_LOCK_H_
#define _KERNEL_KE_SPIN_LOCK_H_

#include "type.h"
#include "kernel/ke/intr.h"

typedef struct
{
	int32_t val;
} k_spin_lock_t;

void SXAPI ke_spin_lock_init(k_spin_lock_t *lock);

void SXAPI ke_spin_lock(k_spin_lock_t *lock);

void SXAPI ke_spin_unlock(k_spin_lock_t *lock);

irql_t SXAPI ke_spin_lock_raise_irql(k_spin_lock_t *lock, irql_t irql);

void SXAPI ke_spin_unlock_lower_irql(k_spin_lock_t *lock, irql_t irql);

#endif
