#ifndef _PMM_H_
#define _PMM_H_

#include "avl_tree.h"
#include "linked_list.h"
#include "g_abi.h"
#include "s_pmm.h"
#include "atomic.h"
#include "status.h"

//#define PMM_PAGE_ATTR_FREE_BIT 0
//#define PMM_PAGE_ATTR_PAGED_BIT 1
//
//typedef struct
//{
//    uint32_t attr;
//} k_physical_page_attr_t;

status_t KABI sx_pmm_init(pmm_info_t *info);

status_t KABI mm_alloc_page(physical_addr_t *out);

status_t KABI mm_free_page(physical_addr_t base);

status_t KABI mm_query_page_attr(physical_addr_t base,
                                 int32_t *out);

// TODO: implement these somehow, i might just reserve the first 16MB for these
int32_t KABI mm_alloc_contiguous_pages(uint64_t num_of_page,
                                       physical_addr_t highest_p_addr,
                                       physical_addr_t *out);

int32_t KABI mm_free_contiguous_pages(physical_addr_t base);

#endif