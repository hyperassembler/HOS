#pragma once

#include "cdef.h"

void
ke_alloc_init(void);

void *
ke_alloc(uint32 size);

void
ke_free(void *ptr);
