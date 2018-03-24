#ifndef KERNEL_KE_INTR_H
#define KERNEL_KE_INTR_H

#include "kernel/hal/intr.h"
#include "type.h"

k_irql SXAPI ke_raise_irql(k_irql irql);

k_irql SXAPI ke_lower_irql(k_irql irql);

uint32 SXAPI ke_get_current_core(void);

k_irql SXAPI ke_get_irql(void);

#endif
