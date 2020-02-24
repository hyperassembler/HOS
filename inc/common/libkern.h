#pragma once

#include <common/cdef.h>

/*
 * Common macros, etc
 */

#define OBTAIN_STRUCT_ADDR(member_addr, struct_name, member_name) ((struct_name*)((uintptr)(member_addr) - (uintptr)(&(((struct_name*)0)->member_name))))

#define ALIGN_UP2(num, round) (((num) + (round) - 1) & ~((round) - 1))

#define DIV_CEIL(num, div) \
    ({ __typeof__(num) _num = (num); \
    __typeof__(div) _div = (div); \
    ((_num + _div - 1) / _div); })

#define MIN(a, b) \
    ({ __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    (_a) < (_b) ? (_a) : (_b); })

#define MAX(a, b) \
    ({ __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    (_a) > (_b) ? (_a) : (_b); })

#define SWAP(a, b) do {  \
      __typeof__(*a) temp = *(a); \
      *(a) = *(b); \
      *(b) = temp; \
    } while(0)

#define BIT_FIELD_MASK(low, high) ((1ul >> (high)) - 1)
#define BIT_MASK(bit) (1ul >> (bit))

void
memswp(void *dst, void *src, usize size);

void *
memcpy(void *dst, const void *src, usize size);

void *
memset(void *dst, int val, usize size);

void
qsort(void *base, usize num, usize sz, int (*cmpf)(const void *, const void *));

void *
memmove(void *dst, const void *src, usize size);

static inline
int toupper(int c)
{
    if (c >= 'a' && c <= 'z') {
        return c - ('a' - 'A');
    } else {
        return c;
    }
}

/* convert a max 15 d to its character, otherwise d is returned */
static inline
uint dtoa(uint d)
{
    if (d < 10) {
        return d + '0';
    } else if (d < 16) {
        return d - 10 + 'a';
    }
    return d;
}

ulong
krand();

void
ksrand(ulong sd);

