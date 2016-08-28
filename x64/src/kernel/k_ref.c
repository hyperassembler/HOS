#include <k_ref.h>
#include <k_alloc.h>
#include <k_spin_lock.h>
#include <k_assert.h>
#include <k_atomic.h>
#include "k_avl_tree.h"

typedef struct
{
    k_avl_tree_node_t tree_node;
    k_handle_t handle;
    k_ref_node_t *ref;
    k_callback_func_t free_routine;
} k_handle_node_t;

static int32_t handle_node_free(void *node, void *up)
{
    ke_free(node);
    return 0;
}

// ===========================
// Ke Functions
// ===========================

static k_avl_tree_t _handle_tree;
static bool _initialized;
static k_spin_lock_t _handle_tree_lock;
static int32_t _handle_base;

static int32_t handle_compare(void *tree_node, void *my_node)
{
    k_handle_node_t *tcb = OBTAIN_STRUCT_ADDR(tree_node, k_handle_node_t, tree_node);
    k_handle_node_t *my_tcb = OBTAIN_STRUCT_ADDR(my_node, k_handle_node_t, tree_node);

    if ((uintptr_t) tcb->handle > (uintptr_t) my_tcb->handle)
        return -1;
    else if ((uintptr_t) tcb->handle == (uintptr_t) my_tcb->handle)
        return 0;
    else
        return 1;
}

static k_handle_node_t *search_handle_node(k_handle_t handle)
{
    k_avl_tree_node_t *result;
    k_handle_node_t temp;
    temp.handle = handle;
    result = ke_avl_tree_search(&_handle_tree, &temp.tree_node);
    return result == NULL ? NULL : OBTAIN_STRUCT_ADDR(result, k_handle_node_t, tree_node);
}

k_status_t KAPI ke_reference_setup()
{
    if (!_initialized)
    {
        ke_avl_tree_init(&_handle_tree, handle_compare);
        ke_spin_lock_init(&_handle_tree_lock);
        _handle_base = K_HANDLE_BASE;
        _initialized = true;
    }
    return STATUS_SUCCESS;
}

k_status_t KAPI ke_reference_create(k_ref_node_t *ref,
                                    k_callback_func_t free_func)
{
    ke_assert(ke_get_irql() <= K_IRQL_DPC_LEVEL);

    if (ref == NULL || free_func == NULL)
        return REF_STATUS_INVALID_ARGUMENTS;

    ref->free_routine = free_func;
    ref->ref_count = 1;

    return STATUS_SUCCESS;
}

k_status_t KAPI ke_reference_obj(k_ref_node_t *ref_node)
{
    ke_assert(ke_get_irql() <= K_IRQL_DPC_LEVEL);

    if (ref_node == NULL)
        return REF_STATUS_INVALID_ARGUMENTS;

    int32_t old_ref_count = ke_interlocked_increment(&ref_node->ref_count, 1);

    ke_assert(old_ref_count >= 1);

    return STATUS_SUCCESS;
}

k_status_t KAPI ke_dereference_obj(k_ref_node_t *ref_node)
{
    ke_assert(ke_get_irql() <= K_IRQL_DPC_LEVEL);

    if (ref_node == NULL)
        return REF_STATUS_INVALID_ARGUMENTS;

    k_status_t result = STATUS_SUCCESS;

    int32_t old_ref_count = ke_interlocked_increment(&ref_node->ref_count, -1);

    ke_assert(old_ref_count >= 1);

    if (old_ref_count == 1)
    {
        ref_node->free_routine(ref_node, NULL);
    }

    return result;
}


static k_status_t KAPI ke_open_obj_by_handle(k_handle_t handle, k_ref_node_t **out)
{
    ke_assert(ke_get_irql() <= K_IRQL_DPC_LEVEL);

    if (!_initialized)
    {
        return REF_STATUS_UNINITIALIZED;
    }

    if (out == NULL)
    {
        return REF_STATUS_INVALID_ARGUMENTS;
    }

    k_irql_t irql;
    k_status_t status = STATUS_SUCCESS;
    k_ref_node_t *ref = NULL;


    irql = ke_spin_lock_raise_irql(&_handle_tree_lock, K_IRQL_DPC_LEVEL);
    k_handle_node_t *handle_node = search_handle_node(handle);
    if (handle_node == NULL)
    {
        status = REF_STATUS_INVALID_HANDLE;
    } else
    {
        ref = handle_node->ref;
    }

    // PREREQUISITE: Having a handle -> having a reference
    // MUST GUARANTEE that handle exists while we reference
    if (SX_SUCCESS(status))
    {
        // reference the object then return the reference
        ke_reference_obj(ref);
        *out = ref;
    }

    ke_spin_unlock_lower_irql(&_handle_tree_lock, irql);

    return status;
}

static k_status_t KAPI ke_create_handle(k_ref_node_t *ref,
                                        k_handle_node_t *node,
                                        k_handle_t *out)
{
    ke_assert(ke_get_irql() <= K_IRQL_DPC_LEVEL);

    if (!_initialized)
        return REF_STATUS_UNINITIALIZED;

    if (ref == NULL || node == NULL || out == NULL)
        return REF_STATUS_INVALID_ARGUMENTS;

    k_status_t result = STATUS_SUCCESS;
    k_irql_t irql;


    if (SX_SUCCESS(result))
    {
        // TODO: CHECK OVERFLOW
        node->handle = (k_handle_t) ke_interlocked_increment(&_handle_base, 1);
        node->ref = ref;
        irql = ke_spin_lock_raise_irql(&_handle_tree_lock, K_IRQL_DPC_LEVEL);
        k_handle_node_t *existing_node = search_handle_node(node->handle);
        if (existing_node == NULL)
        {
            ke_avl_tree_insert(&_handle_tree, &node->tree_node);
        } else
        {
            result = REF_STATUS_DUPLICATED_HANDLE;
        }

        ke_spin_unlock_lower_irql(&_handle_tree_lock, irql);
    }


    if (SX_SUCCESS(result))
    {
        ke_reference_obj(ref);
        *out = node->handle;
    } else
    {
        node->free_routine(node, NULL);
    }

    return result;
}

static k_status_t KAPI ke_close_handle(k_handle_t handle)
{
    ke_assert(ke_get_irql() <= K_IRQL_DPC_LEVEL);

    if (!_initialized)
        return REF_STATUS_UNINITIALIZED;

    k_irql_t irql;
    k_status_t status = STATUS_SUCCESS;
    k_ref_node_t *ref = NULL;
    bool free = false;

    irql = ke_spin_lock_raise_irql(&_handle_tree_lock, K_IRQL_DPC_LEVEL);
    k_handle_node_t *handle_node = search_handle_node(handle);
    if (handle_node == NULL)
    {
        status = REF_STATUS_INVALID_HANDLE;
    } else
    {
        ref = handle_node->ref;
        ke_avl_tree_delete(&_handle_tree, &handle_node->tree_node);
        free = true;
    }
    ke_spin_unlock_lower_irql(&_handle_tree_lock, irql);

    if (free)
    {
        handle_node->free_routine(handle_node, NULL);
    }

    if (SX_SUCCESS(status))
    {
        // dereference the object
        ke_dereference_obj(ref);
    }

    return status;
}


// ===========================
// HW Functions
// ===========================

k_status_t KAPI sx_create_handle(k_ref_node_t *ref, k_handle_t *out)
{
    ke_assert(ke_get_irql() <= K_IRQL_DPC_LEVEL);

    if (!_initialized)
        return REF_STATUS_UNINITIALIZED;

    k_handle_node_t *node;
    node = (k_handle_node_t *) k_alloc(sizeof(k_handle_node_t));
    if (node == NULL)
    {
        return REF_STATUS_ALLOCATION_FAILED;
    }

    node->free_routine = handle_node_free;

    return ke_create_handle(ref, node, out);
}

k_status_t KAPI sx_close_handle(k_handle_t handle)
{
    ke_assert(ke_get_irql() <= K_IRQL_DPC_LEVEL);

    if (!_initialized)
        return REF_STATUS_UNINITIALIZED;

    // need to keep sx version since need to do handle check here

    return ke_close_handle(handle);
}

k_status_t KAPI sx_open_obj_by_handle(k_handle_t handle, k_ref_node_t **out)
{
    ke_assert(ke_get_irql() <= K_IRQL_DPC_LEVEL);

    if (!_initialized)
        return REF_STATUS_UNINITIALIZED;

    if (out == NULL)
        return REF_STATUS_INVALID_ARGUMENTS;

    // check special handles first
//    if (handle == K_HANDLE_CURRENT_THREAD)
//    {
//        // no need to ref first since definitely current thread context
//        hw_tcb_t *tcb = ke_current_thread();
//        ke_reference_obj(&tcb->ref_node);
//        *out = &tcb->ref_node;
//        return STATUS_SUCCESS;
//    }

    return ke_open_obj_by_handle(handle, out);
}
