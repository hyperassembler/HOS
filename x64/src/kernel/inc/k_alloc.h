#ifndef _K_ALLOC_H_
#define _K_ALLOC_H_
#include "g_abi.h"
#include "g_type.h"

void KAPI k_alloc_init();

void* KAPI k_alloc(uint32_t size);

void KAPI ke_free(void *ptr);

#endif