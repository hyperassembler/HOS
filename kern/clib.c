
#include <kern/cdef.h>
#include <kern/clib.h>

void
mem_cpy(void *src, void *dst, uint64 size)
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
}

void
mem_set(void *src, uint8 val, uint64 size)
{
    if (src == NULL)
    {
        return;
    }
    while (size--)
    {
        *(uint8 *) src = val;
        src = (void *) ((uintptr) src + 1);
    }
}

void
mem_mv(void *src, void *dst, uint64 size)
{
    if (src == NULL || dst == NULL)
    {
        return;
    }
    if (src >= dst)
    {
        mem_cpy(src, dst, size);
        return;
    }
    src = (void *) ((uintptr) src + size - 1);
    dst = (void *) ((uintptr) dst + size - 1);
    while (size--)
    {
        *(char *) dst = *(char *) src;
        dst = (void *) ((uintptr) dst - 1);
        src = (void *) ((uintptr) src - 1);
    }
}

//
// Random Generator
//
static uint32 seed = 1;
static uint32 max = (uint32)-1;

uint32
krand(void)
{
    seed = seed * 1103512986 + 29865;
    return (unsigned int) (seed / 65536) % (max + 1);
}

void
ksrand(uint32 _seed)
{
    seed = _seed;
}

//
// String Library
//

uint64
str_len(char const *str)
{
    uint64 length = 0;
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

uint64
str_cmp(char const *str1, char const *str2)
{
    if (str1 == NULL || str2 == NULL)
    {
        return 0;
    }
    uint64 length = str_len(str1);
    if (length != str_len(str2))
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
