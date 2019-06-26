#pragma once
#include <kern/cdef.h>
#include <arch/print.h>

void
arch_printf(const char *format, ...);

void
arch_cls(void);

void
arch_print_init(void);

void
arch_vprintf(char const *format, va_list args);
