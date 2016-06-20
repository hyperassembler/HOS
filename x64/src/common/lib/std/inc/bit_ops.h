/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _BIT_OPERATION_H_
#define _BIT_OPERATION_H_

#include "s_type.h"
#include "s_def.h"

static inline uint64_t KAPI bit_mask(uint32_t bit)
{
    return (uint64_t)1 << bit;
}

static inline uint64_t KAPI bit_field_mask(uint32_t low, uint32_t high)
{
    return ~(~(uint64_t)0 << high << 1) << low;
}

static inline void KAPI bit_map_set(void* bit_map, uint64_t bit)
{
    if(bit_map != NULL)
    {
        uint64_t quot = bit >> 3;
        uint32_t rmd = (uint32_t)(bit & bit_field_mask(0,2));

        *((uint8_t*)(bit_map) + quot) |= (uint8_t)bit_mask(rmd);
    }
}

static inline void KAPI bit_map_clear(void* bit_map, uint64_t bit)
{
    if(bit_map != NULL)
    {
        uint64_t quot = bit >> 3;
        uint32_t rmd = (uint32_t)(bit & bit_field_mask(0,2));

        *((uint8_t*)(bit_map) + quot) &= ~(uint8_t)bit_mask(rmd);
    }
}

static inline uint32_t KAPI bit_map_read(void* bit_map, uint64_t bit)
{
    if(bit_map != NULL)
    {
        uint64_t quot = bit >> 3;
        uint32_t rmd = (uint32_t)(bit & bit_field_mask(0,2));

        return (*((uint8_t*)(bit_map) + quot) & (uint8_t)bit_mask(rmd)) == 0 ? 0 : 1;
    }
    return 0;
}

#endif
