#ifndef _KERNEL_KE_BUG_CHECK_H_
#define _KERNEL_KE_BUG_CHECK_H_

#include "type.h"

#define BUG_CHECK_IRQL_MISMATCH 0
#define BUG_CHECK_PMM_UNALIGNED 1

void KABI ke_panic(uint64_t reason);

void KABI ke_trap(void);

#endif