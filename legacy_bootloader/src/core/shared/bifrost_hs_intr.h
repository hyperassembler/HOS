#ifndef _BIFROST_HS_INTR_H_
#define _BIFROST_HS_INTR_H_

#include "bifrost_types.h"

// HAL-specific macros
#define HW_IRQL_USER_LEVEL (0)
#define HW_IRQL_APC_LEVEL (1)
#define HW_IRQL_DPC_LEVEL (2)
#define HW_IRQL_DISABLED_LEVEL (3)

typedef uint32_t hw_irql_t;
extern hw_irql_t ke_get_irql();
extern hw_irql_t ke_set_irql(hw_irql_t irql);

// Interrupt
// user context is used for sharing one intr_handler with multiple interrupts
// it is used by the user to identify which interrupt happened
typedef void (*hw_intr_handler_t)(void * intr_stack, void* usr_context);
typedef void (*hw_exc_handler_t)(uint64_t pc, uint64_t sp, uint64_t error_code);

typedef struct
{
    uint32_t timer_vec;
    uint32_t apc_vec;
    uint32_t dpc_vec;
} hw_arch_intr_info_t;

typedef enum
{
    general_protection_exc,
    page_fault_exc,
    unsupported_thr_fatal_exc,
    unsupported_thr_nonfatal_exc,
    div_by_zero_exc,
    debug_exc,
    unrecoverable_exc,
    invalid_op_exc
} hw_exc_type_t;

extern void ke_trigger_intr(uint32_t core, uint32_t vec);
extern hw_intr_handler_t ke_register_intr_handler(uint32_t vec, hw_intr_handler_t handler, void *context);
extern void ke_register_exc_handler(hw_exc_type_t type, hw_exc_handler_t handler);
extern void ke_set_timer_timeout(uint32_t timeout);

extern uint32_t ke_get_current_core();
#endif
