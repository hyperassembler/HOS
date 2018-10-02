#include "hal.h"
#include "cpu.h"
#include "intr.h"
#include "io.h"
#include "mem.h"
#include "atomic.h"

/**
 * Provides implementations to HAL functions
 * required by kernel in hal.h
 */

int32 KABI
hal_atomic_xchg_32(int32 *target, int32 val)
{
    return hal_interlocked_exchange_32(target, val);
}

int32 KABI
hal_atomic_inc_32(int32 *target, int32 increment)
{
    return hal_interlocked_increment_32(target, increment);
}

int32 KABI
hal_atomic_cmpxchg_32(int32 *target, int32 compare, int32 val)
{
    return hal_interlocked_compare_exchange_32(target, compare, val);
}

uint32 KABI
hal_set_irql(uint32 irql)
{
    return impl_hal_set_irql(irql);
}

uint32 KABI
hal_get_irql(void)
{
    return impl_hal_get_irql();
}

void KABI
hal_halt(void)
{
    hal_halt_cpu();
}

void KABI
hal_issue_intr(uint32 core, uint32 vector)
{
    impl_hal_issue_intr(core, vector);
}

void KABI
hal_set_intr_dispatcher(k_intr_dispatcher handler)
{
    impl_hal_set_intr_dispatcher(handler);
}

void KABI
hal_set_exc_dispatcher(k_exc_dispatcher handler)
{
    impl_hal_set_exc_dispatcher(handler);
}

uint32 KABI
hal_get_core_id(void)
{
    return impl_hal_get_core_id();
}
