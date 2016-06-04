#ifndef _HAL_ATOMIC_H_
#define _HAL_ATOMIC_H_
#include "k_def.h"
extern uint64_t KAPI hal_interlocked_exchange(uint64_t* dst,
uint64_t val);

#endif