#ifndef _S_CONTEXT_H_
#define _S_CONTEXT_H_

#include "s_def.h"
#include "s_vmm.h"

// This function should never return and directly context switches to the target
// on x86, it should save the context, switch stack, build exc frame and iret
// This function always assumes interrupt context
extern void KAPI k_context_switch(void *intr_stack,
                                  void *cur_context, void *next_context,
                                  k_address_space_t old_addr_space, k_address_space_t next_addr_space);

extern void KAPI k_create_context(void *context, void *pc, void *sp);

extern void KAPI k_destroy_context(void* context);

#endif