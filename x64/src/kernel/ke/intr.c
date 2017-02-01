#include <assert.h>
#include "intr.h"

irql_t KABI ke_raise_irql(irql_t irql)
{
    ke_assert(ke_get_irql() <= irql);
    return ke_set_irql(irql);
}

irql_t KABI ke_lower_irql(irql_t irql)
{
    ke_assert(ke_get_irql() >= irql);
    return ke_set_irql(irql);
}