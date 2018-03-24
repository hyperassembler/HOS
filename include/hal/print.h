#ifndef HAL_PRINT_H
#define HAL_PRINT_H

#include "type.h"
#include "kernel/hal/print.h"

void SXAPI hal_assert(uint32 expression, char *message);

void SXAPI hal_printf(const char *str, ...);

void SXAPI hal_clear_screen(void);

void SXAPI hal_print_init(void);

#endif
