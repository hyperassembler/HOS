#pragma once

#include <common/cdef.h>
#include <ke/spin_lock.h>
#include <common/list.h>

typedef uintptr mm_paddr;

struct mm_page {
    struct ke_spin_lock page_lock; // page lock
    mm_paddr phys_addr; // physical address of the page
    uint8 phys_order; // order of the page in the buddy allocator
    uint8 phys_pool; // which pool it belongs to in the buddy allocator
    struct list_entry phys_flist_ent; // list entry for the free list in the buddy allocator
};

#define MM_PHYS_ORDER_MAX (10)
#define MM_PHYS_ORDER_FREE (MM_PAGE_ORDER_MAX + 1)

#define MM_PHYS_POOL_DMA (0)
#define MM_PHYS_POOL_GENERIC (1)
#define MM_PHYS_MAX_POOLS (MM_PHYS_POOL_DMA + 1)

void
mm_phys_add_phys_seg(mm_paddr start, usize len);

void
mm_phys_add_reserved_seg(mm_paddr start, usize len);
