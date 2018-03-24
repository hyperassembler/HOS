#ifndef KERNEL_MM_PMM_H
#define KERNEL_MM_PMM_H

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
//    uint32 attr;
//} k_physical_page_attr_t;


sx_status SXAPI sx_pmm_init(pmm_info_t *info);

sx_status SXAPI mm_alloc_page(uintptr *out);

sx_status SXAPI mm_free_page(uintptr base);

sx_status SXAPI mm_query_page_attr(uintptr base,
                                 int32 *out);

// TODO: implement these somehow, i might just reserve the first 16MB for these
int32 SXAPI mm_alloc_contiguous_pages(uint64 num_of_page,
                                       uintptr highest_p_addr,
                                       uintptr *out);

int32 SXAPI mm_free_contiguous_pages(uintptr base);

#endif
