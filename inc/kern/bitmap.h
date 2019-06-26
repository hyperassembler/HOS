#pragma once
#include <kern/cdef.h>

static inline uint8* bit_byte(void* base, uint32 bit)
{
    return (uint8 *) ((uintptr) (base) + (bit & (~(sizeof(uint8) - 1))));
}

static inline uint8 bit_in_byte(uint32 bit)
{
    return bit & (sizeof(uint8) - 1);
}

static inline uint8 bit_read(void *base, uint32 bit)
{
    uint8* target = bit_byte(base, bit);
    return (uint8)((*target >> bit_in_byte(bit)) & 1);
}

static inline void bit_set(void *base, uint32 bit, uint8 val)
{
    uint8* target = bit_byte(base, bit);
    *target = *target & (uint8)(~((val & 1) << bit_in_byte(bit)));
}
