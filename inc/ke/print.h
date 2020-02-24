#pragma once

#include <common/cdef.h>
#include <ke/print.h>

ATTR_FMT_PRINTF int
kprintf(const char *fmt, ...);

int
kvprintf(const char *fmt, va_list args);
