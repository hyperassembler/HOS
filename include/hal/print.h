#ifndef _HAL_PRINT_H_
#define _HAL_PRINT_H_

#include "type.h"
#include "kernel/hal/print.h"

void SXAPI hal_assert(uint32_t expression, char *message);

void SXAPI hal_printf(const char *str, ...);

void SXAPI hal_clear_screen(void);

void SXAPI hal_print_init(void);

#endif