#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#include "bifrost_types.h"

extern "C" int32_t arch_interlocked_compare_exchange(int32_t* addr, int32_t compare, int32_t val);

#endif
