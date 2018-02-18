#include "kernel/rf/ref.h"
#include "kernel/ke/alloc.h"
#include "kernel/ke/spin_lock.h"
#include "kernel/ke/assert.h"
#include "kernel/ke/atomic.h"
#include "lib/avl_tree.h"
#include "status.h"

typedef struct
{
	avl_tree_node_t tree_node;
	handle_t handle;
	ref_node_t *ref;
	callback_func_t free_routine;
} handle_node_t;

static int32_t rfp_handle_node_free(void *node, void *up)
{
	UNREFERENCED(up);
	ke_free(node);
	return 0;
}

// ===========================
// Ke Functions
// ===========================

static avl_tree_t handle_tree;
static bool initialized;
static k_spin_lock_t handle_tree_lock;
static int32_t handle_base;

static int32_t rfp_handle_compare(void *tree_node, void *my_node)
{
	handle_node_t *tcb = OBTAIN_STRUCT_ADDR(tree_node, handle_node_t, tree_node);
	handle_node_t *my_tcb = OBTAIN_STRUCT_ADDR(my_node, handle_node_t, tree_node);

	if ((uintptr_t) tcb->handle > (uintptr_t) my_tcb->handle)
	{
		return -1;
	}
	else
	{
		if ((uintptr_t) tcb->handle == (uintptr_t) my_tcb->handle)
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
	avl_tree_node_t *result;
	handle_node_t temp;
	temp.handle = handle;
	result = lb_avl_tree_search(&handle_tree, &temp.tree_node);
	return result == NULL ? NULL : OBTAIN_STRUCT_ADDR(result, handle_node_t, tree_node);
}

status_t KABI rf_reference_setup(void)
{
	if (!initialized)
	{
		lb_avl_tree_init(&handle_tree, rfp_handle_compare);
		ke_spin_lock_init(&handle_tree_lock);
		handle_base = K_HANDLE_BASE;
		initialized = true;
	}
	return STATUS_SUCCESS;
}

status_t KABI rf_reference_create(ref_node_t *ref,
                                  callback_func_t free_func)
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

status_t KABI rf_reference_obj(ref_node_t *ref_node)
{
	ke_assert(ke_get_irql() <= IRQL_DPC_LEVEL);

	if (ref_node == NULL)
	{
		return RF_INVALID_ARGUMENTS;
	}

	int32_t old_ref_count = ke_interlocked_increment_32(&ref_node->ref_count, 1);

	ke_assert(old_ref_count >= 1);

	return STATUS_SUCCESS;
}

status_t KABI rf_dereference_obj(ref_node_t *ref_node)
{
	ke_assert(ke_get_irql() <= IRQL_DPC_LEVEL);

	if (ref_node == NULL)
	{
		return RF_INVALID_ARGUMENTS;
	}

	status_t result = STATUS_SUCCESS;

	int32_t old_ref_count = ke_interlocked_increment_32(&ref_node->ref_count, -1);

	ke_assert(old_ref_count >= 1);

	if (old_ref_count == 1)
	{
		ref_node->free_routine(ref_node, NULL);
	}

	return result;
}


static status_t KABI rf_open_obj_by_handle(handle_t handle, ref_node_t **out)
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

	irql_t irql;
	status_t status = STATUS_SUCCESS;
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

static status_t KABI rf_create_handle(ref_node_t *ref,
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

	status_t result = STATUS_SUCCESS;
	irql_t irql;


	if (sx_success(result))
	{
		// TODO: CHECK OVERFLOW
		node->handle = (handle_t) ke_interlocked_increment_32(&handle_base, 1);
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
		node->free_routine(node, NULL);
	}

	return result;
}

static status_t KABI rf_close_handle(handle_t handle)
{
	ke_assert(ke_get_irql() <= IRQL_DPC_LEVEL);

	if (!initialized)
	{
		return RF_UNINITIALIZED;
	}

	irql_t irql;
	status_t status = STATUS_SUCCESS;
	ref_node_t *ref = NULL;
	bool free = false;

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
		free = true;
	}
	ke_spin_unlock_lower_irql(&handle_tree_lock, irql);

	if (free)
	{
		handle_node->free_routine(handle_node, NULL);
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

status_t KABI sx_create_handle(ref_node_t *ref, handle_t *out)
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

status_t KABI sx_close_handle(handle_t handle)
{
	ke_assert(ke_get_irql() <= IRQL_DPC_LEVEL);

	if (!initialized)
	{
		return RF_UNINITIALIZED;
	}

	// need to keep sx version since need to do handle check here

	return rf_close_handle(handle);
}

status_t KABI sx_open_obj_by_handle(handle_t handle, ref_node_t **out)
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
