#ifndef KERNEL_KE_ATOMIC_H
#define KERNEL_KE_ATOMIC_H

#include "type.h"
#include "kernel/hal/atomic.h"

int32 SXAPI ke_interlocked_exchange_32(int32 *target, int32 val);

int32 SXAPI ke_interlocked_increment_32(int32 *target, int32 increment);

int32 SXAPI ke_interlocked_compare_exchange_32(int32 *target, int32 compare, int32 val);

#endif
