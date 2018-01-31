/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _LIB_SALLOC_H_
#define _LIB_SALLOC_H_

#include "type.h"

void KABI lb_salloc_init(void *base, uint32_t size);

void* KABI lb_salloc(void *base, uint32_t size);

void KABI lb_sfree(void *base, void *ptr);

bool KABI lb_salloc_assert(void *base, uint32_t *blk_size, bool *blk_free, uint32_t size);


#endif
