#ifndef _KERNEL_KE_INTR_H_
#define _KERNEL_KE_INTR_H_

#include "kernel/hal/intr.h"
#include "type.h"

irql_t KABI ke_raise_irql(irql_t irql);

irql_t KABI ke_lower_irql(irql_t irql);

int KABI ke_get_current_core(void);

irql_t KABI ke_get_irql(void);

#endif