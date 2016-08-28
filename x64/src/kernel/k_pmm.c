#include "k_alloc.h"
#include "k_bug_check.h"
#include "k_pmm.h"

typedef struct
{
    k_linked_list_node_t free_list_node;
    k_avl_tree_node_t avl_tree_node;
    k_physical_addr_t base;
    //k_physical_page_attr_t attr;
} k_physical_page_descriptor_t;

typedef struct
{
    k_avl_tree_t active_tree;
    k_linked_list_t free_list;
    k_spin_lock_t lock;
    _Bool initialized;
} k_pmm_descriptor_t;

static k_pmm_descriptor_t _pmm_desc;

/*
 * A comparison function between tree_node and your_node
 * Returns:
 * < 0 if tree_node < your_node
 * = 0 if tree_node == your_node
 * > 0 if tree_node > your_node
 */
static int32_t _avl_compare(k_avl_tree_node_t *tree_node, k_avl_tree_node_t *my_node)
{
    k_physical_addr_t tree_base = OBTAIN_STRUCT_ADDR(tree_node,
                                                     k_physical_page_descriptor_t,
                                                     avl_tree_node)->base;
    k_physical_addr_t my_base = OBTAIN_STRUCT_ADDR(my_node,
                                                   k_physical_page_descriptor_t,
                                                   avl_tree_node)->base;
    if (tree_base > my_base)
        return 1;
    else if (tree_base < my_base)
        return -1;
    else
        return 0;
}

int32_t KAPI k_pmm_init(k_pmm_info_t *info)
{
    if (info == NULL || desc == NULL || desc->initialized)
    {
        return PMM_STATUS_INVALID_ARGUMENTS;
    }

    ke_linked_list_init(&desc->free_list);
    ke_avl_tree_init(&desc->active_tree, _avl_compare);
    for (uint32_t i = 0; i < info->num_of_nodes; i++)
    {
        k_pmm_node_t *each_node = &info->nodes[i];

        if (each_node->base % K_PAGE_SIZE != 0)
        {
            // if not aligned, bug check
            return PMM_STATUS_INIT_UNALIGNED;
        }

        for (uint64_t j = 0; j <= each_node->size; j++)
        {
            // note that k_alloc function here might trigger page fault
            // however it's fine as long as we don't touch linked list just yet
            // it will use the pages that are already on file to enlarge the kernel heap
            // don't worry, be happy :)
            k_physical_page_descriptor_t *page_info = k_alloc(sizeof(k_physical_page_descriptor_t));

            if (page_info == NULL)
            {
                return PMM_STATUS_CANNOT_ALLOC_NODE;
            }

            page_info->base = each_node->base;
            ke_linked_list_push_back(&desc->free_list, &page_info->free_list_node);
        }
    }
    desc->initialized = true;
    return PMM_STATUS_SUCCESS;
}

// free lists can only be updated at IRQL == DISABLED
// we need to guarantee that on the same CPU, these APIs are not preempted by
// potential callers of these, since timer/interrupts queue DPC, which might trigger
// page fault (kernel heap), therefore, it must set IRQL to DISABLED

int32_t KAPI k_alloc_page(k_pmm_descriptor_t *desc, k_physical_addr_t *out)
{
    if (desc == NULL || !desc->initialized)
        return PMM_STATUS_INVALID_ARGUMENTS;

    k_irql_t irql = ke_spin_lock_raise_irql(&desc->lock, K_IRQL_DISABLED_LEVEL);
    int32_t result = PMM_STATUS_SUCCESS;
    k_linked_list_node_t *node = NULL;
    k_physical_page_descriptor_t *page_info = NULL;
    node = ke_linked_list_pop_front(&desc->free_list);
    if (node != NULL)
    {
        page_info = OBTAIN_STRUCT_ADDR(node,
                                       k_physical_page_descriptor_t,
                                       free_list_node);
        ke_avl_tree_insert(&desc->active_tree, &page_info->avl_tree_node);
        *out = page_info->base;
    }
    else
    {
        result = PMM_STATUS_NOT_ENOUGH_PAGE;
    }

    ke_spin_unlock_lower_irql(&desc->lock, irql);

    return result;
}

//int32_t KAPI k_query_page(k_pmm_descriptor_t *desc,
//                          k_physical_addr_t base,
//                          k_physical_page_attr_t *out)
//{
//
//    if (desc == NULL || !desc->initialized)
//        return PMM_STATUS_INVALID_ARGUMENTS;
//
//    k_irql_t irql = k_spin_lock_irql_set(&desc->lock, K_IRQL_DISABLED_LEVEL);
//    int32_t result = PMM_STATUS_SUCCESS;
//    avl_tree_node_t *node = NULL;
//    // search for dummy
//    k_physical_page_descriptor_t dummy, *page_info = NULL;
//    dummy.base = base;
//
//    node = avl_tree_delete(&desc->pages_tree, &dummy.avl_tree_node);
//
//    if (node != NULL)
//    {
//        page_info = OBTAIN_STRUCT_ADDR(node, k_physical_page_descriptor_t, avl_tree_node);
//        *out = page_info->attr;
//    }
//    else
//    {
//        result = PMM_STATUS_PAGE_NOT_FOUND;
//    }
//
//    k_spin_unlock_irql_restore(&desc->lock, irql);
//
//    return result;
//}

int32_t KAPI k_free_page(k_pmm_descriptor_t* desc, k_physical_addr_t base)
{
    if (desc == NULL || !desc->initialized)
        return PMM_STATUS_INVALID_ARGUMENTS;

    // just lock since not sharing with anyone
    k_irql_t irql = ke_spin_lock_raise_irql(&desc->lock, K_IRQL_DISABLED_LEVEL);
    int32_t result = PMM_STATUS_SUCCESS;
    k_avl_tree_node_t *node = NULL;
    // search for dummy
    k_physical_page_descriptor_t dummy, *page_info;
    dummy.base = base;

    node = ke_avl_tree_delete(&desc->active_tree, &dummy.avl_tree_node);
    if (node != NULL)
    {
        page_info = OBTAIN_STRUCT_ADDR(node, k_physical_page_descriptor_t, avl_tree_node);
        ke_linked_list_push_back(&desc->free_list, &page_info->free_list_node);
    }
    else
    {
        result = PMM_STATUS_PAGE_NOT_FOUND;
    }

    ke_spin_unlock_lower_irql(&desc->lock, irql);

    return result;
}