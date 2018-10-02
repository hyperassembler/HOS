#pragma once

#include "hdef.h"

/**
 * ASM declaration
 */

int32 HABI hal_interlocked_exchange_32(int32 *target, int32 val);

int32 HABI hal_interlocked_compare_exchange_32(int32 *dst, int32 test_node_compare, int32 val);

int32 HABI hal_interlocked_increment_32(int32 *target, int32 increment);
