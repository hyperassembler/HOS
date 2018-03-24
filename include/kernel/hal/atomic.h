#ifndef KERNEL_HAL_ATOMIC_H
#define KERNEL_HAL_ATOMIC_H

#include "type.h"

/**
 * Atomic operations
 */

extern int32 SXAPI hal_interlocked_exchange_32(int32 *target, int32 val);

extern int32 SXAPI hal_interlocked_increment_32(int32 *target, int32 increment);

extern int32 SXAPI hal_interlocked_compare_exchange_32(int32 *target, int32 compare, int32 val);

#endif
