#ifndef _K_INTR_H_
#define _K_INTR_H_

#include "s_intr.h"
#include "s_type.h"
#include "s_def.h"

typedef uint64_t k_irql_t;

int32_t KAPI k_register_interrupt_handler(k_handler_type_t type,
                                          uint32_t priority,
                                          void (*handler)(uint64_t pc, uint64_t sp, uint64_t error));

void KAPI k_deregister_interrupt_handler(int32_t index);

void KAPI k_disable_interrupt();

void KAPI k_enable_interrupt();

k_irql_t KAPI k_get_current_irql();

void KAPI k_set_current_irql(k_irql_t irql);

#endif