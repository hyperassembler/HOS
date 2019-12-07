#pragma once

#include <kern/cdef.h>
#include <kern/print.h>

ATTR_FMT_PRINTF int
kprintf(const char *fmt, ...);

int
kvprintf(const char *fmt, va_list args);
