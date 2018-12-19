#pragma once

#include <ke/cdef.h>

int32 KABI cmp_swp_32(int32* dst, int32 old, int32 val);

int32 KABI fetch_add_32(int32* dst, int32 val);

