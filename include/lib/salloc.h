#ifndef _LIB_SALLOC_H_
#define _LIB_SALLOC_H_

#include "type.h"

void SXAPI lb_salloc_init(void *base, uint32_t size);

void *SXAPI lb_salloc(void *base, uint32_t size);

void SXAPI lb_sfree(void *base, void *ptr);

bool SXAPI lb_salloc_assert(void *base, uint32_t *blk_size, bool *blk_free, uint32_t size);


#endif
