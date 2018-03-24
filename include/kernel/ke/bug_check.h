#ifndef KERNEL_KE_BUG_CHECK_H
#define KERNEL_KE_BUG_CHECK_H

#include "type.h"

#define BUG_CHECK_IRQL_MISMATCH 0
#define BUG_CHECK_PMM_UNALIGNED 1

void SXAPI ke_panic(uint64 reason);

void SXAPI ke_trap(void);

#endif
