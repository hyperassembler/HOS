#ifndef _S_ATOMIC_H_
#define _S_ATOMIC_H_
#include "g_abi.h"
#include "g_type.h"

extern int32_t KAPI ke_interlocked_exchange(int32_t *target, int32_t val);

extern int32_t KAPI ke_interlocked_increment(int32_t *target, int32_t val);

extern int32_t KAPI ke_interlocked_compare_exchange(int32_t *target, int32_t compare, int32_t val);

#endif