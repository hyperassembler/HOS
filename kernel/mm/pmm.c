#include "kernel/ke/assert.h"
#include "kernel/ke/rwwlock.h"
#include "status.h"
#include "kernel/ke/alloc.h"
#include "kernel/mm/pmm.h"

struct phys_page_desc
{
	struct linked_list_node free_list_node;
	struct avl_tree_node tree_node;
	uintptr base;
	int32 attr;
};

static struct avl_tree active_tree;
static struct linked_list free_list;
static k_rwwlock_t lock;
static _Bool initialized;

/*
 * A comparison function between tree_node and your_node
 * Returns:
 * < 0 if tree_node < your_node
 * = 0 if tree_node == your_node
 * > 0 if tree_node > your_node
 */
static int32 SXAPI mmp_base_paddr_compare(struct avl_tree_node* tree_node, struct avl_tree_node *my_node)
{
	uintptr tree_base = OBTAIN_STRUCT_ADDR(tree_node,
	                                         struct phys_page_desc,
	                                         tree_node)->base;
	uintptr my_base = OBTAIN_STRUCT_ADDR(my_node,
	                                       struct phys_page_desc,
	                                       tree_node)->base;
	if (tree_base > my_base)
	{
		return 1;
	}
	else
	{
		if (tree_base < my_base)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}
}

sx_status SXAPI sx_pmm_init(pmm_info_t *info)
{
	if (info == NULL)
	{
		return MM_INVALID_ARGUMENTS;
	}

	if (initialized)
	{
		return STATUS_SUCCESS;
	}

	ke_rwwlock_init(&lock);
	lb_linked_list_init(&free_list);
	lb_avl_tree_init(&active_tree, mmp_base_paddr_compare);

	for (uint32 i = 0; i < info->num_of_nodes; i++)
	{
		pmm_node_t *each_node = &info->nodes[i];

		ke_assert (each_node->base % KERNEL_PAGE_SIZE != 0);

		for (uint64 j = 0; j <= each_node->size; j++)
		{
			// note that k_alloc function here might trigger page fault
			// however it's fine as long as we don't touch linked list just yet
			// it will use the pages that are already on file to enlarge the kernel heap
			// don't worry, be happy :)
			struct phys_page_desc *page_info = ke_alloc(sizeof(struct phys_page_desc));

			if (page_info == NULL)
			{
				return MM_ALLOCATION_FAILED;
			}

			page_info->base = each_node->base;
			lb_linked_list_push_back(&free_list, &page_info->free_list_node);
		}
	}
	initialized = TRUE;
	return STATUS_SUCCESS;
}

// free lists can only be updated at IRQL == DISABLED
// we need to guarantee that on the same CPU, these APIs are not preempted by
// potential callers of these, since timer/interrupts queue DPC, which might trigger
// page fault (kernel heap), therefore, it must set IRQL to DISABLED

sx_status SXAPI mm_alloc_page(uintptr *out)
{
	if (!initialized)
	{
		return MM_UNINITIALIZED;
	}

	if (out == NULL)
	{
		return MM_INVALID_ARGUMENTS;
	}

	k_irql irql = ke_rwwlock_writer_lock_raise_irql(&lock, IRQL_DISABLED_LEVEL);
	sx_status result = STATUS_SUCCESS;
	struct linked_list_node *node = NULL;
	struct phys_page_desc *page_info = NULL;
	node = lb_linked_list_pop_front(&free_list);
	if (node != NULL)
	{
		page_info = OBTAIN_STRUCT_ADDR(node,
		                               struct phys_page_desc,
		                               free_list_node);
		lb_avl_tree_insert(&active_tree, &page_info->tree_node);
		*out = page_info->base;
	}
	else
	{
		result = MM_NOT_ENOUGH_PAGE;
	}

	ke_rwwlock_writer_unlock_lower_irql(&lock, irql);
	return result;
}

sx_status SXAPI mm_query_page_attr(uintptr base,
                                 int32 *out)
{
	if (!initialized)
	{
		return MM_UNINITIALIZED;
	}

	if (out == NULL)
	{
		return MM_INVALID_ARGUMENTS;
	}

	k_irql irql = ke_rwwlock_reader_lock_raise_irql(&lock, IRQL_DISABLED_LEVEL);
	sx_status result = STATUS_SUCCESS;
	struct avl_tree_node *node = NULL;
	// search for dummy
	struct phys_page_desc dummy, *page_info = NULL;
	dummy.base = base;

	node = lb_avl_tree_delete(&active_tree, &dummy.tree_node);

	if (node != NULL)
	{
		page_info = OBTAIN_STRUCT_ADDR(node, struct phys_page_desc, tree_node);
		*out = page_info->attr;
	}
	else
	{
		result = MM_INVALID_ARGUMENTS;
	}

	ke_rwwlock_reader_unlock_lower_irql(&lock, irql);

	return result;
}

sx_status SXAPI mm_free_page(uintptr base)
{
	if (!initialized)
	{
		return MM_UNINITIALIZED;
	}

	// just lock since not sharing with anyone
	k_irql irql = ke_rwwlock_writer_lock_raise_irql(&lock, IRQL_DISABLED_LEVEL);
	sx_status result = STATUS_SUCCESS;
	struct avl_tree_node *node = NULL;
	// search for dummy
	struct phys_page_desc dummy, *page_info;
	dummy.base = base;

	node = lb_avl_tree_delete(&active_tree, &dummy.tree_node);
	if (node != NULL)
	{
		page_info = OBTAIN_STRUCT_ADDR(node, struct phys_page_desc, tree_node);
		lb_linked_list_push_back(&free_list, &page_info->free_list_node);
	}
	else
	{
		result = MM_INVALID_ARGUMENTS;
	}

	ke_rwwlock_writer_unlock_lower_irql(&lock, irql);

	return result;
}
