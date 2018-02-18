#ifndef _KERNEL_KE_INTR_H_
#define _KERNEL_KE_INTR_H_

#include "kernel/hal/intr.h"
#include "type.h"

irql_t SXAPI ke_raise_irql(irql_t irql);

irql_t SXAPI ke_lower_irql(irql_t irql);

int SXAPI ke_get_current_core(void);

irql_t SXAPI ke_get_irql(void);

#endif