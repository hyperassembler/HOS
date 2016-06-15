#ifndef _K_ALLOC_H_
#define _K_ALLOC_H_
#include "s_def.h"

void KAPI k_alloc_init();

void* KAPI k_alloc(uint32_t size);

void KAPI k_free(void* ptr);

#endif