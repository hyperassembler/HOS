#include "common.h"
#include "kernel/status.h"
#include "kp.h"

static uint32 irql_arr[IRQL_NUM];

k_status
kp_intr_init(struct boot_info *info)
{
    irql_arr[IRQL_HIGH] = info->intr_info.irql_high;
    irql_arr[IRQL_DPC] = info->intr_info.irql_dpc;
    irql_arr[IRQL_LOW] = info->intr_info.irql_low;
    return STATUS_SUCCESS;
}


uint32
ke_raise_irql(uint32 irql)
{
    ke_assert(ke_get_irql() <= irql);
    return hal_set_irql(irql);
}


uint32
ke_lower_irql(uint32 irql)
{
    uint32 old_irql = ke_get_irql();
    ke_assert(old_irql >= irql);
    return hal_set_irql(irql);
}


uint32
ke_get_irql(void)
{
    return hal_get_irql();
}


void
ke_issue_intr(uint32 core, uint32 vector)
{
    hal_issue_intr(core, vector);
}


void
ke_reg_intr(uint32 index, intr_handler_fp handler)
{
    hal_reg_intr(index, handler);
}


void
ke_dereg_intr(uint32 index)
{
    hal_dereg_intr(index);
}


void
ke_reg_exc(uint32 exc, exc_handler_fp handler)
{
    hal_reg_exc(exc, handler);
}


void
ke_dereg_exc(uint32 exc)
{
    hal_dereg_exc(exc);
}


uint32
ke_get_core_id(void)
{
    return hal_get_core_id();
}

