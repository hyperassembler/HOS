#include <bifrost_thread.h>
#include <bifrost_timer.h>
#include "bifrost_dpc.h"
#include "bifrost_context.h"
#include "bifrost_thread.h"
#include "conf_defs.h"
#include "bifrost_alloc.h"
#include "bifrost_stdlib.h"
#include "bifrost_system_constants.h"

static linked_list_t _dpc_list_arr[HW_PROC_CNT];
static MEM_SRAM_UC hw_spin_lock_t _dpc_lock_arr[HW_PROC_CNT];
static _Bool _dpc_initialized[HW_PROC_CNT] = {false};
static uint32_t _dpc_int_vec;

typedef struct
{
    linked_list_node_t list_node;
    hw_callback_func_t proc;
    void *args;
    hw_callback_func_t free_func;
} hw_dpc_node_t;

static void dpc_free(void *node, void *np)
{
    hw_free(node);
}

// Used for storing regs (kernel context) for switching to the first thread
// Theoretically can return to kernel by restoring these regs
static uint64_t _dummy_regs[16];

hw_result_t ke_dpc_drain(uint32_t core)
{
    if (!_dpc_initialized[core])
        return DPC_STATUS_NOT_INITIALIZED;

    if (core < HW_PROC_CNT)
    {
        ke_trigger_intr(core, _dpc_int_vec);
        return STATUS_SUCCESS;
    }
    return DPC_STATUS_INVALID_ARGUMENTS;
}

static hw_result_t ke_dpc_queue(uint32_t core, hw_callback_func_t proc, void *arg, hw_dpc_node_t *node)
{
    if (!_dpc_initialized[core])
        return DPC_STATUS_NOT_INITIALIZED;

    if (core < HW_PROC_CNT && proc != NULL && node != NULL)
    {
        hw_irql_t irql;
        node->args = arg;
        node->proc = proc;
        node->free_func = dpc_free;
        irql = ke_spin_lock_raise_irql(&_dpc_lock_arr[core], HW_IRQL_DISABLED_LEVEL);
        linked_list_push_back(&_dpc_list_arr[core], &node->list_node);
        ke_spin_unlock_lower_irql(&_dpc_lock_arr[core], irql);
        return STATUS_SUCCESS;
    }
    return DPC_STATUS_INVALID_ARGUMENTS;
}

hw_result_t hw_dpc_queue(uint32_t core, hw_callback_func_t proc, void *args)
{
    if (!_dpc_initialized[core])
        return DPC_STATUS_NOT_INITIALIZED;

    if (core < HW_PROC_CNT && proc != NULL && _dpc_initialized[ke_get_current_core()])
    {
        hw_dpc_node_t *node = (hw_dpc_node_t *) hw_alloc(sizeof(hw_dpc_node_t));
        if (node == NULL)
        {
            return DPC_STATUS_NOT_ENOUGH_MEM;
        }
        return ke_dpc_queue(core, proc, args, node);
    }
    return DPC_STATUS_INVALID_ARGUMENTS;
}

static void ke_dpc_interrupt_handler(void *intr_stack, void *usr_context)
{
    uint32_t core_id = ke_get_current_core();

    void *prev_context = NULL;
    hw_thread_schedule_info_t info;
    bool scheduled = false;

    if (_dpc_initialized[core_id])
    {
        hw_irql_t irql;

        // handle normal DPCs
        while (1)
        {
            linked_list_node_t *node;
            irql = ke_spin_lock_raise_irql(&_dpc_lock_arr[core_id], HW_IRQL_DISABLED_LEVEL);
            node = linked_list_pop_front(&_dpc_list_arr[core_id]);
            ke_spin_unlock_lower_irql(&_dpc_lock_arr[core_id], irql);
            if (node == NULL)
            {
                break;
            }
            else
            {
                hw_dpc_node_t *dpc_node = OBTAIN_STRUCT_ADDR(node, list_node, hw_dpc_node_t);
                dpc_node->proc(NULL, dpc_node->args);
                dpc_node->free_func(dpc_node, NULL);
            }
        }

        // scheduler DPC
        while (ke_query_and_clear_scheduler_dpc(ke_get_current_core()))
        {
            ke_thread_schedule(&info, NULL);
            if (!scheduled)
            {
                // the prev context is only updated the first time scheduler is called
                prev_context = info.prev_context;
                scheduled = true;
            }
        }

        // timer tick
        if (ke_query_and_clear_timer_dpc(ke_get_current_core()))
        {
            ke_timer_tick(NULL, NULL);
        }

        if (scheduled)
        {
            if (prev_context == NULL)
            {
                ke_context_switch(intr_stack, _dummy_regs, info.next_context);
            }
            else
            {
                ke_context_switch(intr_stack, prev_context, info.next_context);
            }
        }
    }
    return;
}

hw_result_t ke_dpc_setup(uint32_t int_vec)
{
    if (!_dpc_initialized[ke_get_current_core()])
    {
        for (uint32_t i = 0; i < HW_PROC_CNT; i++)
        {
            ke_spin_lock_init(&_dpc_lock_arr[i]);
            linked_list_init(&_dpc_list_arr[i]);
        }
        _dpc_int_vec = int_vec;
        ke_register_intr_handler(_dpc_int_vec, ke_dpc_interrupt_handler, NULL);
        _dpc_initialized[ke_get_current_core()] = true;
        return STATUS_SUCCESS;
    }
    return DPC_STATUS_NOT_INITIALIZED;
}
