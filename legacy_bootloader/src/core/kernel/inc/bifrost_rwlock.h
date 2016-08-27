/*-------------------------------------------------------
 |
 |      bifrost_rwlock.h
 |
 |      Contains Bifrost readers-writer lock APIs,
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2016  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#ifndef _BIFROST_RWLOCK_H_
#define _BIFROST_RWLOCK_H_
#include "bifrost_lock.h"
#include "bifrost_intr.h"
#include "bifrost_types.h"

typedef struct
{
    hw_spin_lock_t w_mutex;
    hw_spin_lock_t r_mutex;
    hw_spin_lock_t res_lock;
    hw_spin_lock_t r_try;
    uint32_t reader_ct;
    uint32_t writer_ct;
} hw_rwlock_t;

void ke_rwlock_init(hw_rwlock_t *lock);

void ke_reader_lock(hw_rwlock_t *lock);

void ke_reader_unlock(hw_rwlock_t *lock);

hw_irql_t ke_reader_lock_raise_irql(hw_rwlock_t *lock, hw_irql_t irq);

void ke_reader_unlock_lower_irql(hw_rwlock_t *lock, hw_irql_t irq);

void ke_writer_lock(hw_rwlock_t *lock);

void ke_writer_unlock(hw_rwlock_t *lock);

hw_irql_t ke_writer_lock_raise_irql(hw_rwlock_t *lock, hw_irql_t irq);

void ke_writer_unlock_lower_irql(hw_rwlock_t *lock, hw_irql_t irq);

#endif
