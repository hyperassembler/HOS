#pragma once

#include <common/cdef.h>
#include <ke/spin_lock.h>
#include <common/list.h>

typedef uintptr mm_paddr;

struct mm_phys_page {
    struct ke_spin_lock page_lock; // page lock
    struct list_entry phys_flist_ent; // list entry for the free list in the buddy allocator
    uint8 order; // order of the page in the buddy allocator
    uint8 free;
} ATTR_ALIGN(8);

// maximum allocated page sz = 2^10 * 4096
#define MM_PHYS_ORDER_MAX (10)
#define MM_PHYS_ORDER_FREE (MM_PAGE_ORDER_MAX + 1)

// DMA ZONE, 0 - 16MB
#define MM_PHYS_ARENA_DMA (0)
#define MM_PHYS_ARENA_MAX_ADDR (16 * 1024 * 1024)

// GENERIC ZONE
#define MM_PHYS_ARENA_GENERIC (1)
#define MM_PHYS_MAX_POOLS (MM_PHYS_POOL_DMA + 1)

void
mm_phys_add_phys_seg(mm_paddr start, usize len);
