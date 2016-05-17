#ifndef _SALLOC_H_
#define _SALLOC_H_

#include "../../../sys/kdef.h"

void salloc_init(void *base, uint32_t size);

void* salloc(void *base, uint32_t size);

void sfree(void *base, void *ptr);

#endif
