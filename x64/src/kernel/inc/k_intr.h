#ifndef _K_INTR_H_
#define _K_INTR_H_
#include "k_type.h"
#include "k_def.h"

typedef uint64_t k_irql_t;

#define INTR_VEC_DPC 1
#define INVR_VEC_APC 2
#define INVR_VEC_TIMER 3

#define IRQL_DISABLED 0
#define IRQL_DPC 1
#define IRQL_APC 2
#define IRQL_USER 3

void KAPI k_set_interrupt_handler(uint64_t index, void (*handler)(void));

void KAPI k_disable_interrupt();

void KAPI k_enable_interrupt();


#endif