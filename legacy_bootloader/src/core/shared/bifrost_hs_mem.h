#ifndef _BIFROST_HS_MEM_H_
#define _BIFROST_HS_MEM_H_

#include <stdint.h>

// Memory map
typedef struct {
    uintptr_t base;
    uintptr_t size;
    uint32_t attr;
} hw_arch_memory_info_t;

// Caching
#define HW_CACHELINE_SIZE (64)
extern void ke_flush_addr(void *addr, uint32_t num_of_cacheline);

// Atomics
extern int32_t ke_interlocked_exchange(int32_t *addr, int32_t val);
extern int32_t ke_interlocked_compare_exchange(int32_t *addr, int32_t compare, int32_t val);
extern int32_t ke_interlocked_increment(int32_t *addr, int32_t val);
#endif
