#include <bifrost_thread.h>
#include "bifrost_assert.h"
#include "bifrost_apc.h"
#include "conf_defs.h"
#include "bifrost_stdlib.h"
#include "bifrost_alloc.h"

static uint32_t _apc_int_vec;
static uint32_t _apc_initialized[HW_PROC_CNT] = {false};

typedef struct
{
    linked_list_node_t list_node;
    hw_callback_func_t free_func;
    hw_callback_func_t proc;
    void *args;
} hw_apc_node_t;

static void apc_node_free(void *node, void *up)
{
    hw_free(node);
}

hw_result_t ke_apc_drain(uint32_t core)
{
    if (!_apc_initialized[core])
        return APC_STATUS_NOT_INITIALIZED;

    if (core < HW_PROC_CNT)
    {
        ke_trigger_intr(core, _apc_int_vec);
        return STATUS_SUCCESS;
    }
    return APC_STATUS_INVALID_ARGUMENTS;
}

static hw_result_t ke_apc_queue(hw_tcb_t *tcb, hw_callback_func_t proc, void *args, hw_apc_node_t *node)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    if (proc != NULL && tcb != NULL && node != NULL)
    {
        if (!_apc_initialized[tcb->core_id])
            return APC_STATUS_NOT_INITIALIZED;

        node->args = args;
        node->proc = proc;
        node->free_func = apc_node_free;

        hw_irql_t irql;
        irql = ke_spin_lock_raise_irql(&tcb->apc_lock, HW_IRQL_DPC_LEVEL);
        linked_list_push_back(&tcb->apc_list, &node->list_node);
        ke_spin_unlock_lower_irql(&tcb->apc_lock, irql);
        if (tcb->state == STATE_RUN)
        {
            ke_apc_drain(tcb->core_id);
        }
        return STATUS_SUCCESS;
    }
    return APC_STATUS_INVALID_ARGUMENTS;
}

hw_result_t hw_apc_queue(hw_handle_t thread_handle, hw_callback_func_t proc, void *args)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    if (proc == NULL)
        return APC_STATUS_INVALID_ARGUMENTS;

    // reference tcb pointer
    hw_ref_node_t *ref;
    hw_result_t result = hw_open_obj_by_handle(thread_handle, &ref);

    if (HW_SUCCESS(result))
    {
        hw_tcb_t *tcb = OBTAIN_STRUCT_ADDR(ref, ref_node, hw_tcb_t);

        if (_apc_initialized[tcb->core_id])
        {
            hw_apc_node_t *node = (hw_apc_node_t *) hw_alloc(sizeof(hw_apc_node_t));
            if (node == NULL)
            {
                result = APC_STATUS_CANNOT_ALLOCATE_MEM;
            }
            else
            {
                node->args = args;
                node->proc = proc;
                node->free_func = apc_node_free;
                result = ke_apc_queue(tcb, proc, args, node);
            }
        }
        else
        {
            result = APC_STATUS_NOT_INITIALIZED;
        }
        // dereference the tcb pointer ref
        ke_dereference_obj(&tcb->ref_node);
    }
    return result;
}

static void ke_apc_interrupt_handler(void *intr_stack, void *usr_context)
{
    hw_assert(ke_get_irql() == HW_IRQL_APC_LEVEL);
    if (_apc_initialized[ke_get_current_core()])
    {
        // no need to reference since current thread context
        hw_tcb_t *tcb = ke_current_thread();

        hw_irql_t irql;
        while (1)
        {
            linked_list_node_t *node;
            irql = ke_spin_lock_raise_irql(&tcb->apc_lock, HW_IRQL_DPC_LEVEL);
            node = linked_list_pop_front(&tcb->apc_list);
            ke_spin_unlock_lower_irql(&tcb->apc_lock, irql);
            if (node == NULL)
            {
                break;
            }
            else
            {
                hw_apc_node_t *apc_node = OBTAIN_STRUCT_ADDR(node, list_node, hw_apc_node_t);
                apc_node->proc(NULL, apc_node->args);
                apc_node->free_func(apc_node, NULL);
            }
        }
    }
    return;
}

hw_result_t ke_apc_setup(uint32_t int_vec)
{
    if (!_apc_initialized[ke_get_current_core()])
    {
        _apc_int_vec = int_vec;
        ke_register_intr_handler(_apc_int_vec, ke_apc_interrupt_handler, NULL);
        _apc_initialized[ke_get_current_core()] = true;
    }
    return STATUS_SUCCESS;
}

