#ifndef KERNEL_KE_ALLOC_H
#define KERNEL_KE_ALLOC_H

#include "type.h"

void SXAPI ke_alloc_init(void);

void *SXAPI ke_alloc(uint32 size);

void SXAPI ke_free(void *ptr);

#endif
