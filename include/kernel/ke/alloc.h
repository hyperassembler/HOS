#ifndef _KERNEL_KE_ALLOC_H_
#define _KERNEL_KE_ALLOC_H_

#include "type.h"

void KABI ke_alloc_init(void);

void *KABI ke_alloc(uint32_t size);

void KABI ke_free(void *ptr);

#endif