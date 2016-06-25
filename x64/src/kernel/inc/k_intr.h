#ifndef _K_INTR_H_
#define _K_INTR_H_

#include "s_intr.h"
#include "g_type.h"
#include "g_abi.h"

void KAPI k_exc_handler_page_fault(void *context, void *intr_stack);

#endif