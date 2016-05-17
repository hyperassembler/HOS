#ifndef _KDEF_H_
#define _KDEF_H_

#include <stddef.h>
#include <stdarg.h>
#include "type.h"

#define SAPI __attribute__((sysv_abi))
#define UAPI __attribute__((sysv_abi))

static inline uint64_t align_down(uint64_t val, uint64_t alignment)
{
    return (val / alignment) * alignment;
}

static inline uint64_t align_up(uint64_t val, uint64_t alignment)
{
    return ((((val) % (alignment)) == 0) ? (((val) / (alignment)) * (alignment)) : ((((val) / (alignment)) * (alignment)) + 1));
}

static inline uint64_t is_overlap(uint64_t x1, uint64_t x2, uint64_t y1, uint64_t y2)
{
    return ((x1 <= y2) && (y1 <= x2)) ? 1 : 0;
}

static inline int64_t max_64(int64_t a, int64_t b)
{
    return (a) > (b) ? a : b;
}

static inline int64_t min_64(int64_t a, int64_t b)
{
    return (a) < (b) ? a : b;
}

static inline int32_t max_32(int32_t a, int32_t b)
{
    return (a) > (b) ? a : b;
}

static inline int32_t min_32(int32_t a, int32_t b)
{
    return (a) < (b) ? a : b;
}

static inline uint32_t seg_selector(uint32_t index, uint32_t rpl)
{
    return (index << 3) + rpl;
}

#define OBTAIN_STRUCT_ADDR(member_addr, member_name, struct_name) ((struct_name*)((char*)(member_addr)-(uint64_t)(&(((struct_name*)0)->member_name))))

#endif
