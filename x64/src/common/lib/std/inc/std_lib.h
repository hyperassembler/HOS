/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _STD_LIB_H_
#define _STD_LIB_H_

#include "s_abi.h"
#include "s_type.h"

uint32_t KAPI rand( void );

void KAPI srand(uint32_t _seed );

void KAPI mrand(uint32_t max);

uint64_t KAPI str_len(char const *str);

uint64_t KAPI str_cmp(char const *str1, char const *str2);

void KAPI mem_cpy(void *src, void *dst, uint64_t size);

void KAPI mem_move(void *src, void *dst, uint64_t size);

void KAPI mem_set(void *src, int8_t const val, uint64_t size);

static inline uint64_t KAPI align_down(uint64_t val, uint64_t alignment)
{
    return (val / alignment) * alignment;
}

static inline uint64_t KAPI align_up(uint64_t val, uint64_t alignment)
{
    return ((((val) % (alignment)) == 0) ? (((val) / (alignment)) * (alignment)) : (
            (((val) / (alignment)) * (alignment)) + 1));
}

static inline uint64_t KAPI is_overlap(uint64_t x1, uint64_t x2, uint64_t y1, uint64_t y2)
{
    return ((x1 <= y2) && (y1 <= x2)) ? 1 : 0;
}


static inline int64_t KAPI max_64(int64_t a, int64_t b)
{
    return (a) > (b) ? a : b;
}

static inline int64_t KAPI min_64(int64_t a, int64_t b)
{
    return (a) < (b) ? a : b;
}

static inline int32_t KAPI max_32(int32_t a, int32_t b)
{
    return (a) > (b) ? a : b;
}

static inline int32_t KAPI min_32(int32_t a, int32_t b)
{
    return (a) < (b) ? a : b;
}

static inline uint64_t KAPI round_up_power_of_2(uint64_t num)
{
    num--;
    num |= num >> 1;
    num |= num >> 2;
    num |= num >> 4;
    num |= num >> 8;
    num |= num >> 16;
    num |= num >> 32;
    num++;
    return (uint64_t)num;
}

static inline uint32_t KAPI log_base_2(uint64_t num)
{
    uint32_t result = 0;

    while (num >>= 1)
    {
        result++;
    }

    return result;
}

#define OBTAIN_STRUCT_ADDR(member_addr, struct_name, member_name) ((struct_name*)((void*)(member_addr)-(void*)(&(((struct_name*)0)->member_name))))

#endif