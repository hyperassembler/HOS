/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "g_type.h"
#include "g_abi.h"
#include "std_lib.h"

void KAPI mem_cpy(void *src, void *dst, uint64_t size)
{
    if (src == NULL || dst == NULL)
        return;
    char* cSrc = (char*)src;
    char* cDst = (char*)dst;
    while (size--)
        *(cDst++) = *(cSrc++);
    return;
}

void KAPI mem_set(void *src, int8_t const val, uint64_t size)
{
    if (src == NULL)
        return;
    while (size--)
        *((int8_t*)src++) = val;
    return;
}

void KAPI mem_move(void *src, void *dst, uint64_t size)
{
    if (src == NULL || dst == NULL)
        return;
    if (src >= dst)
    {
        return mem_cpy(src, dst, size);
    }
    src += size;
    dst += size;
    while (size--)
        *((char*)--dst) = *((char*)--src);
    return;
}
