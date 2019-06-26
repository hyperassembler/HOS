#pragma once

#include <kern/cdef.h>
#include <kern/print.h>

void
kprintf(const char *str, ...);

void
kvprintf(const char *str, va_list args);
