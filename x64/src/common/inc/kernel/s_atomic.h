#ifndef _S_ATOMIC_H_
#define _S_ATOMIC_H_
#include "s_abi.h"
#include "s_type.h"

extern uint64_t KAPI k_interlocked_exchange(uint64_t* target, uint64_t val);

#endif