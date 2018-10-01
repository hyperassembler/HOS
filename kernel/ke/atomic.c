#include "kp.h"

int32 ke_atomic_xchg_32(int32 *target, int32 val)
{
    return hal_atomic_xchg_32(target, val);
}

int32 ke_atomic_inc_32(int32 *target, int32 increment)
{
    return hal_atomic_inc_32(target, increment);
}

int32 ke_atmoic_cmpxchg_32(int32 *target, int32 compare, int32 val)
{
    return hal_atomic_cmpxchg_32(target, compare, val);
}

