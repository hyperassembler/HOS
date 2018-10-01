#pragma once
#include "common.h"
#include "print.h"

void
hal_assert(uint32 expression, char *message);

void
hal_printf(const char *str, ...);

void
hal_clear_screen(void);

void
hal_print_init(void);
