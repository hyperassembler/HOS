#pragma once

#include <kern/cdef.h>
#include <kern/print.h>

ATTR_FMT_PRINTF int
kprintf(const char *str, ...);

int
kvprintf(const char *str, va_list args);
