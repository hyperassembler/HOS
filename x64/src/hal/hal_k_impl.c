#include "hal_arch.h"
#include "s_atomic.h"
#include "s_boot.h"
#include "s_context.h"
#include "s_intr.h"

uint64_t KAPI k_interlocked_exchange(uint64_t* target, uint64_t val)
{
    return hal_interlocked_exchange(target, val);
}

k_irql_t KAPI k_set_irql(k_irql_t irql)
{
    return 0;
}

k_irql_t KAPI k_get_irql()
{
    return 0;
}

void KAPI k_halt_cpu()
{
    hal_halt_cpu();
}