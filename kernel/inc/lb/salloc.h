#pragma once

#include "cdef.h"

void
lb_salloc_init(void *base, uint32 size);

void *
lb_salloc(void *base, uint32 size);

void
lb_sfree(void *base, void *ptr);

bool
lb_salloc_assert(void *base, const uint32 *blk_size, const bool *blk_free, uint32 size);

