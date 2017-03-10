#include "assert.h"
#include "intr.h"

irql_t KABI ke_raise_irql(irql_t irql)
{
    ke_assert(ke_get_irql() <= irql);
    return hal_set_irql(irql);
}

irql_t KABI ke_lower_irql(irql_t irql)
{
    irql_t old_irql = ke_get_irql();
    ke_assert(old_irql >= irql);
    return hal_set_irql(irql);
}

irql_t KABI ke_get_irql()
{
    return hal_get_irql();
}

int KABI ke_get_current_core()
{
    return hal_get_current_core();
}