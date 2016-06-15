#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#include "s_def.h"
#include "k_intr.h"

typedef struct
{
    uint64_t val;
} k_spin_lock_t;

void KAPI k_spin_lock(k_spin_lock_t *lock);

void KAPI k_spin_unlock(k_spin_lock_t *lock);

k_irql_t KAPI k_spin_lock_irq_set(k_spin_lock_t *lock, k_irql_t irql);

void KAPI k_spin_lock_irq_restore(k_spin_lock_t *lock, k_irql_t irql);

#endif