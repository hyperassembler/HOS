#ifndef _S_INTR_H_
#define _S_INTR_H_

typedef enum
{
    K_INTR_TIMER,
    K_INTR_IO,
    K_INTR_SOFTWARE,
    K_INTR_NMI,
    K_EXC_DIV_BY_ZERO,
    K_EXC_MEM_ACCESS,
    K_EXC_INV_OP,
    K_EXC_GP
} k_handler_type_t;

#endif