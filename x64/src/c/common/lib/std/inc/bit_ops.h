/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _BIT_OPERATION_H_
#define _BIT_OPERATION_H_

#include "k_type.h"

static inline uint64_t SAPI bit_mask_64(uint32_t bit)
{
    return (uint64_t)1 << bit;
}

static inline uint32_t SAPI bit_mask_32(uint32_t bit)
{
    return (uint32_t)1 << bit;
}

static inline uint64_t SAPI bit_field_mask_64(uint32_t low, uint32_t high)
{
    return ~(~(uint64_t)0 << high << 1) << low;
}

static inline uint32_t SAPI bit_field_mask_32(uint32_t low, uint32_t high)
{
    return ~(~(uint32_t)0 << high << 1) << low;
}

#endif
