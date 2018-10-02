#pragma once

#include "cdef.h"

int32 KABI
hal_atomic_xchg_32(int32 *target, int32 val);

int32 KABI
hal_atomic_inc_32(int32 *target, int32 increment);

int32 KABI
hal_atomic_cmpxchg_32(int32 *target, int32 compare, int32 val);

uint32 KABI
hal_set_irql(uint32 irql);

uint32 KABI
hal_get_irql(void);

void KABI
hal_halt(void);

void KABI
hal_issue_intr(uint32 core, uint32 vector);

typedef void (KABI *k_intr_dispatcher)(uint32 intr_vec, void *h_context);

void KABI
hal_set_intr_dispatcher(k_intr_dispatcher handler);

typedef void (KABI *k_exc_dispatcher)(uint32 exc_vec, uintptr exc_addr, uint32 err_code, void *h_context);

void KABI
hal_set_exc_dispatcher(k_exc_dispatcher handler);

uint32 KABI
hal_get_core_id(void);

