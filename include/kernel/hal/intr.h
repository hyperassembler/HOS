#ifndef KERNEL_HAL_INTR_H
#define KERNEL_HAL_INTR_H

#include "type.h"

/**
 * IRQL Definitions
 */
typedef uint32 k_irql;
#define IRQL_DISABLED_LEVEL (1 << 3)
#define IRQL_DPC_LEVEL (1 << 2)
#define IRQL_APC_LEVEL (1 << 1)
#define IRQL_PASSIVE_LEVEL (1 << 0)

k_irql SXAPI hal_set_irql(k_irql irql);

k_irql SXAPI hal_get_irql(void);

uint32 SXAPI hal_get_core_id(void);

void SXAPI hal_issue_interrupt(uint32 target_core, uint32 vector);

/**
 * Interrupt Handler Registration
 */
struct intr_info
{
	uint32 timer_intr_vec;
	uint32 apc_intr_vec;
	uint32 dpc_intr_vec;
};

typedef void (SXAPI *intr_handler)(void *context, void *intr_stack);

void SXAPI hal_register_interrupt_handler(uint32 coreid, uint32 index, intr_handler handler, void *context);

void SXAPI hal_deregister_interrupt_handler(uint32 coreid, uint32 index);

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

typedef void (SXAPI *exc_handler)(uint64 exc_addr, uint64 exc_stack, uint64 error_code);

void SXAPI hal_register_exception_handler(uint32 coreid, uint32 index, exc_handler handler);

void SXAPI hal_deregister_exception_handler(uint32 coreid, uint32 index);


#endif
