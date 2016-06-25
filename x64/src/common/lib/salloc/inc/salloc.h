/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _SALLOC_H_
#define _SALLOC_H_

#include "g_abi.h"

void KAPI salloc_init(void *base, uint32_t size);

void* KAPI salloc(void *base, uint32_t size);

void KAPI sfree(void *base, void *ptr);

bool KAPI salloc_assert(void *base, uint32_t blk_size[], bool blk_free[], uint32_t size);


#endif
