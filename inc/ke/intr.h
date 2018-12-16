#pragma once

#include "cdef.h"

#define IRQL_LOW (0)
#define IRQL_DPC (1)
#define IRQL_HIGH (2)
#define IRQL_NUM (3)

uint32
ke_raise_irql(uint32 irql);

uint32
ke_lower_irql(uint32 irql);

uint32
ke_get_irql(void);

void
ke_issue_intr(uint32 core, uint32 vector);

typedef void (KABI *k_intr_handler)(void* k_context);

void
ke_reg_intr(uint32 vec, k_intr_handler);

void
ke_dereg_intr(uint32 vec);

#define EXC_UNRCVY (0)
#define EXC_DIV (1)
#define EXC_PROT (2)
#define EXC_OP (3)
#define EXC_PF (4)
#define EXC_UNSUP (5)
#define EXC_DEBUG (6)

typedef void (KABI *k_exc_handler)(uintptr exc_addr, uint64 err_code);

void
ke_reg_exc(uint32 vec, k_exc_handler handler);

void
ke_dereg_exc(uint32 vec);

uint32
ke_get_core_id(void);

