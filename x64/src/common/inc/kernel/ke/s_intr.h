#ifndef _S_INTR_H_
#define _S_INTR_H_

#include "g_abi.h"
#include "g_type.h"

typedef struct
{
    uint32_t timer_intr_vec;
    uint32_t apc_intr_vec;
    uint32_t dpc_intr_vec;
} intr_info_t;

typedef enum
{
    unrecoverable_exc,
    div_by_zero_exc,
    general_protection_exc,
    invalid_op_exc,
    page_fault_exc,
    unsupported_exc,
    debug_exc
} exc_type_t;

// IRQL APIs
typedef uint32_t irql_t;
#define K_IRQL_DISABLED_LEVEL (1 << 3)
#define K_IRQL_DPC_LEVEL (1 << 2)
#define K_IRQL_APC_LEVEL (1 << 1)
#define K_IRQL_PASSIVE_LEVEL (1 << 0)

//
// interrupt functions
//
extern void KABI hal_disable_interrupt(void);

extern void KABI hal_enable_interrupt(void);

extern void KABI hal_set_timer_timeout(uint64_t millis);

extern void KABI hal_halt_cpu(void);

extern int32_t KABI hal_get_current_core(void);

extern void KABI hal_set_irql(irql_t irql);

extern irql_t KABI hal_get_irql(void);

extern void KABI hal_issue_interrupt(uint32_t core_id, uint32_t vector);

//
// Interrupt handler registration
//
// context is a parameter passed by the kernel. HAL must pass back.
// intr_stack is a parameter passed by the HAL. Used by some HAL interrupt context functions.
//
typedef void ( KABI *k_intr_handler_t)(void *context, void *intr_stack);

extern void KABI hal_register_intr_handler(uint32_t index, k_intr_handler_t handler, void *context);

extern k_intr_handler_t KABI hal_deregister_intr_handler(uint32_t index);

//
// Exception handler registration
//
typedef void ( KABI *k_exc_handler_t)(uint64_t exc_addr, uint64_t exc_stack, uint64_t error_code);

extern void KABI ke_register_exc_handler(exc_type_t type, k_exc_handler_t handler);

extern k_exc_handler_t KABI ke_deregister_exc_handler(uint64_t index);

#endif