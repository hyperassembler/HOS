/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "g_type.h"
#include "g_abi.h"
#include "sxtdlib.h"

void KABI lb_mem_copy(void *src, void *dst, uint64_t size)
{
    if (src == NULL || dst == NULL)
        return;
    char* cSrc = (char*)src;
    char* cDst = (char*)dst;
    while (size--)
        *(cDst++) = *(cSrc++);
    return;
}

void KABI lb_mem_set(void *src, int8_t const val, uint64_t size)
{
    if (src == NULL)
        return;
    while (size--)
        *((int8_t*)src++) = val;
    return;
}

void KABI lb_mem_move(void *src, void *dst, uint64_t size)
{
    if (src == NULL || dst == NULL)
        return;
    if (src >= dst)
    {
        return lb_mem_copy(src, dst, size);
    }
    src += size;
    dst += size;
    while (size--)
        *((char*)--dst) = *((char*)--src);
    return;
}

//
// Random Generator
//
static uint32_t seed = 1;
static uint32_t max = 16777215;

uint32_t KABI rand( void )
{
    seed = seed * 1103512986 + 29865;
    return (unsigned int)(seed / 65536) % (max+1);
}

void KABI srand( uint32_t _seed )
{
    seed = _seed;
}

void KABI mrand(uint32_t _max)
{
    max = _max;
}

//
// String Library
//

uint64_t KABI lb_str_len(char const *str)
{
    uint64_t length = 0;
    if(str == NULL)
        return 0;
    while(*str != 0)
    {
        str++;
        length++;
    }
    return length;
}

uint64_t KABI rtl_str_cmp(char const *str1, char const *str2)
{
    if(str1 == NULL || str2 == NULL)
        return 0;
    uint64_t length = lb_str_len(str1);
    if(length != lb_str_len(str2))
        return 0;
    while(length--)
    {
        if(*(str1+length) != *(str2+length))
            return 0;
    }
    return 1;
}
