#ifndef _ALLOC_H_
#define _ALLOC_H_
#include "abi.h"
#include "type.h"

void KABI ke_alloc_init();

void* KABI ke_alloc(uint32_t size);

void KABI ke_free(void *ptr);

#endif