#include "mm/pmm.h"

#include "lb/atree.h"
#include "lb/dlist.h"
#include "ke/rww_lock.h"
#include "clib.h"
#include "ke/intr.h"

struct phys_page_desc
{
    struct dlist_node free_list_node;
    struct atree_node tree_node;
    uintptr base;
    int32 attr;
};

static struct a_tree active_tree;
static struct dlist free_list;
static struct rww_lock lock;

/*
 * A comparison function between tree_node and your_node
 * Returns:
 * < 0 if tree_node < your_node
 * = 0 if tree_node == your_node
 * > 0 if tree_node > your_node
 */
//static int32 mmp_base_paddr_compare(struct atree_node *tree_node, struct atree_node *my_node)
//{
//    uintptr tree_base = OBTAIN_STRUCT_ADDR(tree_node,
//                                           struct phys_page_desc,
//                                           tree_node)->base;
//    uintptr my_base = OBTAIN_STRUCT_ADDR(my_node,
//                                         struct phys_page_desc,
//                                         tree_node)->base;
//    if (tree_base > my_base)
//    {
//        return 1;
//    }
//    else
//    {
//        if (tree_base < my_base)
//        {
//            return -1;
//        }
//        else
//        {
//            return 0;
//        }
//    }
//}

k_status mm_pmm_init(struct boot_info *info)
{
    UNREFERENCED(info);
//	if (info == NULL)
//	{
//		return MM_INVALID_ARGUMENTS;
//	}
//
//	if (initialized)
//	{
//		return STATUS_SUCCESS;
//	}
//
//	ke_rwwlock_init(&lock);
//	lb_linked_list_init(&free_list);
//	lb_avl_tree_init(&active_tree, mmp_base_paddr_compare);
//
//	for (uint32 i = 0; i < info->num_of_nodes; i++)
//	{
//		pmm_node_t *each_node = &info->nodes[i];
//
//		KE_ASSERT (each_node->base % KERNEL_PAGE_SIZE != 0);
//
//		for (uint64 j = 0; j <= each_node->size; j++)
//		{
//			// note that k_alloc function here might trigger page fault
//			// however it's fine as long as we don't touch linked list just yet
//			// it will use the pages that are already on file to enlarge the kernel heap
//			// don't worry, be happy :)
//			struct phys_page_desc *page_info = ke_alloc(sizeof(struct phys_page_desc));
//
//			if (page_info == NULL)
//			{
//				return MM_ALLOCATION_FAILED;
//			}
//
//			page_info->base = each_node->base;
//			lb_linked_list_push_back(&free_list, &page_info->free_list_node);
//		}
//	}
//	initialized = TRUE;
    return STATUS_INVALID_ARGS;
}

// free lists can only be updated at IRQL == DISABLED
// we need to guarantee that on the same CPU, these APIs are not preempted by
// potential callers of these, since timer/interrupts queue DPC, which might trigger
// page fault (kernel heap), therefore, it must set IRQL to DISABLED

k_status mm_alloc_page(uintptr *out)
{
    uint32 irql;
    k_status status;

    status = STATUS_SUCCESS;

    irql = ke_raise_irql(IRQL_HIGH);
    ke_rww_w_lock(&lock);

    struct dlist_node *node = NULL;
    struct phys_page_desc *page_info = NULL;
    node = lb_llist_pop_front(&free_list);
    if (node != NULL)
    {
        page_info = OBTAIN_STRUCT_ADDR(node,
                                       struct phys_page_desc,
                                       free_list_node);
        lb_atree_insert(&active_tree, &page_info->tree_node);
        *out = page_info->base;
    }
    else
    {
        status = STATUS_NO_MEM;
    }

    ke_rww_w_unlock(&lock);
    ke_lower_irql(irql);
    return status;
}

k_status mm_query_page_attr(uintptr base,
                            int32 *out)
{
    uint32 irql;
    k_status result;
    struct atree_node *node;
    struct phys_page_desc dummy;
    struct phys_page_desc *page_info;

    result = STATUS_SUCCESS;
    node = NULL;
    page_info = NULL;

    dummy.base = base;

    irql = ke_raise_irql(IRQL_HIGH);
    ke_rww_r_lock(&lock);

    node = lb_atree_search(&active_tree, &dummy.tree_node);

    ke_rww_r_unlock(&lock);
    ke_lower_irql(irql);

    if (node != NULL)
    {
        page_info = OBTAIN_STRUCT_ADDR(node, struct phys_page_desc, tree_node);
        *out = page_info->attr;
    }
    else
    {
        result = STATUS_INVALID_ARGS;
    }


    return result;
}

k_status mm_free_page(uintptr base)
{
    // just lock since not sharing with anyone
    uint32 irql;
    k_status result;
    struct atree_node *node;
    struct phys_page_desc dummy, *page_info;

    result = STATUS_SUCCESS;
    dummy.base = base;

    irql = ke_raise_irql(IRQL_HIGH);
    ke_rww_w_lock(&lock);

    node = lb_atree_delete(&active_tree, &dummy.tree_node);

    if (node != NULL)
    {
        page_info = OBTAIN_STRUCT_ADDR(node, struct phys_page_desc, tree_node);
        lb_llist_push_back(&free_list, &page_info->free_list_node);
    }
    else
    {
        result = STATUS_INVALID_ARGS;
    }

    ke_rww_w_unlock(&lock);
    ke_lower_irql(irql);

    return result;
}
