#ifndef _K_ATOMIC_H_
#define _K_ATOMIC_H_

#include "g_abi.h"
#include "k_intr.h"
#include "s_atomic.h"

typedef struct
{
    int32_t val;
} k_spin_lock_t;

void KAPI ke_spin_lock(k_spin_lock_t *lock);

void KAPI ke_spin_unlock(k_spin_lock_t *lock);

k_irql_t KAPI ke_spin_lock_raise_irql(k_spin_lock_t *lock, k_irql_t irql);

void KAPI ke_spin_unlock_lower_irql(k_spin_lock_t *lock, k_irql_t irql);

#endif