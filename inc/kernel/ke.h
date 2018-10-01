#pragma once

#include "common.h"
#include "kernel/status.h"
#include "kernel/lb.h"
#include "hal_export.h"

/**
 * memory
 */
void
ke_alloc_init(void);

void *
ke_alloc(uint32 size);

void
ke_free(void *ptr);


/**
 * atomic
 */
int32
ke_atomic_xchg_32(int32 *target, int32 val);

int32
ke_atomic_inc_32(int32 *target, int32 increment);

int32
ke_atmoic_cmpxchg_32(int32 *target, int32 compare, int32 val);


/**
 * assert
 */
#define ke_assert(expr) ke_assert_ex(#expr, __FILE__, __LINE__, expr)

void
ke_assert_ex(const char *expr_str, const char *file, int32 line, int32 expr);


/**
 * bugcheck
 */
void
ke_panic(uint64 reason);


/**
 * interrupt
 */
#define IRQL_LOW (0)
#define IRQL_DPC (1)
#define IRQL_HIGH (2)
#define IRQL_NUM (3)

uint32
ke_raise_irql(uint32 irql);

uint32
ke_lower_irql(uint32 irql);

uint32
ke_get_irql(void);

void
ke_issue_intr(uint32 core, uint32 vector);

void
ke_reg_intr(uint32 index, intr_handler_fp handler);

void
ke_dereg_intr(uint32 index);

#define EXC_UNRCVY (0)
#define EXC_DIV (1)
#define EXC_PROT (2)
#define EXC_OP (3)
#define EXC_PF (4)
#define EXC_UNSUP (5)
#define EXC_DEBUG (6)
void
ke_reg_exc(uint32 exc, exc_handler_fp handler);

void
ke_dereg_exc(uint32 exc);

uint32
ke_get_core_id(void);


/**
 * print
 */
void
ke_printf(const char *str, ...);

void
ke_vprintf(const char *str, va_list args);


/**
 * spinlock
 */
struct spin_lock
{
    int32 val;
};

void
ke_spin_init(struct spin_lock *lock);

void
ke_spin_lock(struct spin_lock *lock);

void
ke_spin_unlock(struct spin_lock *lock);


/**
 * rwwlock
 */
struct rwwlock
{
    struct spin_lock w_mutex;
    struct spin_lock r_mutex;
    struct spin_lock res_lock;
    struct spin_lock r_try;
    uint32 reader_ct;
    uint32 writer_ct;
};

void
ke_rww_init(struct rwwlock *lock);

void
ke_rww_r_lock(struct rwwlock *lock);

void
ke_rww_r_unlock(struct rwwlock *lock);

void
ke_rww_w_lock(struct rwwlock *lock);

void
ke_rww_w_unlock(struct rwwlock *lock);
