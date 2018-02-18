#ifndef _KERNEL_KE_ALLOC_H_
#define _KERNEL_KE_ALLOC_H_

#include "type.h"

void SXAPI ke_alloc_init(void);

void *SXAPI ke_alloc(uint32_t size);

void SXAPI ke_free(void *ptr);

#endif