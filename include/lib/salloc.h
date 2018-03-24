#ifndef LIB_SALLOC_H
#define LIB_SALLOC_H

#include "type.h"

void SXAPI lb_salloc_init(void *base, uint32 size);

void *SXAPI lb_salloc(void *base, uint32 size);

void SXAPI lb_sfree(void *base, void *ptr);

bool SXAPI lb_salloc_assert(void *base, const uint32 *blk_size, const bool *blk_free, uint32 size);


#endif
