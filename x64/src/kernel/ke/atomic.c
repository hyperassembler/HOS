#include "type.h"
#include "kernel/ke/atomic.h"

int32_t KABI ke_interlocked_exchange_32(int32_t *target, int32_t val)
{
    return hal_interlocked_exchange_32(target, val);
}

int32_t KABI ke_interlocked_increment_32(int32_t *target, int32_t increment)
{
    return hal_interlocked_increment_32(target, increment);
}

int32_t KABI ke_interlocked_compare_exchange_32(int32_t *target, int32_t compare, int32_t val)
{
    return hal_interlocked_compare_exchange_32(target, compare, val);
}
