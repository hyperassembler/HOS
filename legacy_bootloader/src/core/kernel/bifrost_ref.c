#include <bifrost_assert.h>
#include <bifrost_thread.h>
#include "bifrost_statuscode.h"
#include "bifrost_mem.h"
#include "bifrost_ref.h"
#include "bifrost_lock.h"
#include "bifrost_stdlib.h"
#include "bifrost_alloc.h"

typedef struct
{
    avl_tree_node_t tree_node;
    hw_handle_t handle;
    hw_ref_node_t *ref;
    hw_callback_func_t free_func;
} hw_handle_node_t;

static void handle_node_free(void *node, void *up)
{
    hw_free(node);
}

// ===========================
// Ke Functions
// ===========================

static avl_tree_t _handle_tree;
static bool _initialized;
static hw_spin_lock_t _handle_tree_lock;
static int32_t _handle_base;

static int32_t handle_compare(avl_tree_node_t *tree_node, avl_tree_node_t *my_node)
{
    hw_handle_node_t *tcb = OBTAIN_STRUCT_ADDR(tree_node, tree_node, hw_handle_node_t);
    hw_handle_node_t *my_tcb = OBTAIN_STRUCT_ADDR(my_node, tree_node, hw_handle_node_t);

    if ((uintptr_t) tcb->handle > (uintptr_t) my_tcb->handle)
        return -1;
    else if ((uintptr_t) tcb->handle == (uintptr_t) my_tcb->handle)
        return 0;
    else
        return 1;
}

static hw_handle_node_t *search_handle_node(hw_handle_t handle)
{
    avl_tree_node_t *result;
    hw_handle_node_t temp;
    temp.handle = handle;
    result = avl_tree_search(&_handle_tree, &temp.tree_node);
    return result == NULL ? NULL : OBTAIN_STRUCT_ADDR(result, tree_node, hw_handle_node_t);
}

hw_result_t ke_reference_setup()
{
    if (!_initialized)
    {
        avl_tree_init(&_handle_tree, handle_compare);
        ke_spin_lock_init(&_handle_tree_lock);
        _handle_base = HW_HANDLE_BASE;
        _initialized = true;
    }
    return STATUS_SUCCESS;
}

hw_result_t ke_reference_create(hw_ref_node_t *ref,
                                hw_callback_func_t free_func)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    if (!_initialized)
        return REF_STATUS_UNINITIALIZED;

    if (ref == NULL || free_func == NULL)
        return REF_STATUS_INVALID_ARGUMENTS;

    ref->callback = free_func;
    ref->ref_count = 1;

    return STATUS_SUCCESS;
}

hw_result_t ke_reference_obj(hw_ref_node_t *ref_node)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    if (!_initialized)
        return REF_STATUS_UNINITIALIZED;

    if (ref_node == NULL)
        return REF_STATUS_INVALID_ARGUMENTS;

    int32_t old_ref_count = ke_interlocked_increment(&ref_node->ref_count, 1);

    hw_assert(old_ref_count >= 1);

    return STATUS_SUCCESS;
}

hw_result_t ke_dereference_obj(hw_ref_node_t *ref_node)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    if (!_initialized)
        return REF_STATUS_UNINITIALIZED;
    if (ref_node == NULL)
        return REF_STATUS_INVALID_ARGUMENTS;

    hw_result_t result = STATUS_SUCCESS;

    int32_t old_ref_count = ke_interlocked_increment(&ref_node->ref_count, -1);

    hw_assert(old_ref_count >= 1);

    if (old_ref_count == 1)
    {
        ref_node->callback(ref_node, NULL);
    }

    return result;
}


static hw_result_t ke_open_obj_by_handle(hw_handle_t handle, hw_ref_node_t **out)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    if (!_initialized)
    {
        return REF_STATUS_UNINITIALIZED;
    }

    if (out == NULL)
    {
        return REF_STATUS_INVALID_ARGUMENTS;
    }

    hw_irql_t irql;
    hw_result_t result = STATUS_SUCCESS;
    hw_ref_node_t *ref = NULL;


    irql = ke_spin_lock_raise_irql(&_handle_tree_lock, HW_IRQL_DPC_LEVEL);
    hw_handle_node_t *handle_node = search_handle_node(handle);
    if (handle_node == NULL)
    {
        result = REF_STATUS_HANDLE_NOT_FOUND;
    }
    else
    {
        ref = handle_node->ref;
    }

    // PREREQUISITE: Having a handle -> having a reference
    // MUST GUARANTEE that handle exists while we reference
    if (HW_SUCCESS(result))
    {
        // reference the object then return the reference
        result = ke_reference_obj(ref);
        if (HW_SUCCESS(result))
        {
            *out = ref;
        }
    }

    ke_spin_unlock_lower_irql(&_handle_tree_lock, irql);

    return result;
}

static hw_result_t ke_create_handle(hw_ref_node_t *ref,
                                    hw_handle_node_t *node,
                                    hw_handle_t *out)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    if (!_initialized)
        return REF_STATUS_UNINITIALIZED;

    if (ref == NULL || node == NULL || out == NULL)
        return REF_STATUS_INVALID_ARGUMENTS;

    hw_result_t result = STATUS_SUCCESS;
    hw_irql_t irql;


    if (HW_SUCCESS(result))
    {
        // TODO: CHECK OVERFLOW
        node->handle = (hw_handle_t) ke_interlocked_increment(&_handle_base, 1);
        node->ref = ref;
        irql = ke_spin_lock_raise_irql(&_handle_tree_lock, HW_IRQL_DPC_LEVEL);
        hw_handle_node_t *existing_node = search_handle_node(node->handle);
        if (existing_node == NULL)
        {
            avl_tree_insert(&_handle_tree, &node->tree_node);
        }
        else
        {
            result = REF_STATUS_HANDLE_DUPLICATE;
        }

        ke_spin_unlock_lower_irql(&_handle_tree_lock, irql);
    }


    if (HW_SUCCESS(result))
    {
        ke_reference_obj(ref);
        *out = node->handle;
    }
    else
    {
        node->free_func(node, NULL);
    }

    return result;
}

static hw_result_t ke_close_handle(hw_handle_t handle)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    if (!_initialized)
        return REF_STATUS_UNINITIALIZED;

    hw_irql_t irql;
    hw_result_t result = STATUS_SUCCESS;
    hw_ref_node_t *ref = NULL;

    irql = ke_spin_lock_raise_irql(&_handle_tree_lock, HW_IRQL_DPC_LEVEL);
    hw_handle_node_t *handle_node = search_handle_node(handle);
    if (handle_node == NULL)
    {
        result = REF_STATUS_HANDLE_NOT_FOUND;
    }
    else
    {
        ref = handle_node->ref;
        avl_tree_delete(&_handle_tree, &handle_node->tree_node);
        handle_node->free_func(handle_node, NULL);
    }
    ke_spin_unlock_lower_irql(&_handle_tree_lock, irql);

    if (HW_SUCCESS(result))
    {
        // dereference the object
        result = ke_dereference_obj(ref);
    }

    return result;
}


// ===========================
// HW Functions
// ===========================

hw_result_t hw_create_handle(hw_ref_node_t *ref, hw_handle_t *out)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    if (!_initialized)
        return REF_STATUS_UNINITIALIZED;

    hw_handle_node_t *node;
    node = (hw_handle_node_t *) hw_alloc(sizeof(hw_handle_node_t));
    if (node == NULL)
    {
        return REF_STATUS_CANNOT_ALLOCATE_MEM;
    }

    node->free_func = handle_node_free;

    return ke_create_handle(ref, node, out);
}

hw_result_t hw_close_handle(hw_handle_t handle)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    if (!_initialized)
        return REF_STATUS_UNINITIALIZED;

    if (handle == HW_HANDLE_CURRENT_THREAD)
    {
        return REF_STATUS_NO_EFFECT;
    }

    hw_result_t result = ke_close_handle(handle);

    return result;
}

hw_result_t hw_open_obj_by_handle(hw_handle_t handle, hw_ref_node_t **out)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    if (!_initialized)
        return REF_STATUS_UNINITIALIZED;

    if (out == NULL)
        return REF_STATUS_INVALID_ARGUMENTS;

    // check special handles first
    if (handle == HW_HANDLE_CURRENT_THREAD)
    {
        // no need to ref first since definitely current thread context
        hw_tcb_t *tcb = ke_current_thread();
        ke_reference_obj(&tcb->ref_node);
        *out = &tcb->ref_node;
        return STATUS_SUCCESS;
    }

    return ke_open_obj_by_handle(handle, out);
}
