#include "g_abi.h"
#include "g_type.h"
#include "k_alloc.h"
#include "k_salloc.h"

#define K_KERNEL_HEAP_SIZE 8192

static _Bool _k_alloc_initialized;
static uint8_t _k_alloc_heap[K_KERNEL_HEAP_SIZE];

void KAPI k_alloc_init()
{
    if (!_k_alloc_initialized)
    {
        ke_salloc_init(_k_alloc_heap, K_KERNEL_HEAP_SIZE);
        _k_alloc_initialized = true;
    }
}

void *KAPI k_alloc(uint32_t size)
{
    return _k_alloc_initialized ? ke_salloc(_k_alloc_heap, size) : NULL;
}

void KAPI ke_free(void *ptr)
{
    if (_k_alloc_initialized)
    {
        ke_sfree(_k_alloc_heap, ptr);
    }
}