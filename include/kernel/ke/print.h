#ifndef KERNEL_KE_PRINT_H
#define KERNEL_KE_PRINT_H

#include "type.h"
#include "kernel/hal/print.h"

void SXAPI ke_printf(const char *str, ...);

void SXAPI ke_vprintf(const char *str, va_list args);

#endif
