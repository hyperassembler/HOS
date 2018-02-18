#ifndef _KERNEL_HAL_PRINT_H_
#define _KERNEL_HAL_PRINT_H_

#include "type.h"

void KABI hal_vprintf(const char *str, va_list args);

#endif