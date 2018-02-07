/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "type.h"
#include "lib/sxtdlib.h"

void KABI lb_mem_copy(void *src, void *dst, uint64_t size)
{
    if (src == NULL || dst == NULL)
    {
        return;
    }
    char *cSrc = (char *) src;
    char *cDst = (char *) dst;
    while (size--)
    {
        *(cDst++) = *(cSrc++);
    }
    return;
}

void KABI lb_mem_set(void *src, uint8_t const val, uint64_t size)
{
    if (src == NULL)
    {
        return;
    }
    while (size--)
    {
        *(uint8_t *)src = val;
        src = (void*)((uintptr_t)src + 1);
    }
    return;
}

void KABI lb_mem_move(void *src, void *dst, uint64_t size)
{
    if (src == NULL || dst == NULL)
    {
        return;
    }
    if (src >= dst)
    {
        lb_mem_copy(src, dst, size);
        return;
    }
    src = (void*)((uintptr_t)src + size - 1);
    dst = (void*)((uintptr_t)dst + size - 1);
    while (size--)
    {
        *(char*)dst = *(char*)src;
        dst = (void*)((uintptr_t)dst - 1);
        src = (void*)((uintptr_t)src - 1);
    }
    return;
}

//
// Random Generator
//
static uint32_t seed = 1;
static uint32_t max = 16777215;

uint32_t KABI lb_rand(void)
{
    seed = seed * 1103512986 + 29865;
    return (unsigned int) (seed / 65536) % (max + 1);
}

void KABI lb_srand(uint32_t _seed)
{
    seed = _seed;
}

void KABI lb_mrand(uint32_t _max)
{
    max = _max;
}

//
// String Library
//

uint64_t KABI lb_str_len(char const *str)
{
    uint64_t length = 0;
    if (str == NULL)
    {
        return 0;
    }
    while (*str != 0)
    {
        str++;
        length++;
    }
    return length;
}

uint64_t KABI lb_str_cmp(char const *str1, char const *str2)
{
    if (str1 == NULL || str2 == NULL)
    {
        return 0;
    }
    uint64_t length = lb_str_len(str1);
    if (length != lb_str_len(str2))
    {
        return 0;
    }
    while (length--)
    {
        if (*(str1 + length) != *(str2 + length))
        {
            return 0;
        }
    }
    return 1;
}
