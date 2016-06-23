#ifndef _S_CONTEXT_H_
#define _S_CONTEXT_H_

#include "s_def.h"

// This function should never return and directly context switches to the target
// on x86, it should save the context, switch stack, build exc frame and iret
// This function always assumes interrupt context
extern void KAPI k_context_switch(void* intr_stack, void* cur_context, void* next_context);

#endif