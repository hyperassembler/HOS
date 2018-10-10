#pragma once
#include "cdef.h"
#include "print.h"
#include "multiboot2.h"

void
hal_assert(uint32 expression, char *message);

void
hal_printf(const char *str, ...);

void
hal_clear_screen(void);

void
hal_print_init(struct multiboot_tag_framebuffer* info);
