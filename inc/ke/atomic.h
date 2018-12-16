#pragma once

#include "cdef.h"

int32
ke_atomic_xchg_32(int32 *target, int32 val);

int32
ke_atomic_inc_32(int32 *target, int32 increment);

int32
ke_atmoic_cmpxchg_32(int32 *target, int32 compare, int32 val);
