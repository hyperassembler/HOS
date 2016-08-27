#ifndef _BIFROST_HS_CONTEXT_H_
#define _BIFROST_HS_CONTEXT_H_
#include "bifrost_hs_intr.h"

// This function saves intr_context to old_context
// it then switches stack to the new_context and constructs an exception frame on the stack
// it guarantees that it takes
extern void ke_context_switch(void *intr_context, void *old_context, void *new_context);
extern void ke_create_context(void *context, void *pc, void *sp, hw_irql_t irql, void *arg);
#endif
