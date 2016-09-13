#ifndef _K_RWLOCK_H_
#define _K_RWLOCK_H_
#include "k_spin_lock.h"
#include "g_type.h"

typedef struct
{
    k_spin_lock_t w_mutex;
    k_spin_lock_t r_mutex;
    k_spin_lock_t res_lock;
    k_spin_lock_t r_try;
    uint32_t reader_ct;
    uint32_t writer_ct;
} k_rwwlock_t;

void ke_rwwlock_init(k_rwwlock_t *lock);

void ke_rwwlock_reader_lock(k_rwwlock_t *lock);

void ke_rwwlock_reader_unlock(k_rwwlock_t *lock);

void ke_rwwlock_writer_lock(k_rwwlock_t *lock);

void ke_rwwlock_writer_unlock(k_rwwlock_t *lock);

k_irql_t ke_rwwlock_reader_lock_raise_irql(k_rwwlock_t *lock, k_irql_t irql);

void ke_rwwlock_reader_unlock_lower_irql(k_rwwlock_t *lock, k_irql_t irql);

k_irql_t ke_rwwlock_writer_lock_raise_irql(k_rwwlock_t *lock, k_irql_t irql);

void ke_rwwlock_writer_unlock_lower_irql(k_rwwlock_t *lock, k_irql_t irql);

#endif
