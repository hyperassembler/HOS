#include <hal_intr.h>
#include "hal_arch.h"
#include "s_atomic.h"
#include "s_boot.h"
#include "s_context.h"
#include "s_intr.h"

int32_t KABI ke_interlocked_increment_32(int32_t *target, int32_t increment)
{
    return hal_interlocked_increment_32(target, increment);
}

int32_t KABI ke_interlocked_compare_exchange_32(int32_t *target, int32_t compare, int32_t val)
{
    return hal_interlocked_compare_exchange_32(target, compare, val);
}

int32_t KABI ke_interlocked_exchange_32(int32_t *target, int32_t val)
{
    return hal_interlocked_exchange_32(target, val);
}

irql_t KABI ke_set_irql(irql_t irql)
{
    irql_t old_irql = (irql_t)hal_read_cr8();
    hal_write_cr8((irql_t)irql);
    return old_irql;
}

irql_t KABI ke_get_irql()
{
    return (irql_t)hal_read_cr8();
}

void KABI ke_halt_cpu()
{
    hal_halt_cpu();
}

int32_t KABI ke_get_core_id()
{
    return hal_get_core_id();
}
