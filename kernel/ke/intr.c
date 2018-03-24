#include "kernel/ke/assert.h"
#include "kernel/ke/intr.h"

k_irql SXAPI ke_raise_irql(k_irql irql)
{
	ke_assert(ke_get_irql() <= irql);
	return hal_set_irql(irql);
}

k_irql SXAPI ke_lower_irql(k_irql irql)
{
	k_irql old_irql = ke_get_irql();
	ke_assert(old_irql >= irql);
	return hal_set_irql(irql);
}

k_irql SXAPI ke_get_irql(void)
{
	return hal_get_irql();
}

uint32 SXAPI ke_get_current_core(void)
{
	return hal_get_core_id();
}
