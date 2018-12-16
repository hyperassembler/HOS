#pragma once

#include "kern/cdef.h"

/**
 * Common macros, etc
 */

#define OBTAIN_STRUCT_ADDR(member_addr, struct_name, member_name) ((struct_name*)((uintptr)(member_addr) - (uintptr)(&(((struct_name*)0)->member_name))))

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define SWAP(a, b, T) do { T temp = *(a); *(a) = *(b); *(b) = temp; } while(0);

uint32
lb_rand(void);

void
lb_srand(uint32 _seed);

void
lb_mrand(uint32 max);

uint64
str_len(char const *str);


uint64
str_cmp(char const *str1, char const *str2);


void
mem_cpy(void *src, void *dst, uint64 size);


void
mem_mv(void *src, void *dst, uint64 size);


void
mem_set(void *src, uint8 val, uint64 size);


static inline uint64
bit_mask(uint32 bit)
{
    return (uint64) 1 << bit;
}

static inline uint64
bit_field_mask(uint32 low, uint32 high)
{
    return ~(~(uint64) 0 << high << 1) << low;
}
