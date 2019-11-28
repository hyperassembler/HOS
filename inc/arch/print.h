#pragma once
#include <kern/cdef.h>
#include <arch/print.h>

void
arch_cls();

void
arch_print_init();

void
arch_putc(char c);
