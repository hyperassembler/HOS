#include "avl_tree.h"
#include "linked_list.h"
#include "g_abi.h"
#include "s_pmm.h"

typedef struct
{
    // more attributes such as paged/non-paged coming soon
    linked_list_node_t free_list_node;
    avl_tree_node_t avl_tree_node;
    k_physical_addr_t base;
} k_physical_page_info_t;

void KAPI k_pmm_init(k_pmm_info_t* info);

k_physical_addr_t KAPI k_alloc_page();

k_physical_page_info_t* KAPI k_query_page(k_physical_addr_t base);

void KAPI k_free_page(k_physical_addr_t base);

// TODO: implement these somehow, i might just reserve the first 16MB for these
k_physical_addr_t KAPI k_alloc_contiguous_pages(uint64_t num_of_page,
                                                k_physical_addr_t highest_p_addr);

k_physical_addr_t KAPI k_free_contiguous_pages(k_physical_addr_t base);