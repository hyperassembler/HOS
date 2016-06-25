#ifndef _K_ATOMIC_H_
#define _K_ATOMIC_H_

#include "g_abi.h"
#include "k_intr.h"
#include "s_atomic.h"

typedef struct
{
    uint64_t val;
} k_spin_lock_t;

void KAPI k_spin_lock(k_spin_lock_t *lock);

void KAPI k_spin_unlock(k_spin_lock_t *lock);

k_irql_t KAPI k_spin_lock_irql_set(k_spin_lock_t *lock, k_irql_t irql);

void KAPI k_spin_unlock_irql_restore(k_spin_lock_t *lock, k_irql_t irql);

#endif