#ifndef _INTR_H_
#define _INTR_H_

#include "../../arch/amd64/intr.h"
#include "../../../common/inc/type.h"
#include "../../../common/inc/abi.h"

irql_t KABI ke_raise_irql(irql_t irql);

irql_t KABI ke_lower_irql(irql_t irql);

int KABI ke_get_current_core();

irql_t KABI ke_get_irql();

#endif