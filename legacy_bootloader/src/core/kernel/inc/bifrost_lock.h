#ifndef _BIFROST_LOCK_H_
#define _BIFROST_LOCK_H_

#include "bifrost_intr.h"
#include "bifrost_types.h"

typedef struct
{
    int32_t lock;
} hw_spin_lock_t;

#define HW_LOCK_INITIALIZER {0}

void ke_spin_lock_init(hw_spin_lock_t *lock);

hw_irql_t ke_spin_lock_raise_irql(hw_spin_lock_t *lock, hw_irql_t irql);

void ke_spin_unlock_lower_irql(hw_spin_lock_t *lock, hw_irql_t irql);

void ke_spin_lock(hw_spin_lock_t *lock);

void ke_spin_unlock(hw_spin_lock_t *lock);

#endif
