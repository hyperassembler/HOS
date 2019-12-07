#pragma once

#include <kern/cdef.h>
#include <arch/mlayout.h>

static inline void *
arch_pmap_map(uintptr paddr, ATTR_UNUSED usize size)
{
    return (void*)(paddr + KERN_PMAP_START);
}
