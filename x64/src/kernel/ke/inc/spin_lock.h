#ifndef _SPIN_LOCK_H_
#define _SPIN_LOCK_H_

#include "g_type.h"
#include "g_abi.h"
#include "intr.h"

typedef struct
{
    int32_t val;
} k_spin_lock_t;

void KABI ke_spin_lock_init(k_spin_lock_t *lock);

void KABI ke_spin_lock(k_spin_lock_t *lock);

void KABI ke_spin_unlock(k_spin_lock_t *lock);

irql_t KABI ke_spin_lock_raise_irql(k_spin_lock_t *lock, irql_t irql);

void KABI ke_spin_unlock_lower_irql(k_spin_lock_t *lock, irql_t irql);

#endif
