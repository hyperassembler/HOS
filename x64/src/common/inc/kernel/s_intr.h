#ifndef _S_INTR_H_
#define _S_INTR_H_

#include "g_abi.h"
#include "g_type.h"

typedef struct
{
    uint32_t timer_intr_vec;
    uint32_t apc_intr_vec;
    uint32_t dpc_intr_vec;
} k_hal_intr_info_t;

typedef enum
{
    unrecoverable_exc,
    div_by_zero_exc,
    general_protection_exc,
    invalid_op_exc,
    page_fault_exc,
    unsupported_exc,
    debug_exc
} k_exc_type_t;

// IRQL APIs
typedef uint32_t k_irql_t;
#define K_IRQL_DISABLED_LEVEL 15
#define K_IRQL_DPC_LEVEL 4
#define K_IRQL_APC_LEVEL 2
#define K_IRQL_PASSIVE_LEVEL 0

extern k_irql_t KAPI  ke_set_irql(k_irql_t irql);

extern k_irql_t KAPI  ke_get_irql();

extern void KAPI ke_halt_cpu();

extern void KAPI ke_set_timer_timeout(uint64_t timeout);

extern int32_t KAPI ke_get_core_id();

extern int32_t KAPI ke_issue_interrupt(int32_t core_id, uint32_t vector);

// Interrupt handler registration
// context is a parameter passed by the kernel. HAL must pass back.
// intr_stack is a parameter passed by the HAL. Used by some HAL interrupt context functions.
typedef void ( KAPI *k_intr_handler_t)(void *context, void *intr_stack);

extern void KAPI ke_register_intr_handler(uint32_t index, k_intr_handler_t handler, void *context);

extern k_intr_handler_t KAPI ke_deregister_intr_handler(uint32_t index);

// Exception handler registration
typedef void ( KAPI *k_exc_handler_t)(uint64_t exc_addr, uint64_t exc_stack, uint64_t error_code);

extern void KAPI ke_register_exc_handler(k_exc_type_t type, k_exc_handler_t handler);

extern k_exc_handler_t KAPI ke_deregister_exc_handler(uint64_t index);

#endif