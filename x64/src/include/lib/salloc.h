/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _SALLOC_H_
#define _SALLOC_H_

#include "../../common/inc/abi.h"
#include "../../common/inc/type.h"

void KABI lb_salloc_init(void *base, uint32_t size);

void* KABI lb_salloc(void *base, uint32_t size);

void KABI lb_sfree(void *base, void *ptr);

bool KABI lb_salloc_assert(void *base, uint32_t *blk_size, bool *blk_free, uint32_t size);


#endif
