#ifndef _S_ATOMIC_H_
#define _S_ATOMIC_H_
#include "abi.h"
#include "type.h"

extern int32_t KABI hal_interlocked_exchange_32(int32_t *target, int32_t val);

extern int32_t KABI hal_interlocked_increment_32(int32_t *target, int32_t increment);

extern int32_t KABI hal_interlocked_compare_exchange_32(int32_t *target, int32_t compare, int32_t val);

#endif