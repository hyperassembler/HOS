#include "common.h"
#include "kernel/ke.h"
#include "hal_export.h"

/**
 * Bogus implementation of HAL
 */
int32 KABI
hal_atomic_xchg_32(int32 *target, int32 val)
{
    return 0;
}


int32 KABI
hal_atomic_inc_32(int32 *target, int32 increment)
{
    return 0;
}

int32 KABI
hal_atomic_cmpxchg_32(int32 *target, int32 compare, int32 val)
{
    return 0;
}

uint32 KABI
hal_set_irql(uint32 irql)
{
    return 0;
}

uint32 KABI
hal_get_irql(void)
{
    return 0;
}

void KABI
hal_issue_intr(uint32 core, uint32 vector)
{
}

void KABI
hal_reg_intr(uint32 index, intr_handler_fp handler)
{
}

void KABI
hal_dereg_intr(uint32 index)
{
}

void KABI
hal_reg_exc(uint32 exc, exc_handler_fp handler)
{
}

void KABI
hal_dereg_exc(uint32 exc)
{
}

uint32 KABI
hal_get_core_id(void)
{
    return 0;
}