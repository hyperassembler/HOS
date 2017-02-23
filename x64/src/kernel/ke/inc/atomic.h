#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#include "g_type.h"
#include "g_abi.h"

int32_t KABI ke_interlocked_exchange_32(int32_t *target, int32_t val);

int32_t KABI ke_interlocked_increment_32(int32_t *target, int32_t increment);

int32_t KABI ke_interlocked_compare_exchange_32(int32_t *target, int32_t compare, int32_t val);

#endif