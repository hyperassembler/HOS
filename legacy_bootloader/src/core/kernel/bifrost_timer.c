#include <conf_defs.h>
#include <bifrost_timer.h>
#include <bifrost_stdlib.h>
#include <bifrost_alloc.h>
#include <bifrost_apc.h>
#include <bifrost_print.h>
#include "bifrost_assert.h"

static linked_list_t _timer_list[HW_PROC_CNT];
static bool _initialized[HW_PROC_CNT] = {false};
static bool _timer_queued[HW_PROC_CNT] = {false};

static void timer_free(void *timer, void *up)
{
    hw_free(timer);
}

static void timer_node_free(void *timer, void *up)
{
    hw_free(timer);
}

// ==========================
// Ke Functions
// ==========================

hw_result_t ke_timer_init(hw_timer_t *timer,
                          hw_timer_type_t timer_type)
{
    if (timer != NULL)
    {
        linked_list_init(&timer->waiting_threads);

        timer->tick = 0;
        timer->elapsed_tick = 0;

        ke_spin_lock_init(&timer->lock);
        timer->timer_type = timer_type;
        timer->signaled = false;
        timer->periodic = false;
        timer->active = false;
        return STATUS_SUCCESS;
    }
    return TIMER_STATUS_INVALID_ARGUMENTS;
}

hw_result_t ke_timer_setup()
{
    uint32_t coreid = ke_get_current_core();
    if (!_initialized[coreid])
    {
        linked_list_init(&_timer_list[coreid]);
        _initialized[coreid] = true;
    }
    return TIMER_STATUS_SUCCESS;
}


hw_result_t ke_timer_wait(hw_timer_t *timer, hw_timer_node_t *node)
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);

    if (!_initialized[ke_get_current_core()])
    {
        return TIMER_STATUS_NOT_INITIALIZED;
    }

    if (timer == NULL || node == NULL)
    {
        return TIMER_STATUS_INVALID_ARGUMENTS;
    }

    hw_tcb_t *cur_thread = ke_current_thread();
    ke_reference_obj(&cur_thread->ref_node);

    hw_irql_t irql;
    irql = ke_spin_lock_raise_irql(&timer->lock, HW_IRQL_DPC_LEVEL);

    if (!timer->signaled)
    {
        // timer has not expired
        node->tcb = (void *) cur_thread;

        linked_list_push_back(&timer->waiting_threads, &node->list_node);

        ke_thread_block(cur_thread);
    }

    // release the lock
    ke_spin_unlock_lower_irql(&timer->lock, irql);

    return STATUS_SUCCESS;
}

hw_result_t ke_timer_set(hw_timer_t *timer, uint32_t tick, bool periodic)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);
    uint32_t coreid = ke_get_current_core();

    if (!_initialized[coreid])
        return TIMER_STATUS_NOT_INITIALIZED;

    if (timer == NULL || tick == 0)
        return TIMER_STATUS_INVALID_ARGUMENTS;

    hw_irql_t irql;

    // Raising to DPC level prevents anyone
    // else from accessing the timer list
    irql = ke_raise_irql(HW_IRQL_DPC_LEVEL);
    ke_spin_lock(&timer->lock);

    timer->tick = tick;
    timer->elapsed_tick = 0;
    timer->periodic = periodic;
    // reset signaled
    timer->signaled = false;
    if (!timer->active)
    {
        // reference timer for keeping it on the linked list
        ke_reference_obj(&timer->ref_node);
        linked_list_push_back(&_timer_list[coreid], &timer->list_node);
        timer->active = true;
    }

    ke_spin_unlock(&timer->lock);
    ke_lower_irql(irql);

    return STATUS_SUCCESS;

}

hw_result_t ke_timer_cancel(hw_timer_t *timer)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);
    uint32_t coreid = ke_get_current_core();

    if (!_initialized[coreid])
        return TIMER_STATUS_NOT_INITIALIZED;

    if (timer == NULL)
        return TIMER_STATUS_INVALID_ARGUMENTS;

    hw_irql_t irql = ke_raise_irql(HW_IRQL_DPC_LEVEL);
    ke_spin_lock(&timer->lock);

    // simply release from the list
    if (timer->active)
    {
        linked_list_remove_ref(&_timer_list[coreid], &timer->list_node);
        timer->active = false;
        // remove timer for deleting it from the list
        ke_dereference_obj(&timer->ref_node);
    }

    ke_spin_unlock(&timer->lock);
    ke_lower_irql(irql);

    return STATUS_SUCCESS;
}

bool ke_query_and_clear_timer_dpc(uint32_t core)
{
    bool result = _timer_queued[core];
    _timer_queued[core] = false;
    return result;
}

hw_result_t ke_queue_timer_dpc(uint32_t core)
{
    _timer_queued[core] = true;
    return STATUS_SUCCESS;
}

void ke_timer_tick(void *kp, void *up)
{
    hw_assert(ke_get_irql() == HW_IRQL_DPC_LEVEL);
    uint32_t coreid = ke_get_current_core();

    if (!_initialized[coreid])
        return;

    linked_list_node_t *cur_node = linked_list_first(&_timer_list[coreid]);

    while (cur_node != NULL)
    {
        hw_timer_t *timer = OBTAIN_STRUCT_ADDR(cur_node, list_node, hw_timer_t);
        cur_node = linked_list_next(cur_node);

        timer->elapsed_tick++;
        if (timer->elapsed_tick >= timer->tick)
        {
            // if timer expired
            ke_spin_lock(&timer->lock);

            linked_list_node_t *list_node = linked_list_pop_front(&timer->waiting_threads);

            while (list_node != NULL)
            {
                hw_timer_node_t *node = OBTAIN_STRUCT_ADDR(list_node, list_node, hw_timer_node_t);
                ke_thread_resume((hw_tcb_t *) node->tcb);

                list_node = linked_list_pop_front(&timer->waiting_threads);

                ke_dereference_obj(&((hw_tcb_t *) node->tcb)->ref_node);
                node->free_func(node, NULL);
            }


            if(timer->timer_type == TIMER_TYPE_MANUAL_RESET)
            {
                timer->signaled = true;
            }

            if (!timer->periodic)
            {
                // if not periodic, remove from the list
                timer->active = false;
                linked_list_remove_ref(&_timer_list[coreid], &timer->list_node);
                // remove timer for deleting it from the list
                ke_dereference_obj(&timer->ref_node);
            }
            else
            {
                // otherwise still active
                timer->elapsed_tick = 0;
            }

            ke_spin_unlock(&timer->lock);
        }
    }

    return;
}

// ===================
// HW functions
// ===================

hw_result_t hw_timer_wait(hw_handle_t handle)
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);

    hw_result_t result;

    hw_ref_node_t *ref;

    result = hw_open_obj_by_handle(handle, &ref);

    if (!HW_SUCCESS(result))
    {
        return result;
    }

    hw_timer_t *timer = OBTAIN_STRUCT_ADDR(ref, ref_node, hw_timer_t);

    hw_timer_node_t* node = (hw_timer_node_t*)hw_alloc(sizeof(hw_timer_node_t));

    if(node == NULL)
    {
        ke_dereference_obj(&timer->ref_node);
        return TIMER_STATUS_CANNOT_ALLOCATE_MEM;
    }

    node->free_func = timer_node_free;

    result = ke_timer_wait(timer, node);

    ke_dereference_obj(&timer->ref_node);

    return result;
}

hw_result_t hw_timer_set(hw_handle_t handle, uint32_t tick, bool periodic)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    hw_result_t result;

    hw_ref_node_t *ref;

    result = hw_open_obj_by_handle(handle, &ref);

    if (!HW_SUCCESS(result))
    {
        return result;
    }

    hw_timer_t *timer = OBTAIN_STRUCT_ADDR(ref, ref_node, hw_timer_t);

    result = ke_timer_set(timer, tick, periodic);

    ke_dereference_obj(&timer->ref_node);

    return result;
}

hw_result_t hw_timer_cancel(hw_handle_t handle)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    hw_result_t result;

    hw_ref_node_t *ref;

    result = hw_open_obj_by_handle(handle, &ref);

    if (!HW_SUCCESS(result))
    {
        return result;
    }

    hw_timer_t *timer = OBTAIN_STRUCT_ADDR(ref, ref_node, hw_timer_t);

    result = ke_timer_cancel(timer);

    ke_dereference_obj(&timer->ref_node);

    return result;
}

hw_result_t hw_timer_create(hw_handle_t *out,
                            hw_timer_type_t type)
{
    hw_assert(ke_get_irql() < HW_IRQL_DPC_LEVEL);

    hw_result_t result = STATUS_SUCCESS;

    hw_timer_t *timer = (hw_timer_t *) hw_alloc(sizeof(hw_timer_t));

    if (timer == NULL)
        return TIMER_STATUS_CANNOT_ALLOCATE_MEM;

    result = ke_reference_create(&timer->ref_node, timer_free);

    if (!HW_SUCCESS(result))
    {
        hw_free(timer);
        return result;
    }

    result = ke_timer_init(timer, type);

    if (!HW_SUCCESS(result))
    {
        ke_dereference_obj(&timer->ref_node);
        return result;
    }

    result = hw_create_handle(&timer->ref_node, out);

    ke_dereference_obj(&timer->ref_node);

    return result;
}
