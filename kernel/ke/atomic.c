#include "type.h"
#include "kernel/ke/atomic.h"

int32 SXAPI ke_interlocked_exchange_32(int32 *target, int32 val)
{
	return hal_interlocked_exchange_32(target, val);
}

int32 SXAPI ke_interlocked_increment_32(int32 *target, int32 increment)
{
	return hal_interlocked_increment_32(target, increment);
}

int32 SXAPI ke_interlocked_compare_exchange_32(int32 *target, int32 compare, int32 val)
{
	return hal_interlocked_compare_exchange_32(target, compare, val);
}

