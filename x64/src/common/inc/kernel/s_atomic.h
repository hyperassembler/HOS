#ifndef _S_ATOMIC_H_
#define _S_ATOMIC_H_
#include "g_abi.h"
#include "g_type.h"

extern uint64_t KAPI k_interlocked_exchange(uint64_t* target, uint64_t val);

extern void KAPI k_interlocked_increment(uint64_t* target);

#endif