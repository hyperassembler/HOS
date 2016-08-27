/*-------------------------------------------------------
 |
 |    context.c
 |
 |    Thread switching functions for 'xtensa' architecture.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2013  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#include "xtruntime-frames-custom.h"
#include "context.h"

UserFrame* switch_to_this[26];

void arch_context_switch(UserFrame* intr_context, UserFrame* prev_context, UserFrame* next_context)
{
    // save context
    *prev_context = *intr_context;
    
#ifdef BEMU
    switch_to_this[xthal_get_prid()] = next_context;
#else
    xthal_window_spill();
    // hardware context switch, it should never comeback
    __asm volatile("mov a2, %0     \n"            \
                 "j _arch_context_switch"
    :: "a" (next_context)
    : "a2");
#endif

}

void arch_create_context(UserFrame* context, void* pc, void* sp, uint32_t irql_mask, void* arg)
{
    context->pc = (uint32_t)pc;
    context->a1 = (uint32_t)sp;
    context->ps = irql_mask | XCHAL_PS_WOE_MASK | XCHAL_PS_UM_MASK | (1 << XCHAL_PS_CALLINC_SHIFT);
    context->a6 = (uint32_t)arg;
    return;
}
