#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include "xtruntime-frames-custom.h"
#include "bifrost_types.h"
#include "bifrost_hs_intr.h"

#define TIMER_INTR_VEC HW_INT_TIMER_2 // 2
#define APC_INTR_VEC HW_INT_SW_5      // 21
#define DPC_INTR_VEC HW_INT_SW_10     // 26
#define PROC_FREQUENCY_MHZ (50)

int32_t arch_interrupt_init(hw_arch_intr_info_t* intr_info);

extern "C" uint32_t arch_set_irql(uint32_t irql);

extern "C" uint32_t arch_get_irql();

void arch_set_timer_timeout(uint32_t timeout);

hw_intr_handler_t arch_register_intr_handler(uint32_t vec, hw_intr_handler_t handler, void* context);

void arch_register_exc_handler(uint32_t vec, hw_exc_handler_t handler);

void arch_context_switch(UserFrame* intr_context, UserFrame* prev_context, UserFrame* next_context);

void arch_trigger_interrupt(uint32_t core, uint32_t vec);

void arch_suspend();

#endif
