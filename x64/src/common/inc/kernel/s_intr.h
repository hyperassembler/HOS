#ifndef _S_INTR_H_
#define _S_INTR_H_

#include "s_abi.h"

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
typedef uint64_t k_irql_t;
#define K_IRQL_HIGH 4
#define K_IRQL_IO 3
#define K_IRQL_DPC 2
#define K_IRQL_APC 1
#define K_IRQL_LOW 0

extern KAPI k_irql_t k_raise_irql(k_irql_t irql);

extern KAPI k_irql_t k_lower_irql(k_irql_t irql);

extern KAPI k_irql_t k_get_irql();

// Interrupt handler registration
// context is a parameter passed by the kernel. HAL must pass back.
// intr_stack is a parameter passed by the HAL. Used by some HAL interrupt context functions.
typedef void ( KAPI *k_intr_handler_t)(void *context, void *intr_stack);

extern void KAPI k_register_intr_handler(uint32_t index, k_intr_handler_t handler);

extern k_intr_handler_t KAPI k_deregister_intr_handler(uint32_t index);

// Exception handler registration
typedef void ( KAPI *k_exc_handler_t)(uint64_t exc_addr, uint64_t exc_stack, uint64_t error_code);

extern void KAPI k_register_exc_handler(k_exc_type_t type, k_exc_handler_t handler);

extern k_exc_handler_t KAPI k_deregister_exc_handler(uint64_t index);

#endif