#pragma once

#include <common/cdef.h>
#include <arch/mlayout.h>
#include <mm/phys.h>

#define ARCH_PMAP_MAX_PHYS_SEGS (64)

// on x86-64 we always keep full direct mapping in the kernel
// maps normal memory (cacheable)
void *
arch_pmap_map(mm_paddr paddr, usize size);

mm_paddr
arch_paddr_to_mm_page(mm_paddr paddr);

// maps device memory (uncacheable)
void *
arch_pmap_mapdev(mm_paddr paddr, usize size);

#define ARCH_VADDR_ATTR_PRESENT (0x1u)
#define ARCH_VADDR_ATTR_UNCACHED (0x2u)
#define ARCH_VADDR_ATTR_READONLY (0x4u)
#define ARCH_VADDR_ATTR_NX (0x8u)

int
arch_map_vaddr(void * base, mm_paddr paddr, uintptr vaddr, usize sz, uint attr);

// module-private functions
#define ARCH_PMAP_TO_PHYS(pmaddr) ((pmaddr) -  ARCH_ML_PMAP_START)
#define ARCH_PHYS_TO_PMAP(paddr) (ARCH_ML_PMAP_START + (paddr))
void
archp_mem_init();

void
archp_mem_addseg(uintptr start, usize len);
