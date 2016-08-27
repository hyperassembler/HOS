#ifndef _BIFROST_INTR_H
#define _BIFROST_INTR_H

#include "bifrost_hs_intr.h"


uint32_t ke_get_system_tick();
hw_irql_t ke_raise_irql(hw_irql_t irql);
hw_irql_t ke_lower_irql(hw_irql_t irql);

void ke_timer_interrupt_handler(void *intr_stack, void *usr_context);

#endif // _BIFROST_INTR_H

