#include <bifrost_event.h>
#include <bifrost_thread.h>
#include "bifrost_assert.h"
#include "bifrost_stdlib.h"
#include "bifrost_alloc.h"
#include "bifrost_apc.h"

static void event_node_free(void *node, void *up)
{
    hw_free(node);
}

static void event_free(void *node, void *up)
{
    hw_free(node);
}

// =================
// Ke Functions
// =================

hw_result_t ke_event_wait(hw_event_t *event, hw_event_node_t *node)
{
    if (event == NULL || node == NULL)
    {
        return EVENT_STATUS_INVALID_ARGUMENTS;
    }

    if (event->signaled)
    {
        return STATUS_SUCCESS;
    }

    hw_tcb_t *cur_thread = ke_current_thread();
    ke_reference_obj(&cur_thread->ref_node);
    hw_result_t result;
    hw_irql_t irql;
    irql = ke_spin_lock_raise_irql(&event->lock, HW_IRQL_DPC_LEVEL);

    // TODO: finish tid and check duplicate registration?
    node->tcb = cur_thread;
    linked_list_push_back(&event->waiting_threads, &node->list_node);
    // we want to disable DPC here since hw_block will immediately yield if
    // a thread blocks itself. Otherwise the sem lock will be locked forever resulting in
    // deadlocks. This is only a note since interrupt is disabled here.
    //
    // also thread_block really can't go after unlocking sem_lock, since if before hw_thread_block
    // is called, the scheduler context switch to another thread that signals the sem, then the thread
    // would be unblocked again. If we unblock a thread then block it, it will remain blocked forever.
    //
    // Anyways, hw_thread_block state change must take place simultaneously with the sem_block queue.
    result = ke_thread_block(cur_thread);
    // release the lock
    ke_spin_unlock_lower_irql(&event->lock, irql);

    return result;
}

hw_result_t ke_event_reset(hw_event_t *event)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    if (event != NULL)
    {
        // acquire lock
        hw_irql_t irql;
        irql = ke_spin_lock_raise_irql(&event->lock, HW_IRQL_DPC_LEVEL);

        event->signaled = false;

        ke_spin_unlock_lower_irql(&event->lock, irql);

        return STATUS_SUCCESS;
    }

    return EVENT_STATUS_INVALID_ARGUMENTS;
}

hw_result_t ke_event_signal(hw_event_t *event)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    hw_result_t result = STATUS_SUCCESS;
    if (event != NULL)
    {
        // acquire the lock
        hw_irql_t irql;
        irql = ke_spin_lock_raise_irql(&event->lock, HW_IRQL_DPC_LEVEL);

        if (!event->signaled)
        {
            event->signaled = true;

            linked_list_node_t *node = linked_list_pop_front(&event->waiting_threads);
            while (node != NULL)
            {
                hw_event_node_t *event_node = OBTAIN_STRUCT_ADDR(node, list_node, hw_event_node_t);
                // sync, unblock
                result = ke_thread_resume((hw_tcb_t *) event_node->tcb);
                ke_dereference_obj(&((hw_tcb_t *) event_node->tcb)->ref_node);
                event_node->free_func(event_node, NULL);
                node = linked_list_pop_front(&event->waiting_threads);
            }

            if (event->type == EVENT_TYPE_AUTO)
            {
                event->signaled = false;
            }
        }
        // release the lock
        ke_spin_unlock_lower_irql(&event->lock, irql);
    }
    else
    {
        result = EVENT_STATUS_INVALID_ARGUMENTS;
    }
    return result;
}

hw_result_t ke_event_init(hw_event_t *event, hw_event_type_t event_type)
{
    if (event != NULL)
    {
        linked_list_init(&event->waiting_threads);
        ke_spin_lock_init(&event->lock);
        event->type = event_type;
        event->signaled = false;
        return STATUS_SUCCESS;
    }
    return EVENT_STATUS_INVALID_ARGUMENTS;
}

// =================
// HW Functions
// =================

hw_result_t hw_event_create(hw_handle_t *out, hw_event_type_t event_type)
{
    hw_assert(ke_get_irql() < HW_IRQL_DPC_LEVEL);

    hw_result_t result = STATUS_SUCCESS;

    hw_event_t *event = (hw_event_t *) hw_alloc(sizeof(hw_event_t));

    if (event == NULL)
        return EVENT_STATUS_CANNOT_ALLOCATE_MEM;

    result = ke_reference_create(&event->ref_node, event_free);

    if (!HW_SUCCESS(result))
    {
        hw_free(event);
        return result;
    }

    result = ke_event_init(event, event_type);

    if (!HW_SUCCESS(result))
    {
        ke_dereference_obj(&event->ref_node);
        return result;
    }

    result = hw_create_handle(&event->ref_node, out);

    ke_dereference_obj(&event->ref_node);

    return result;
}

hw_result_t hw_event_signal(hw_handle_t handle)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    hw_result_t result;

    hw_ref_node_t *ref;

    // reference sem pointer
    result = hw_open_obj_by_handle(handle, &ref);

    if (!HW_SUCCESS(result))
    {
        return result;
    }

    hw_event_t *event = OBTAIN_STRUCT_ADDR(ref, ref_node, hw_event_t);

    result = ke_event_signal(event);

    //dereference ref node
    ke_dereference_obj(&event->ref_node);

    return result;
}

hw_result_t hw_event_wait(hw_handle_t handle)
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);

    hw_result_t result;

    hw_ref_node_t *ref;

    // reference sem pointer
    result = hw_open_obj_by_handle(handle, &ref);

    if (!HW_SUCCESS(result))
    {
        return result;
    }

    hw_event_t *event = OBTAIN_STRUCT_ADDR(ref, ref_node, hw_event_t);

    hw_event_node_t *event_node = (hw_event_node_t *) hw_alloc(sizeof(hw_event_node_t));

    if (event_node != NULL)
    {
        event_node->free_func = event_node_free;
        result = ke_event_wait(event, event_node);
    }
    else
    {
        result = EVENT_STATUS_CANNOT_ALLOCATE_MEM;
    }

    ke_dereference_obj(&event->ref_node);
    return result;
}

hw_result_t hw_event_reset(hw_handle_t handle)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    hw_result_t result;

    hw_ref_node_t *ref;

    // reference sem pointer
    result = hw_open_obj_by_handle(handle, &ref);

    if (!HW_SUCCESS(result))
    {
        return result;
    }

    hw_event_t *event = OBTAIN_STRUCT_ADDR(ref, ref_node, hw_event_t);

    result = ke_event_reset(event);

    //dereference ref node
    ke_dereference_obj(&event->ref_node);

    return result;
}
