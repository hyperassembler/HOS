#include "kernel/ke/assert.h"
#include "kernel/ke/rwwlock.h"
#include "status.h"
#include "kernel/ke/alloc.h"
#include "kernel/mm/pmm.h"

typedef struct
{
	linked_list_node_t free_list_node;
	avl_tree_node_t avl_tree_node;
	uintptr_t base;
	int32_t attr;
} physical_page_descriptor_t;

static avl_tree_t active_tree;
static linked_list_t free_list;
static k_rwwlock_t lock;
static _Bool initialized;

/*
 * A comparison function between tree_node and your_node
 * Returns:
 * < 0 if tree_node < your_node
 * = 0 if tree_node == your_node
 * > 0 if tree_node > your_node
 */
static int32_t mmp_base_paddr_compare(void *tree_node, void *my_node)
{
	uintptr_t tree_base = OBTAIN_STRUCT_ADDR(tree_node,
	                                         physical_page_descriptor_t,
	                                         avl_tree_node)->base;
	uintptr_t my_base = OBTAIN_STRUCT_ADDR(my_node,
	                                       physical_page_descriptor_t,
	                                       avl_tree_node)->base;
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

status_t KABI sx_pmm_init(pmm_info_t *info)
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

	for (uint32_t i = 0; i < info->num_of_nodes; i++)
	{
		pmm_node_t *each_node = &info->nodes[i];

		ke_assert (each_node->base % KERNEL_PAGE_SIZE != 0);

		for (uint64_t j = 0; j <= each_node->size; j++)
		{
			// note that k_alloc function here might trigger page fault
			// however it's fine as long as we don't touch linked list just yet
			// it will use the pages that are already on file to enlarge the kernel heap
			// don't worry, be happy :)
			physical_page_descriptor_t *page_info = ke_alloc(sizeof(physical_page_descriptor_t));

			if (page_info == NULL)
			{
				return MM_ALLOCATION_FAILED;
			}

			page_info->base = each_node->base;
			lb_linked_list_push_back(&free_list, &page_info->free_list_node);
		}
	}
	initialized = true;
	return STATUS_SUCCESS;
}

// free lists can only be updated at IRQL == DISABLED
// we need to guarantee that on the same CPU, these APIs are not preempted by
// potential callers of these, since timer/interrupts queue DPC, which might trigger
// page fault (kernel heap), therefore, it must set IRQL to DISABLED

status_t KABI mm_alloc_page(uintptr_t *out)
{
	if (!initialized)
	{
		return MM_UNINITIALIZED;
	}

	if (out == NULL)
	{
		return MM_INVALID_ARGUMENTS;
	}

	irql_t irql = ke_rwwlock_writer_lock_raise_irql(&lock, IRQL_DISABLED_LEVEL);
	status_t result = STATUS_SUCCESS;
	linked_list_node_t *node = NULL;
	physical_page_descriptor_t *page_info = NULL;
	node = lb_linked_list_pop_front(&free_list);
	if (node != NULL)
	{
		page_info = OBTAIN_STRUCT_ADDR(node,
		                               physical_page_descriptor_t,
		                               free_list_node);
		lb_avl_tree_insert(&active_tree, &page_info->avl_tree_node);
		*out = page_info->base;
	}
	else
	{
		result = MM_NOT_ENOUGH_PAGE;
	}

	ke_rwwlock_writer_unlock_lower_irql(&lock, irql);
	return result;
}

status_t KABI mm_query_page_attr(uintptr_t base,
                                 int32_t *out)
{
	if (!initialized)
	{
		return MM_UNINITIALIZED;
	}

	if (out == NULL)
	{
		return MM_INVALID_ARGUMENTS;
	}

	irql_t irql = ke_rwwlock_reader_lock_raise_irql(&lock, IRQL_DISABLED_LEVEL);
	status_t result = STATUS_SUCCESS;
	avl_tree_node_t *node = NULL;
	// search for dummy
	physical_page_descriptor_t dummy, *page_info = NULL;
	dummy.base = base;

	node = lb_avl_tree_delete(&active_tree, &dummy.avl_tree_node);

	if (node != NULL)
	{
		page_info = OBTAIN_STRUCT_ADDR(node, physical_page_descriptor_t, avl_tree_node);
		*out = page_info->attr;
	}
	else
	{
		result = MM_INVALID_ARGUMENTS;
	}

	ke_rwwlock_reader_unlock_lower_irql(&lock, irql);

	return result;
}

status_t KABI mm_free_page(uintptr_t base)
{
	if (!initialized)
	{
		return MM_UNINITIALIZED;
	}

	// just lock since not sharing with anyone
	irql_t irql = ke_rwwlock_writer_lock_raise_irql(&lock, IRQL_DISABLED_LEVEL);
	status_t result = STATUS_SUCCESS;
	avl_tree_node_t *node = NULL;
	// search for dummy
	physical_page_descriptor_t dummy, *page_info;
	dummy.base = base;

	node = lb_avl_tree_delete(&active_tree, &dummy.avl_tree_node);
	if (node != NULL)
	{
		page_info = OBTAIN_STRUCT_ADDR(node, physical_page_descriptor_t, avl_tree_node);
		lb_linked_list_push_back(&free_list, &page_info->free_list_node);
	}
	else
	{
		result = MM_INVALID_ARGUMENTS;
	}

	ke_rwwlock_writer_unlock_lower_irql(&lock, irql);

	return result;
}