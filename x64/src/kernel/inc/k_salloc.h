/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _SALLOC_H_
#define _SALLOC_H_

#include "g_abi.h"
#include "g_type.h"

void KAPI ke_salloc_init(void *base, uint32_t size);

void* KAPI ke_salloc(void *base, uint32_t size);

void KAPI ke_sfree(void *base, void *ptr);

bool KAPI ke_salloc_assert(void *base, uint32_t *blk_size, bool *blk_free, uint32_t size);


#endif
