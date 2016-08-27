/*-----------------------------------------------------
 |
 |      hw_semaphore.c
 |
 |  Provides support for semaphores and mutexes.
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2016  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

#include <bifrost_semaphore.h>
#include "bifrost_ref.h"
#include "bifrost_stdlib.h"
#include "bifrost_semaphore.h"
#include "bifrost_assert.h"
#include "bifrost_thread.h"
#include "bifrost_alloc.h"

static void sem_node_free(void *node, void *up)
{
    hw_free(node);
}

static void sem_free(void *sem, void *up)
{
    hw_free(sem);
}

//
// Ke stuff
//

hw_result_t ke_sem_init(hw_sem_t *sem, int32_t count)
{
    if (sem == NULL)
        return SEM_STATUS_INVALID_ARGUMENTS;
    ke_spin_lock_init(&sem->lock);
    linked_list_init(&sem->block_list);
    sem->count = count;
    return STATUS_SUCCESS;
}

hw_result_t ke_sem_signal(hw_sem_t *sem, int32_t quota)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    if (quota <= 0 || sem == NULL)
        return SEM_STATUS_INVALID_ARGUMENTS;

    hw_result_t result = STATUS_SUCCESS;

    hw_irql_t irq;
    // sem_signal can be shared with higher level interrupts, so irq save
    // also disable DPC
    irq = ke_spin_lock_raise_irql(&sem->lock, HW_IRQL_DPC_LEVEL);
    sem->count += quota;
    while (quota != 0)
    {
        linked_list_node_t *node = linked_list_first(&sem->block_list);
        if (node != NULL)
        {
            hw_sem_node_t *sem_node = OBTAIN_STRUCT_ADDR(node, node, hw_sem_node_t);
            if (sem_node->quota <= quota)
            {
                quota -= sem_node->quota;
                linked_list_pop_front(&sem->block_list);
                result = ke_thread_resume((hw_tcb_t*)sem_node->tcb);
                ke_dereference_obj(&((hw_tcb_t*)sem_node->tcb)->ref_node);
                sem_node->free_callback(sem_node, NULL);
            }
            else
            {
                sem_node->quota -= quota;
                quota = 0;
            }
        }
        else
            break;
    }
    ke_spin_unlock_lower_irql(&sem->lock, irq);

    return result;
}


hw_result_t ke_sem_wait(hw_sem_t *sem, hw_sem_node_t *node, int quota)
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);

    if (sem == NULL || quota <= 0 || node == NULL)
    {
        return SEM_STATUS_INVALID_ARGUMENTS;
    }

    hw_irql_t irq;
    hw_result_t result = STATUS_SUCCESS;
    irq = ke_spin_lock_raise_irql(&sem->lock, HW_IRQL_DPC_LEVEL);
    if (sem->count < quota)
    {
        // if we don't have enough resources
        // if sem->count > 0, then we allocate all existing quota to the thread
        // then wait for the remaining quota requested by the thread
        // else we can't allocate anything then just wait for the quota requested
        int32_t real_quota = sem->count > 0 ? quota - sem->count : quota;
        sem->count -= quota;

        node->quota = real_quota;
        // guaranteed tcb is valid since current thread context
        node->tcb = ke_current_thread();
        // reference for holding a pointer for usage of
        ke_reference_obj(&((hw_tcb_t*)node->tcb)->ref_node);

        linked_list_push_back(&sem->block_list, &node->node);
        // we want to disable DPC here since hw_block will immediately yield if
        // a thread blocks itself. Otherwise the sem lock will be locked forever resulting in
        // deadlocks. This is only a note since interrupt is disabled here.
        //
        // also thread_block really can't go after unlocking sem_lock, since if before hw_thread_block
        // is called, the scheduler context switch to another thread that signals the sem, then the thread
        // would be unblocked again. If we unblock a thread then block it, it will remain blocked forever.
        //
        // Anyways, hw_thread_block state change must take place simultaneously with the sem_block queue.
        result = ke_thread_block((hw_tcb_t*)node->tcb);
    }
    else
    {
        // we have enough resources
        sem->count -= quota;
    }
    ke_spin_unlock_lower_irql(&sem->lock, irq);

    return result;
}

hw_result_t ke_sem_trywait(hw_sem_t* sem, int32_t quota)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    if (quota <= 0 || sem == NULL)
        return SEM_STATUS_INVALID_ARGUMENTS;

    hw_result_t result = SEM_STATUS_OCCUPIED;

    hw_irql_t irq;
    irq = ke_spin_lock_raise_irql(&sem->lock, HW_IRQL_DPC_LEVEL);
    if (sem->count >= quota)
    {
        result = STATUS_SUCCESS;
        sem->count -= quota;
    }
    ke_spin_unlock_lower_irql(&sem->lock, irq);

    return result;
}

//
// hw thingys
//

hw_result_t hw_sem_create(hw_handle_t *out, int32_t count)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    hw_result_t result = STATUS_SUCCESS;

    hw_sem_t *sem = (hw_sem_t *) hw_alloc(sizeof(hw_sem_t));

    if (sem == NULL)
        return SEM_STATUS_CANNOT_ALLOCATE_MEM;

    result = ke_reference_create(&sem->ref_node, sem_free);

    if (!HW_SUCCESS(result))
    {
        hw_free(sem);
        return result;
    }

    result = ke_sem_init(sem, count);

    if (!HW_SUCCESS(result))
    {
        ke_dereference_obj(&sem->ref_node);
        return result;
    }

    result = hw_create_handle(&sem->ref_node, out);

    ke_dereference_obj(&sem->ref_node);

    return result;
}

hw_result_t hw_sem_wait(hw_handle_t handle, int32_t quota)
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);
    if (quota <= 0)
        return SEM_STATUS_INVALID_ARGUMENTS;

    hw_result_t result;
    hw_ref_node_t *ref;
    // reference sem pointer
    result = hw_open_obj_by_handle(handle, &ref);

    if (!HW_SUCCESS(result))
    {
        return result;
    }

    hw_sem_t *sem = OBTAIN_STRUCT_ADDR(ref, ref_node, hw_sem_t);
    hw_sem_node_t* sem_node = (hw_sem_node_t*)hw_alloc(sizeof(hw_sem_node_t));

    if(sem_node == NULL)
    {
        ke_dereference_obj(&sem->ref_node);
        return SEM_STATUS_CANNOT_ALLOCATE_MEM;
    }

    sem_node->free_callback = sem_node_free;

    ke_sem_wait(sem, sem_node, quota);

    // dereference sem pointer
    ke_dereference_obj(&sem->ref_node);

    return result;
}

hw_result_t hw_sem_signal(hw_handle_t handle, int32_t quota)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    if (quota <= 0)
        return SEM_STATUS_INVALID_ARGUMENTS;

    hw_result_t result;

    hw_ref_node_t *ref;

    // reference sem pointer
    result = hw_open_obj_by_handle(handle, &ref);

    if (!HW_SUCCESS(result))
    {
        return result;
    }

    hw_sem_t *sem = OBTAIN_STRUCT_ADDR(ref, ref_node, hw_sem_t);

    result = ke_sem_signal(sem, quota);

    //dereference ref node
    ke_dereference_obj(&sem->ref_node);

    return result;
}

hw_result_t hw_sem_trywait(hw_handle_t handle, int32_t quota)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    if (quota <= 0)
        return SEM_STATUS_INVALID_ARGUMENTS;

    hw_result_t result;

    hw_ref_node_t *ref;

    // reference sem pointer
    result = hw_open_obj_by_handle(handle, &ref);

    if (!HW_SUCCESS(result))
    {
        return result;
    }

    hw_sem_t *sem = OBTAIN_STRUCT_ADDR(ref, ref_node, hw_sem_t);

    result = ke_sem_trywait(sem, quota);

    ke_dereference_obj(&sem->ref_node);

    return result;
}
