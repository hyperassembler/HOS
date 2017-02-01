#ifndef _S_CONTEXT_H_
#define _S_CONTEXT_H_

#include "g_abi.h"
#include "s_vmm.h"
#include "s_intr.h"

// This function should never return and directly context switches to the target
// on x86, it should save the context, switch stack, build exc frame and iret
// This function always assumes interrupt context
extern void KABI ke_context_switch(void *intr_stack,
                                   void *cur_context, void *next_context,
                                   physical_addr_t old_addr_space,
                                   physical_addr_t next_addr_space);

extern void KABI ke_create_context(void *context, void *pc, void *sp, void *ksp, irql_t irql, void *arg);

extern void KABI ke_destroy_context(void *context);

#endif