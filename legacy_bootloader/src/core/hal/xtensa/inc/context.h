#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include "xtruntime-frames-custom.h"
#include "bifrost_hs_context.h"

void arch_context_switch(UserFrame* intr_context, UserFrame* prev_context, UserFrame* next_context);

void arch_create_context(UserFrame* context, void* pc, void* sp, uint32_t irql_mask, void* arg);

#endif
