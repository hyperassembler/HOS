#pragma once

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

typedef uint32_t uint32;
typedef int32_t int32;
typedef uint64_t uint64;
typedef int64_t int64;
typedef uintptr_t uintptr;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint8_t uint8;
typedef int8_t int8;

typedef _Bool bool;
#define TRUE (1)
#define FALSE (0)

#define PRAGMA_PACKED __attribute__((packed))

#define PRAGMA_SECTION(x) __attribute__ ((section (x)))

#define PRAGMA_ALIGN(x) __attribute__ ((aligned(x)))

#define UNREFERENCED(x) {(x) = (x);}

#define KABI __attribute__((sysv_abi))


