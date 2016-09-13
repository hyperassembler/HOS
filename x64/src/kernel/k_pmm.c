#include <k_assert.h>
#include "k_rwwlock.h"
#include "k_status.h"
#include "k_alloc.h"
#include "k_pmm.h"

typedef struct
{
    k_linked_list_node_t free_list_node;
    k_avl_tree_node_t avl_tree_node;
    k_physical_addr_t base;
    int32_t attr;
} k_physical_page_descriptor_t;

static k_avl_tree_t active_tree;
static k_linked_list_t free_list;
static k_rwwlock_t lock;
static _Bool initialized;

/*
 * A comparison function between tree_node and your_node
 * Returns:
 * < 0 if tree_node < your_node
 * = 0 if tree_node == your_node
 * > 0 if tree_node > your_node
 */
static int32_t base_addr_compare(void *tree_node, void *my_node)
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

k_status_t KAPI sx_pmm_init(k_pmm_info_t *info)
{
    if (info == NULL)
    {
        return PMM_STATUS_INVALID_ARGUMENTS;
    }

    if (initialized)
    {
        return STATUS_SUCCESS;
    }

    ke_rwwlock_init(&lock);
    ke_linked_list_init(&free_list);
    ke_avl_tree_init(&active_tree, base_addr_compare);

    for (uint32_t i = 0; i < info->num_of_nodes; i++)
    {
        k_pmm_node_t *each_node = &info->nodes[i];

        ke_assert (each_node->base % K_PAGE_SIZE != 0);

        for (uint64_t j = 0; j <= each_node->size; j++)
        {
            // note that k_alloc function here might trigger page fault
            // however it's fine as long as we don't touch linked list just yet
            // it will use the pages that are already on file to enlarge the kernel heap
            // don't worry, be happy :)
            k_physical_page_descriptor_t *page_info = ke_alloc(sizeof(k_physical_page_descriptor_t));

            if (page_info == NULL)
            {
                return PMM_STATUS_ALLOCATION_FAILED;
            }

            page_info->base = each_node->base;
            ke_linked_list_push_back(&free_list, &page_info->free_list_node);
        }
    }
    initialized = true;
    return STATUS_SUCCESS;
}

// free lists can only be updated at IRQL == DISABLED
// we need to guarantee that on the same CPU, these APIs are not preempted by
// potential callers of these, since timer/interrupts queue DPC, which might trigger
// page fault (kernel heap), therefore, it must set IRQL to DISABLED

k_status_t KAPI ke_alloc_page(k_physical_addr_t *out)
{
    if (!initialized)
    {
        return PMM_STATUS_UNINITIALIZED;
    }

    if (out == NULL)
    {
        return PMM_STATUS_INVALID_ARGUMENTS;
    }

    k_irql_t irql = ke_rwwlock_writer_lock_raise_irql(&lock, K_IRQL_DISABLED_LEVEL);
    k_status_t result = STATUS_SUCCESS;
    k_linked_list_node_t *node = NULL;
    k_physical_page_descriptor_t *page_info = NULL;
    node = ke_linked_list_pop_front(&free_list);
    if (node != NULL)
    {
        page_info = OBTAIN_STRUCT_ADDR(node,
                                       k_physical_page_descriptor_t,
                                       free_list_node);
        ke_avl_tree_insert(&active_tree, &page_info->avl_tree_node);
        *out = page_info->base;
    } else
    {
        result = PMM_STATUS_NOT_ENOUGH_PAGE;
    }

    ke_rwwlock_writer_unlock_lower_irql(&lock, irql);
    return result;
}

k_status_t KAPI ke_query_page_attr(k_physical_addr_t base,
                                   int32_t *out)
{
    if (!initialized)
    {
        return PMM_STATUS_UNINITIALIZED;
    }

    if (out == NULL)
    {
        return PMM_STATUS_INVALID_ARGUMENTS;
    }

    k_irql_t irql = ke_rwwlock_reader_lock_raise_irql(&lock, K_IRQL_DISABLED_LEVEL);
    k_status_t result = STATUS_SUCCESS;
    k_avl_tree_node_t *node = NULL;
    // search for dummy
    k_physical_page_descriptor_t dummy, *page_info = NULL;
    dummy.base = base;

    node = ke_avl_tree_delete(&active_tree, &dummy.avl_tree_node);

    if (node != NULL)
    {
        page_info = OBTAIN_STRUCT_ADDR(node, k_physical_page_descriptor_t, avl_tree_node);
        *out = page_info->attr;
    } else
    {
        result = PMM_STATUS_INVALID_ARGUMENTS;
    }

    ke_rwwlock_reader_unlock_lower_irql(&lock, irql);

    return result;
}

k_status_t KAPI ke_free_page(k_physical_addr_t base)
{
    if (!initialized)
    {
        return PMM_STATUS_UNINITIALIZED;
    }

    // just lock since not sharing with anyone
    k_irql_t irql = ke_rwwlock_writer_lock_raise_irql(&lock, K_IRQL_DISABLED_LEVEL);
    k_status_t result = STATUS_SUCCESS;
    k_avl_tree_node_t *node = NULL;
    // search for dummy
    k_physical_page_descriptor_t dummy, *page_info;
    dummy.base = base;

    node = ke_avl_tree_delete(&active_tree, &dummy.avl_tree_node);
    if (node != NULL)
    {
        page_info = OBTAIN_STRUCT_ADDR(node, k_physical_page_descriptor_t, avl_tree_node);
        ke_linked_list_push_back(&free_list, &page_info->free_list_node);
    } else
    {
        result = PMM_STATUS_INVALID_ARGUMENTS;
    }

    ke_rwwlock_writer_unlock_lower_irql(&lock, irql);

    return result;
}