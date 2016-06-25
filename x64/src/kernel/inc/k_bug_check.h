#ifndef _K_BUG_CHECK_H_
#define _K_BUG_CHECK_H_

#include "g_type.h"
#include "g_abi.h"
#include "k_intr.h"

#define K_BUG_CHECK_IRQL_MISMATCH 0
#define K_BUG_CHECK_PMM_UNALIGNED 1

void KAPI k_bug_check(uint64_t reason);

#endif