#include "kernel/rf/ref.h"
#include "kernel/ke/alloc.h"
#include "kernel/ke/spin_lock.h"
#include "kernel/ke/assert.h"
#include "kernel/ke/atomic.h"
#include "lib/avl_tree.h"
#include "status.h"

typedef struct
{
	struct avl_tree_node tree_node;
	handle_t handle;
	ref_node_t *ref;
	ref_free_func free_routine;
} handle_node_t;

static void SXAPI rfp_handle_node_free(void *node)
{
	ke_free(node);
}

// ===========================
// Ke Functions
// ===========================

static struct avl_tree handle_tree;
static bool initialized;
static k_spin_lock_t handle_tree_lock;
static uint32 handle_base;

static int32 rfp_handle_compare(struct avl_tree_node *tree_node, struct avl_tree_node *my_node)
{
	handle_node_t *tcb = OBTAIN_STRUCT_ADDR(tree_node, handle_node_t, tree_node);
	handle_node_t *my_tcb = OBTAIN_STRUCT_ADDR(my_node, handle_node_t, tree_node);

	if ((uintptr) tcb->handle > (uintptr) my_tcb->handle)
	{
		return -1;
	}
	else
	{
		if ((uintptr) tcb->handle == (uintptr) my_tcb->handle)
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
}

static handle_node_t *rfp_search_handle_node(handle_t handle)
{
	struct avl_tree_node *result;
	handle_node_t temp;
	temp.handle = handle;
	result = lb_avl_tree_search(&handle_tree, &temp.tree_node);
	return result == NULL ? NULL : OBTAIN_STRUCT_ADDR(result, handle_node_t, tree_node);
}

sx_status SXAPI rf_reference_setup(void)
{
	if (!initialized)
	{
		lb_avl_tree_init(&handle_tree, rfp_handle_compare);
		ke_spin_lock_init(&handle_tree_lock);
		handle_base = K_HANDLE_BASE;
		initialized = TRUE;
	}
	return STATUS_SUCCESS;
}

sx_status SXAPI rf_reference_create(ref_node_t *ref,
                                  ref_free_func free_func)
{
	ke_assert(ke_get_irql() <= IRQL_DPC_LEVEL);

	if (ref == NULL || free_func == NULL)
	{
		return RF_INVALID_ARGUMENTS;
	}

	ref->free_routine = free_func;
	ref->ref_count = 1;

	return STATUS_SUCCESS;
}

sx_status SXAPI rf_reference_obj(ref_node_t *ref_node)
{
	ke_assert(ke_get_irql() <= IRQL_DPC_LEVEL);

	if (ref_node == NULL)
	{
		return RF_INVALID_ARGUMENTS;
	}

	int32 old_ref_count = ke_interlocked_increment_32(&ref_node->ref_count, 1);

	ke_assert(old_ref_count >= 1);

	return STATUS_SUCCESS;
}

sx_status SXAPI rf_dereference_obj(ref_node_t *ref_node)
{
	ke_assert(ke_get_irql() <= IRQL_DPC_LEVEL);

	if (ref_node == NULL)
	{
		return RF_INVALID_ARGUMENTS;
	}

	sx_status result = STATUS_SUCCESS;

	int32 old_ref_count = ke_interlocked_increment_32(&ref_node->ref_count, -1);

	ke_assert(old_ref_count >= 1);

	if (old_ref_count == 1)
	{
		ref_node->free_routine(ref_node);
	}

	return result;
}


static sx_status SXAPI rf_open_obj_by_handle(handle_t handle, ref_node_t **out)
{
	ke_assert(ke_get_irql() <= IRQL_DPC_LEVEL);

	if (!initialized)
	{
		return RF_UNINITIALIZED;
	}

	if (out == NULL)
	{
		return RF_INVALID_ARGUMENTS;
	}

	k_irql irql;
	sx_status status = STATUS_SUCCESS;
	ref_node_t *ref = NULL;


	irql = ke_spin_lock_raise_irql(&handle_tree_lock, IRQL_DPC_LEVEL);
	handle_node_t *handle_node = rfp_search_handle_node(handle);
	if (handle_node == NULL)
	{
		status = RF_INVALID_HANDLE;
	}
	else
	{
		ref = handle_node->ref;
	}

	// PREREQUISITE: Having a handle -> having a reference
	// MUST GUARANTEE that handle exists while we reference
	if (sx_success(status))
	{
		// reference the object then return the reference
		rf_reference_obj(ref);
		*out = ref;
	}

	ke_spin_unlock_lower_irql(&handle_tree_lock, irql);

	return status;
}

static sx_status SXAPI rf_create_handle(ref_node_t *ref,
                                      handle_node_t *node,
                                      handle_t *out)
{
	ke_assert(ke_get_irql() <= IRQL_DPC_LEVEL);

	if (!initialized)
	{
		return RF_UNINITIALIZED;
	}

	if (ref == NULL || node == NULL || out == NULL)
	{
		return RF_INVALID_ARGUMENTS;
	}

	sx_status result = STATUS_SUCCESS;
	k_irql irql;


	if (sx_success(result))
	{
		// TODO: CHECK OVERFLOW
		node->handle = (handle_t) ke_interlocked_increment_32((int32*)&handle_base, 1);
		node->ref = ref;
		irql = ke_spin_lock_raise_irql(&handle_tree_lock, IRQL_DPC_LEVEL);
		handle_node_t *existing_node = rfp_search_handle_node(node->handle);
		if (existing_node == NULL)
		{
			lb_avl_tree_insert(&handle_tree, &node->tree_node);
		}
		else
		{
			result = RF_DUPLICATED_HANDLE;
		}

		ke_spin_unlock_lower_irql(&handle_tree_lock, irql);
	}


	if (sx_success(result))
	{
		rf_reference_obj(ref);
		*out = node->handle;
	}
	else
	{
		node->free_routine(node);
	}

	return result;
}

static sx_status SXAPI rf_close_handle(handle_t handle)
{
	ke_assert(ke_get_irql() <= IRQL_DPC_LEVEL);

	if (!initialized)
	{
		return RF_UNINITIALIZED;
	}

	k_irql irql;
	sx_status status = STATUS_SUCCESS;
	ref_node_t *ref = NULL;
	bool free = FALSE;

	irql = ke_spin_lock_raise_irql(&handle_tree_lock, IRQL_DPC_LEVEL);
	handle_node_t *handle_node = rfp_search_handle_node(handle);
	if (handle_node == NULL)
	{
		status = RF_INVALID_HANDLE;
	}
	else
	{
		ref = handle_node->ref;
		lb_avl_tree_delete(&handle_tree, &handle_node->tree_node);
		free = TRUE;
	}
	ke_spin_unlock_lower_irql(&handle_tree_lock, irql);

	if (free)
	{
		handle_node->free_routine(handle_node);
	}

	if (sx_success(status))
	{
		// dereference the object
		rf_dereference_obj(ref);
	}

	return status;
}


// ===========================
// SX Functions
// ===========================

sx_status SXAPI sx_create_handle(ref_node_t *ref, handle_t *out)
{
	ke_assert(ke_get_irql() <= IRQL_DPC_LEVEL);

	if (!initialized)
	{
		return RF_UNINITIALIZED;
	}

	handle_node_t *node;
	node = (handle_node_t *) ke_alloc(sizeof(handle_node_t));
	if (node == NULL)
	{
		return RF_ALLOCATION_FAILED;
	}

	node->free_routine = rfp_handle_node_free;

	return rf_create_handle(ref, node, out);
}

sx_status SXAPI sx_close_handle(handle_t handle)
{
	ke_assert(ke_get_irql() <= IRQL_DPC_LEVEL);

	if (!initialized)
	{
		return RF_UNINITIALIZED;
	}

	// need to keep sx version since need to do handle check here

	return rf_close_handle(handle);
}

sx_status SXAPI sx_open_obj_by_handle(handle_t handle, ref_node_t **out)
{
	ke_assert(ke_get_irql() <= IRQL_DPC_LEVEL);

	if (!initialized)
	{
		return RF_UNINITIALIZED;
	}

	if (out == NULL)
	{
		return RF_INVALID_ARGUMENTS;
	}

	// check special handles first
//    if (handle == K_HANDLE_CURRENT_THREAD)
//    {
//        // no need to ref first since definitely current thread context
//        hw_tcb_t *tcb = ke_current_thread();
//        ke_reference_obj(&tcb->ref_node);
//        *out = &tcb->ref_node;
//        return STATUS_SUCCESS;
//    }

	return rf_open_obj_by_handle(handle, out);
}
