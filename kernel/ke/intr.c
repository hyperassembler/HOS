#include "kernel/ke/assert.h"
#include "kernel/ke/intr.h"

irql_t SXAPI ke_raise_irql(irql_t irql)
{
	ke_assert(ke_get_irql() <= irql);
	return hal_set_irql(irql);
}

irql_t SXAPI ke_lower_irql(irql_t irql)
{
	irql_t old_irql = ke_get_irql();
	ke_assert(old_irql >= irql);
	return hal_set_irql(irql);
}

irql_t SXAPI ke_get_irql(void)
{
	return hal_get_irql();
}

int SXAPI ke_get_current_core(void)
{
	return hal_get_core_id();
}