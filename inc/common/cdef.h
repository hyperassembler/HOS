#pragma once

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdatomic.h>

typedef uintmax_t uintmax;
typedef intmax_t intmax;
typedef uint32_t uint32;
typedef int32_t int32;
typedef uint64_t uint64;
typedef int64_t int64;
typedef uintptr_t uintptr;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint8_t uint8;
typedef int8_t int8;
typedef size_t usize;
typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned short ushort;
#define KABI __attribute__((sysv_abi))

#define STATIC_ASSERT(cond, msg) _Static_assert((cond), msg)
#define TYPEOF(type) __typeof__(type)

/* Declarations */
#define DECL_ATOMIC(type) _Atomic type

/* Attributes */
#define ATTR_PACKED __attribute__((packed))
#define ATTR_UNUSED __attribute__((unused))
#define ATTR_USED __attribute__((used))
#define ATTR_SECTION(x) __attribute__ ((section (#x)))
#define ATTR_ALIGN(x) __attribute__((aligned (x)))
#define ATTR_FMT_PRINTF __attribute__((format (printf, 1, 2)))
#define ATTR_NORETURN _Noreturn


#define BOCHS_BREAK __asm__("xchg %bx, %bx")
