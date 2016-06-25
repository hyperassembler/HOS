#include "k_alloc.h"
#include "k_bug_check.h"
#include "k_atomic.h"
#include "k_pmm.h"

static avl_tree_t _active_avl_tree;
static linked_list_t _free_list;
static k_spin_lock_t _lock;
static _Bool _initialized;

/*
 * A comparison function between tree_node and your_node
 * Returns:
 * < 0 if tree_node < your_node
 * = 0 if tree_node == your_node
 * > 0 if tree_node > your_node
 */
static int32_t _avl_compare(avl_tree_node_t *tree_node, avl_tree_node_t *my_node)
{
    k_physical_addr_t tree_base = OBTAIN_STRUCT_ADDR(tree_node, k_physical_page_info_t, avl_tree_node)->base;
    k_physical_addr_t my_base = OBTAIN_STRUCT_ADDR(my_node, k_physical_page_info_t, avl_tree_node)->base;
    if (tree_base > my_base)
        return 1;
    else if (tree_base < my_base)
        return -1;
    else
        return 0;
}

void KAPI k_pmm_init(k_pmm_info_t* info)
{
    if(info != NULL && !_initialized)
    {
        linked_list_init(&_free_list);
        avl_tree_init(&_active_avl_tree, _avl_compare);
        for(int i = 0; i < info->num_of_nodes; i++)
        {
            k_pmm_node_t* each_node = &info->nodes[i];

            if(each_node->base % K_PAGE_SIZE != 0)
            {
                // if not aligned, bug check
                k_bug_check(K_BUG_CHECK_PMM_UNALIGNED);
            }

            for(int j = 0; j <= each_node->size; j++)
            {
                // note that k_alloc function here might trigger page fault
                // however it's fine as long as we don't touch linked list just yet
                // it will use the pages that are already on file to enlarge the kernel heap
                // don't worry, be happy :)
                k_physical_page_info_t* page_info = k_alloc(sizeof(k_physical_page_info_t));
                page_info->base = each_node->base;
                linked_list_push_back(&_free_list, &page_info->free_list_node);
            }
        }
        _initialized = true;
    }
}

// these APIs can only be called at IRQL == DISABLED.
// we need to guarantee that on the same CPU, these APIs are not preempted by
// potential callers of these, since timer/interrupts queue DPC, which might trigger
// page fault (kernel heap), therefore, it must set IRQL to DISABLED

k_physical_addr_t KAPI k_alloc_page()
{
    if(!_initialized)
        return NULL;

    k_irql_t irql = k_spin_lock_irql_set(&_lock, K_IRQL_DISABLED_LEVEL);

    linked_list_node_t *node = NULL;
    k_physical_page_info_t *page_info = NULL;
    k_physical_addr_t base = NULL;
    node = linked_list_pop_front(&_free_list);
    if (node != NULL)
    {
        page_info = OBTAIN_STRUCT_ADDR(node, k_physical_page_info_t, free_list_node);
        base = page_info->base;
        avl_tree_insert(&_active_avl_tree, &page_info->avl_tree_node);
    }

    k_spin_unlock_irql_restore(&_lock, irql);

    return base;
}

k_physical_page_info_t* KAPI k_query_page(k_physical_addr_t base)
{

    if(!_initialized)
        return NULL;

    k_irql_t irql = k_spin_lock_irql_set(&_lock, K_IRQL_DISABLED_LEVEL);

    avl_tree_node_t *node = NULL;
    // search for dummy
    k_physical_page_info_t dummy, *page_info = NULL;
    dummy.base = base;

    node = avl_tree_delete(&_active_avl_tree, &dummy.avl_tree_node);

    if (node != NULL)
    {
        page_info = OBTAIN_STRUCT_ADDR(node, k_physical_page_info_t, avl_tree_node);
    }

    k_spin_unlock_irql_restore(&_lock, irql);

    return page_info;
}

void KAPI k_free_page(k_physical_addr_t base)
{
    if(!_initialized)
        return;

    // just lock since not sharing with anyone
    k_irql_t irql = k_spin_lock_irql_set(&_lock, K_IRQL_DISABLED_LEVEL);

    avl_tree_node_t *node = NULL;
    // search for dummy
    k_physical_page_info_t dummy, *page_info;
    dummy.base = base;

    node = avl_tree_delete(&_active_avl_tree, &dummy.avl_tree_node);
    if (node != NULL)
    {
        page_info = OBTAIN_STRUCT_ADDR(node, k_physical_page_info_t, avl_tree_node);
        linked_list_push_back(&_free_list, &page_info->free_list_node);
    }

    k_spin_unlock_irql_restore(&_lock, irql);

    return;
}