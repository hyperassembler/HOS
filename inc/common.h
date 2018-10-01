#pragma once

/**
 * Kernel Memory Layout
 * ----------------------- 0x0000,0000,0000,0000 - User Space
 * Application       SIZE: 0x0000,8000,0000,0000 (256x PML4)
 * ----------------------- 0x0000,8000,0000,0000
 * Non-canonical
 * ----------------------- 0xFFFF,8000,0000,0000 - Kernel Space
 * Reserved          SIZE: 0x0000,7F00,0000,0000 (254x PML4)
 * ----------------------- 0xFFFF,FF00,0000,0000
 * Page Table        SIZE: 0x0000,0080,0000,0000 (1x PML4)
 * ----------------------- 0xFFFF,FF80,0000,0000
 * Kernel Dynamic    SIZE: 0x0000,007F,8000,0000 (Kernel Dynamic + Kernel Image = 1x PML4)
 * ----------------------- 0xFFFF,FFFF,8000,0000
 * Kernel Image      SIZE: 0x0000,0000,8000,0000
 * ----------------------- 0xFFFF,FFFF,FFFF,FFFF
**/

#define KERNEL_IMAGE_PADDR       (0x1000000)
#define KERNEL_PAGE_SIZE         (0x1000)

#define KERNEL_SPACE_VADDR       (0xFFFF800000000000)
#define KERNEL_RESERVED_VADDR    KERNEL_SPACE_VADDR
#define KERNEL_RESERVED_SIZE     (0x00007F0000000000)
#define KERNEL_PAGE_TABLE_VADDR  (0xFFFFFF0000000000)
#define KERNEL_PAGE_TABLE_SIZE   (0x0000008000000000)
#define KERNEL_DYNAMIC_VADDR     (0xFFFFFF8000000000)
#define KERNEL_DYNAMIC_SIZE      (0x0000007F80000000)
#define KERNEL_IMAGE_VADDR       (0xFFFFFFFF80000000)
#define KERNEL_IMAGE_SIZE        (0x0000000080000000)

#ifndef ASM_FILE

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

#define STRUCT_PACKED __attribute__((packed))

#define UNREFERENCED(x) {(x) = (x);}

#define KABI __attribute__((sysv_abi))

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

#endif

