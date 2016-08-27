#include <bifrost_thread.h>
#include <conf_defs.h>
#include "bifrost_intr.h"
#include "bifrost_dpc.h"
#include "bifrost_timer.h"
#include "bifrost_assert.h"

static uint32_t _sys_tick[HW_PROC_CNT] = {0};

uint32_t ke_get_system_tick()
{
    return _sys_tick[ke_get_current_core()];
}


hw_irql_t ke_raise_irql(hw_irql_t irql)
{
    hw_assert(ke_get_irql() <= irql);
    return ke_set_irql(irql);
}

hw_irql_t ke_lower_irql(hw_irql_t irql)
{
    hw_assert(ke_get_irql() >= irql);
    return ke_set_irql(irql);
}

void ke_timer_interrupt_handler(void *intr_stack, void *usr_context)
{
    _sys_tick[ke_get_current_core()]++;
    ke_queue_scheduler_dpc(ke_get_current_core());
    ke_queue_timer_dpc(ke_get_current_core());
    ke_dpc_drain(ke_get_current_core());
    return;
}
