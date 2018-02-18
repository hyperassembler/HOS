#ifndef _KERNEL_HAL_INTR_H_
#define _KERNEL_HAL_INTR_H_

#include "type.h"

/**
 * IRQL Definitions
 */
typedef uint32_t irql_t;
#define IRQL_DISABLED_LEVEL (1 << 3)
#define IRQL_DPC_LEVEL (1 << 2)
#define IRQL_APC_LEVEL (1 << 1)
#define IRQL_PASSIVE_LEVEL (1 << 0)

irql_t SXAPI hal_set_irql(irql_t irql);

irql_t SXAPI hal_get_irql(void);

uint32_t SXAPI hal_get_core_id(void);

/**
 * Interrupt Handler Registration
 */
typedef struct
{
	uint32_t timer_intr_vec;
	uint32_t apc_intr_vec;
	uint32_t dpc_intr_vec;
} intr_info_t;

typedef void (SXAPI *intr_handler_t)(void *context, void *intr_stack);

void SXAPI hal_register_interrupt_handler(uint32_t coreid, uint32_t index, intr_handler_t handler, void *context);

void SXAPI hal_deregister_interrupt_handler(uint32_t coreid, uint32_t index);

/**
 * Exception Handler Registration
 */
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

typedef void (SXAPI *exc_handler_t)(uint64_t exc_addr, uint64_t exc_stack, uint64_t error_code);

void SXAPI hal_register_exception_handler(uint32_t coreid, uint32_t index, exc_handler_t handler);

void SXAPI hal_deregister_exception_handler(uint32_t coreid, uint32_t index);

#endif