#include "type.h"
#include "kernel/ke/alloc.h"
#include "lib/salloc.h"

#define K_KERNEL_HEAP_SIZE 8192

static _Bool alloc_initialized;
static uint8_t alloc_heap[K_KERNEL_HEAP_SIZE];

void KABI ke_alloc_init()
{
    if (!alloc_initialized)
    {
        lb_salloc_init(alloc_heap, K_KERNEL_HEAP_SIZE);
        alloc_initialized = true;
    }
}

void *KABI ke_alloc(uint32_t size)
{
    return alloc_initialized ? lb_salloc(alloc_heap, size) : NULL;
}

void KABI ke_free(void *ptr)
{
    if (alloc_initialized)
    {
        lb_sfree(alloc_heap, ptr);
    }
}