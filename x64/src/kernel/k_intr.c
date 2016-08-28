#include <k_assert.h>
#include "k_intr.h"

k_irql_t KAPI ke_raise_irql(k_irql_t irql)
{
    ke_assert(ke_get_irql() <= irql);
    return ke_set_irql(irql);
}

k_irql_t KAPI ke_lower_irql(k_irql_t irql)
{
    ke_assert(ke_get_irql() >= irql);
    return ke_set_irql(irql);
}