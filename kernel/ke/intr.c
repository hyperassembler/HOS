#include "cdef.h"
#include "status.h"
#include "intrp.h"
#include "ke/assert.h"

uint32
ke_raise_irql(uint32 irql)
{
    KE_ASSERT(ke_get_irql() <= irql);
    return hal_set_irql(irql);
}


uint32
ke_lower_irql(uint32 irql)
{
    uint32 old_irql = ke_get_irql();
    KE_ASSERT(old_irql >= irql);
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
ke_reg_intr(uint32 vec, k_intr_handler handler)
{
    // TODO: implement kernel dispatch table
    UNREFERENCED(vec);
    UNREFERENCED(handler);
}


void
ke_dereg_intr(uint32 vec)
{
    // TODO: implement kernel dispatch table
    UNREFERENCED(vec);
}


void
ke_reg_exc(uint32 vec, k_exc_handler handler)
{
    // TODO: implement kernel dispatch table
    UNREFERENCED(vec);
    UNREFERENCED(handler);
}


void
ke_dereg_exc(uint32 vec)
{
    // TODO: implement kernel dispatch table
    UNREFERENCED(vec);
}


uint32
ke_get_core_id(void)
{
    return hal_get_core_id();
}


k_status
kp_intr_init(struct boot_info *info)
{
    // TODO: initialize kernel dispatch table
    UNREFERENCED(info);
    return STATUS_INVALID_ARGS;
}


