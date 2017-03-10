#ifndef _INTR_H_
#define _INTR_H_

#include "s_intr.h"
#include "g_type.h"
#include "g_abi.h"

irql_t KABI ke_raise_irql(irql_t irql);

irql_t KABI ke_lower_irql(irql_t irql);

int KABI ke_get_current_core();

irql_t KABI ke_get_irql();

#endif