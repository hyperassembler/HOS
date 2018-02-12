#ifndef _KERNEL_MM_PMM_H_
#define _KERNEL_MM_PMM_H_

#include "type.h"
#include "lib/avl_tree.h"
#include "lib/linked_list.h"
#include "kernel/mm/mem.h"
#include "kernel/ke/atomic.h"
#include "status.h"

//#define PMM_PAGE_ATTR_FREE_BIT 0
//#define PMM_PAGE_ATTR_PAGED_BIT 1
//
//typedef struct
//{
//    uint32_t attr;
//} k_physical_page_attr_t;


status_t KABI sx_pmm_init(pmm_info_t *info);

status_t KABI mm_alloc_page(uintptr_t *out);

status_t KABI mm_free_page(uintptr_t base);

status_t KABI mm_query_page_attr(uintptr_t base,
                                 int32_t *out);

// TODO: implement these somehow, i might just reserve the first 16MB for these
int32_t KABI mm_alloc_contiguous_pages(uint64_t num_of_page,
                                       uintptr_t highest_p_addr,
                                       uintptr_t *out);

int32_t KABI mm_free_contiguous_pages(uintptr_t base);

#endif