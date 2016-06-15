#include "k_intr.h"
#include "s_hal.h"

int32_t KAPI k_register_interrupt_handler(k_handler_type_t type,
                                          uint32_t priority,
                                          void (*handler)(uint64_t pc, uint64_t sp, uint64_t error))
{
    return hal_register_interrupt_handler(type, priority, handler);
}

void KAPI k_deregister_interrupt_handler(int32_t index)
{
    hal_deregister_interrupt_handler(index);
}

k_irql_t KAPI k_get_current_irql()
{
    return hal_read_interrupt_priority();
}

void KAPI k_set_current_irql(k_irql_t irql)
{
    hal_set_interrupt_priority(irql);
}

void KAPI k_disable_interrupt()
{
    hal_disable_interrupt();
}

void KAPI k_enable_interrupt()
{
    hal_disable_interrupt();
}