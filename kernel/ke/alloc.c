#include "ke/alloc.h"
#include "lb/salloc.h"

#define K_KERNEL_HEAP_SIZE 8192

static bool alloc_initialized;
static uint8 alloc_heap[K_KERNEL_HEAP_SIZE];

void
ke_alloc_init(void)
{
    if (!alloc_initialized)
    {
        lb_salloc_init(alloc_heap, K_KERNEL_HEAP_SIZE);
        alloc_initialized = TRUE;
    }
}

void *
ke_alloc(uint32 size)
{
    return alloc_initialized ? lb_salloc(alloc_heap, size) : NULL;
}

void
ke_free(void *ptr)
{
    if (alloc_initialized)
    {
        lb_sfree(alloc_heap, ptr);
    }
}
