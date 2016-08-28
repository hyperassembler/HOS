#ifndef _K_INTR_H_
#define _K_INTR_H_

#include "s_intr.h"
#include "g_type.h"
#include "g_abi.h"

k_irql_t KAPI ke_raise_irql(k_irql_t irql);

k_irql_t KAPI ke_lower_irql(k_irql_t irql);

#endif