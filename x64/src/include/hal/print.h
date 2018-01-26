#ifndef _HAL_PRINT_H_
#define _HAL_PRINT_H_

#include "type.h"
#include "kernel/hal/print.h"

void KABI hal_assert(uint32_t expression, char *message);

void KABI hal_printf(const char *str, ...);

void KABI hal_clear_screen();

void KABI hal_print_init();

#endif