#include <bifrost_timer.h>
#include <bifrost_thread.h>
#include "bifrost_apc.h"
#include "bifrost_context.h"
#include "bifrost_rwlock.h"
#include "bifrost_dpc.h"
#include "bifrost_mem.h"
#include "conf_defs.h"
#include "bifrost_stdlib.h"
#include "bifrost_alloc.h"
#include "bifrost_system_constants.h"
#include "bifrost_assert.h"

// The global AVL tree is only for fast TCB lookup
static MEM_SRAM_UC hw_rwlock_t _tree_lock;
static avl_tree_t _thread_tree;

// Represents the current thread
static hw_tcb_t *_current_thread[HW_PROC_CNT];

// These lists are per core X per state X per priority
// no lock needed since only the sheduler of each core modifies this
static linked_list_t _scheduler_queue[HW_PROC_CNT][STATE_NUM - 1][PRIORITY_LEVEL_NUM];
static linked_list_t _scheduler_notif_queue[HW_PROC_CNT];
static hw_spin_lock_t _scheduler_lock[HW_PROC_CNT];
static bool _scheduler_queued[HW_PROC_CNT] = {false};

// The global non-decreasing thread_id counter
static int32_t _thread_id_count;

// The global variable signifying whether the library is initialized
static _Bool _initialized[HW_PROC_CNT] = {false};
static MEM_SRAM_UC int32_t _avl_initialized = 0;
static volatile _Bool _thread_avl_initialized = false;

//==================================
// TCB helper Routines
//==================================

static int thread_tree_compare(avl_tree_node_t *tree_node, avl_tree_node_t *my_node)
{
    hw_tcb_t *tcb = OBTAIN_STRUCT_ADDR(tree_node, tree_node, hw_tcb_t);
    hw_tcb_t *my_tcb = OBTAIN_STRUCT_ADDR(my_node, tree_node, hw_tcb_t);
    return tcb->thread_id - my_tcb->thread_id;
}

static void tcb_free(void *tcb, void *up)
{
    hw_free(tcb);
    return;
}

// scheduler lock must be held before calling this thing
static void notify_scheduler(hw_tcb_t *thread_handle)
{
    if (!thread_handle->in_scheduler_queue)
    {
        linked_list_push_back(&_scheduler_notif_queue[thread_handle->core_id], &thread_handle->scheduler_queue_node);
        thread_handle->in_scheduler_queue = true;
    }
    return;
}

//==================================
// Per Processor NULL proc
//==================================
static hw_handle_t _null_proc_handles[HW_PROC_CNT];

static void null_proc(void *par)
{
    while (1)
    {
        //hw_printf("%s: running\n", __func__);
        ke_thread_yield(ke_get_current_core());
    }
}

static inline _Bool is_thread_initialized()
{
    return _initialized[ke_get_current_core()] && _avl_initialized == 1;
}

//==================================
// Ke Functions
//==================================

hw_tcb_t *ke_current_thread()
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);
    return _current_thread[ke_get_current_core()];
}

hw_result_t ke_thread_open(int32_t id, hw_tcb_t **out)
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);

    if (!is_thread_initialized())
        return THREAD_STATUS_UNINITIALIZED;

    if (out == NULL)
        return THREAD_STATUS_INVALID_ARGUMENT;

    hw_result_t result;
    avl_tree_node_t *temp;
    hw_tcb_t *target = NULL;

    hw_tcb_t clone;
    clone.thread_id = id;

    hw_irql_t irql = ke_reader_lock_raise_irql(&_tree_lock, HW_IRQL_APC_LEVEL);
    temp = avl_tree_search(&_thread_tree, &clone.tree_node);
    if (temp != NULL)
    {
        // reference for having a pointer
        target = OBTAIN_STRUCT_ADDR(temp, tree_node, hw_tcb_t);
        result = ke_reference_obj(&target->ref_node);
    }
    else
    {
        result = THREAD_STATUS_INVALID_ARGUMENT;
    }
    ke_reader_unlock_lower_irql(&_tree_lock, irql);

    if (HW_SUCCESS(result))
    {
        *out = target;
    }
    return result;
}

void ke_thread_schedule(void *info, void *up)
{
    hw_assert(ke_get_irql() == HW_IRQL_DPC_LEVEL);
    hw_thread_schedule_info_t *result = (hw_thread_schedule_info_t *) info;
    if (result == NULL || !is_thread_initialized())
        return;

    uint32_t coreid = ke_get_current_core();

    ke_spin_lock(&_scheduler_lock[coreid]);

    // take care of the notification queue first
    linked_list_node_t *node = linked_list_pop_front(&_scheduler_notif_queue[coreid]);
    while (node != NULL)
    {
        hw_tcb_t *tcb = OBTAIN_STRUCT_ADDR(node, scheduler_queue_node, hw_tcb_t);
        // target thread is not the currently running thread
        if (tcb != _current_thread[coreid])
        {
            if (tcb->state == STATE_OUTSIDE)
            {
                // this means the target thread is being deleted ->
                // detach the thread from scheduler queue and deref the thread
                linked_list_remove_ref(&_scheduler_queue[coreid][tcb->location][tcb->priority],
                                       &tcb->list_node);
                ke_dereference_obj(&tcb->ref_node);
            }
            else
            {
                // target thread is being moved around
                if (tcb->location != STATE_OUTSIDE)
                {
                    // this means not a new thread, need to detach from the prev queue
                    linked_list_remove_ref(&_scheduler_queue[coreid][tcb->location][tcb->priority],
                                           &tcb->list_node);
                }

                // add to the next queue
                linked_list_push_back(&_scheduler_queue[coreid][tcb->state][tcb->priority],
                                      &tcb->list_node);

                // update the location
                tcb->location = tcb->state;
            }
        }
        // else
        // {
        // the target thread is the currently running thread
        // in this case we don't move it around the queues just yet
        // }

        // reset the tcb queued flag
        tcb->in_scheduler_queue = false;
        node = linked_list_pop_front(&_scheduler_notif_queue[coreid]);
    }


    // handle the currently running thread state change
    // and pick a new thread
    hw_tcb_t *old_tcb = _current_thread[coreid];
    // check current thread -> block to block queue, run to ready queue, exit to exit queue
    if (old_tcb != NULL)
    {
        if (old_tcb->state == STATE_OUTSIDE)
        {
            result->prev_context = NULL;
            ke_dereference_obj(&old_tcb->ref_node);
        }
        else
        {
            result->prev_context = old_tcb->regs;

            if (old_tcb->state == STATE_RUN)
            {
                // if nothing is changed about the thread
                // change its state to ready
                old_tcb->state = STATE_READY;
            }

            // move to whatever old_tcb state is
            linked_list_push_back(&_scheduler_queue[coreid][old_tcb->state][old_tcb->priority],
                                  &old_tcb->list_node);

            // update location
            old_tcb->location = old_tcb->state;
        }
    }
    else
    {
        result->prev_context = NULL;
    }

    // pick a new thread
    for (int i = 0; i < PRIORITY_LEVEL_NUM; i++)
    {
        linked_list_node_t *front;
        front = linked_list_pop_front(&_scheduler_queue[coreid][STATE_READY][i]);
        if (front != NULL)
        {
            _current_thread[coreid] = OBTAIN_STRUCT_ADDR(front, list_node, hw_tcb_t);
            _current_thread[coreid]->state = STATE_RUN;
            result->next_context = _current_thread[coreid]->regs;

            // update the location
            _current_thread[coreid]->location = STATE_RUN;
            break;
        }
    }

    ke_spin_unlock(&_scheduler_lock[coreid]);

    // check if there is a queued APC, if there is then fire an APC interrupt
    if (linked_list_first(&_current_thread[coreid]->apc_list) != NULL)
    {
        ke_apc_drain(coreid);
    }

    return;
}

//
// this assumes that tcb is referenced once already
//
hw_result_t ke_thread_create(hw_tcb_t *tcb,
                             void (*proc)(void *),
                             void *args,
                             hw_thread_priority_t priority,
                             uint32_t stack_size,
                             void *stack_ptr)
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);

    if (!is_thread_initialized())
        return THREAD_STATUS_UNINITIALIZED;
    if (proc == NULL || tcb == NULL || stack_ptr == NULL || stack_size == 0)
        return THREAD_STATUS_INVALID_ARGUMENT;

    hw_irql_t irql;
    int32_t alloc_thread_id;
    uint32_t coreid = ke_get_current_core();

    alloc_thread_id = ke_interlocked_increment(&_thread_id_count, 1);

    if (alloc_thread_id == THREAD_INVALID_PID)
    {
        return THREAD_STATUS_ID_OVERFLOW;
    }

    tcb->thread_id = alloc_thread_id;
    tcb->proc = proc;
    tcb->stack_ptr = stack_ptr;
    tcb->args = args;
    tcb->priority = priority;
    tcb->stack_size = stack_size;
    tcb->location = STATE_OUTSIDE;
    tcb->core_id = ke_get_current_core();
    tcb->exit_code = 0;
    tcb->initialized = false;
    tcb->in_scheduler_queue = false;
    ke_create_context(tcb->regs, (void *) tcb->proc, tcb->stack_ptr, HW_IRQL_USER_LEVEL, args);
    ke_spin_lock_init(&tcb->apc_lock);
    linked_list_init(&tcb->apc_list);

    hw_result_t result;

    result = ke_event_init(&tcb->thread_exit_event, EVENT_TYPE_MANUAL);

    if (!HW_SUCCESS(result))
    {
        return result;
    }

    // reference the TCB for avl tree
    result = ke_reference_obj(&tcb->ref_node);

    if (HW_SUCCESS(result))
    {
        // reference the TCB for scheduler
        result = ke_reference_obj(&tcb->ref_node);

        if (!HW_SUCCESS(result))
        {
            // deref the tcb for avl tree
            ke_dereference_obj(&tcb->ref_node);
        }
    }

    if (HW_SUCCESS(result))
    {
        // write avl tree and issue command
        // we go to DPC level here since AVL insertion and notif node insertion
        // must take place atomically
        // (The order of notif node matters)
        irql = ke_writer_lock_raise_irql(&_tree_lock, HW_IRQL_APC_LEVEL);
        ke_spin_lock_raise_irql(&_scheduler_lock[coreid], HW_IRQL_DPC_LEVEL);

        avl_tree_insert(&_thread_tree, &tcb->tree_node);

        tcb->state = STATE_NEW;
        notify_scheduler(tcb);

        ke_spin_unlock_lower_irql(&_scheduler_lock[coreid], HW_IRQL_APC_LEVEL);
        ke_writer_unlock_lower_irql(&_tree_lock, irql);
    }

    return result;
}

//
// this assumes that tcb is referenced once already
//
hw_result_t ke_thread_start(hw_tcb_t *tcb)
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);
    if (!is_thread_initialized())
        return THREAD_STATUS_UNINITIALIZED;
    hw_result_t result = STATUS_SUCCESS;

    if (tcb == NULL)
    {
        result = THREAD_STATUS_INVALID_ARGUMENT;
    }
    else
    {
        hw_irql_t irql;

        irql = ke_spin_lock_raise_irql(&_scheduler_lock[tcb->core_id], HW_IRQL_DPC_LEVEL);
        if (tcb->state == STATE_NEW)
        {
            tcb->state = STATE_READY;
            notify_scheduler(tcb);
        }
        else
        {
            result = THREAD_STATUS_INVALID_STATE;
        }
        ke_spin_unlock_lower_irql(&_scheduler_lock[tcb->core_id], irql);
    }

    return result;
}

//
// this assumes that tcb is referenced once already
//
hw_result_t ke_thread_terminate(hw_tcb_t *tcb)
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);

    if (!is_thread_initialized())
        return THREAD_STATUS_UNINITIALIZED;
    hw_result_t result;
    hw_irql_t irql;

    if (tcb == NULL)
    {
        result = THREAD_STATUS_INVALID_ARGUMENT;
    }
    else
    {
        // if the thread is running on the target core, then schedule an DPC
        irql = ke_raise_irql(HW_IRQL_DPC_LEVEL);
        ke_spin_lock(&_scheduler_lock[tcb->core_id]);

        hw_thread_state_t old_state = tcb->state;

        if (old_state != STATE_EXIT)
        {
            tcb->exit_code = THREAD_EXIT_CODE_TERMINATED;
            tcb->state = STATE_EXIT;
            notify_scheduler(tcb);
        }

        if (old_state == STATE_RUN)
        {
            // yielding here works since the irql is set to DPC_LEVEL, which masks off DPC
            // interrupts, yield will trigger once we hw_unlock_irq_restore.
            // if it's on another core, trivial, definitely works
            ke_thread_yield(tcb->core_id);
        }

        ke_spin_unlock(&_scheduler_lock[tcb->core_id]);

        // event signal should happen at DPC LEVEL and when scheduler lock is released
        result = ke_event_signal(&tcb->thread_exit_event);
        ke_lower_irql(irql);
    }

    return result;
}

//
// get exit code also informs the kernel that the thread can be freed
//
hw_result_t ke_thread_get_exit_code(hw_tcb_t *tcb, int32_t *exit_code)
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);

    if (!is_thread_initialized())
        return THREAD_STATUS_UNINITIALIZED;


    if (tcb == NULL)
    {
        return THREAD_STATUS_INVALID_ARGUMENT;
    }
    hw_result_t result = STATUS_SUCCESS;
    hw_irql_t irql;
    irql = ke_writer_lock_raise_irql(&_tree_lock, HW_IRQL_APC_LEVEL);
    ke_spin_lock_raise_irql(&_scheduler_lock[tcb->core_id], HW_IRQL_DPC_LEVEL);
    if (tcb->state == STATE_EXIT)
    {
        if(exit_code != NULL)
        {
            *exit_code = tcb->exit_code;
        }
        tcb->state = STATE_OUTSIDE;
        notify_scheduler(tcb);
        avl_tree_delete(&_thread_tree, &tcb->tree_node);
        ke_dereference_obj(&tcb->ref_node);
    }
    else
    {
        result = THREAD_STATUS_INVALID_STATE;
    }
    ke_spin_unlock_lower_irql(&_scheduler_lock[tcb->core_id], HW_IRQL_APC_LEVEL);
    ke_writer_unlock_lower_irql(&_tree_lock, irql);

    return result;
}

void ke_thread_exit(int32_t exit_code)
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);

    if (is_thread_initialized())
    {
        hw_irql_t irql;
        uint32_t coreid = ke_get_current_core();
        // Don't need a reference here since the context is already current thread
        hw_tcb_t *current_thread = ke_current_thread();
        irql = ke_raise_irql(HW_IRQL_DPC_LEVEL);
        ke_spin_lock(&_scheduler_lock[coreid]);

        current_thread->exit_code = exit_code;
        current_thread->state = STATE_EXIT;
        notify_scheduler(current_thread);

        ke_thread_yield(coreid);

        ke_spin_unlock(&_scheduler_lock[coreid]);

        // signal the event at DPC and when scheduler lock is unlocked
        ke_event_signal(&current_thread->thread_exit_event);
        ke_lower_irql(irql);
    }
}

hw_result_t ke_thread_block(hw_tcb_t *tcb)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    if (!is_thread_initialized())
        return THREAD_STATUS_UNINITIALIZED;
    hw_result_t result = STATUS_SUCCESS;

    if (tcb == NULL)
    {
        return THREAD_STATUS_INVALID_ARGUMENT;
    }
    else
    {
        hw_irql_t irql;
        irql = ke_spin_lock_raise_irql(&_scheduler_lock[tcb->core_id], HW_IRQL_DPC_LEVEL);

        hw_thread_state_t old_state = tcb->state;

        if (old_state == STATE_READY || old_state == STATE_NEW || old_state == STATE_RUN)
        {
            tcb->state = STATE_BLOCK;
            notify_scheduler(tcb);
            if (old_state == STATE_RUN)
            {
                ke_thread_yield(tcb->core_id);
            }
        }
        else
        {
            result = THREAD_STATUS_INVALID_STATE;
        }

        ke_spin_unlock_lower_irql(&_scheduler_lock[tcb->core_id], irql);
    }
    return result;
}

hw_result_t ke_thread_resume(hw_tcb_t *tcb)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);

    if (!is_thread_initialized())
        return THREAD_STATUS_UNINITIALIZED;

    hw_result_t result = STATUS_SUCCESS;
    if (tcb == NULL)
    {
        result = THREAD_STATUS_INVALID_ARGUMENT;
    }
    else
    {
        hw_irql_t irql;
        irql = ke_spin_lock_raise_irql(&_scheduler_lock[tcb->core_id], HW_IRQL_DPC_LEVEL);
        if (tcb->state == STATE_BLOCK)
        {
            tcb->state = STATE_READY;
            notify_scheduler(tcb);
        }
        else
        {
            result = THREAD_STATUS_INVALID_STATE;
        }
        ke_spin_unlock_lower_irql(&_scheduler_lock[tcb->core_id], irql);
    }

    return result;
}


hw_result_t ke_thread_yield(uint32_t core)
{
    ke_queue_scheduler_dpc(core);
    return ke_dpc_drain(core);
}


//==================================
// HW Functions
//==================================
hw_result_t hw_thread_setup()
{
    uint32_t coreid = ke_get_current_core();

    if (ke_interlocked_exchange(&_avl_initialized, 1) == 0)
    {
        // if avl not initialized, then do it
        avl_tree_init(&_thread_tree, thread_tree_compare);
        ke_rwlock_init(&_tree_lock);
        _thread_id_count = 0;

        _thread_avl_initialized = true;
    }
    else
    {
        while (!_thread_avl_initialized);
    }

    hw_result_t result = STATUS_SUCCESS;

    if (!_initialized[coreid])
    {
        linked_list_init(&_scheduler_notif_queue[coreid]);
        ke_spin_lock_init(&_scheduler_lock[coreid]);
        _current_thread[coreid] = NULL;
        for (int j = 0; j < STATE_NUM - 1; j++)
        {
            for (int k = 0; k < PRIORITY_LEVEL_NUM; k++)
            {
                linked_list_init(&_scheduler_queue[coreid][j][k]);
            }
        }
        _initialized[coreid] = true;
        result = hw_thread_create(null_proc, NULL, PRIORITY_LOWEST, THREAD_DEFAULT_STACK_SIZE,
                                  &_null_proc_handles[coreid]);
        if (HW_SUCCESS(result))
        {
            result = hw_thread_start(_null_proc_handles[coreid]);
        }
    }

    return result;
}

hw_result_t hw_thread_create(void (*proc)(void *),
                             void *args,
                             hw_thread_priority_t priority,
                             uint32_t stack_size,
                             hw_handle_t *thread_handle)
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);

    if (!is_thread_initialized())
        return THREAD_STATUS_UNINITIALIZED;
    if (proc == NULL || thread_handle == NULL)
        return THREAD_STATUS_INVALID_ARGUMENT;

    if (stack_size == 0)
    {
        stack_size = THREAD_DEFAULT_STACK_SIZE;
    }

    hw_tcb_t *tcb = (hw_tcb_t *) hw_alloc(sizeof(hw_tcb_t) + stack_size);

    if (tcb == NULL)
    {
        return THREAD_STATUS_OUT_OF_MEMORY;
    }

    void *stack = (char *) tcb + sizeof(hw_tcb_t) + stack_size;

    // reference for the tcb pointer this function owns
    hw_result_t result = ke_reference_create(&tcb->ref_node, tcb_free);

    if (!HW_SUCCESS(result))
    {
        hw_free(tcb);
        return result;
    }

    result = hw_create_handle(&tcb->ref_node, thread_handle);

    if (!HW_SUCCESS(result))
    {
        hw_free(tcb);
        return result;
    }

    // FINISHED CREATING REFERABLE tcb object

    result = ke_thread_create(tcb,
                              proc,
                              args,
                              priority,
                              stack_size,
                              stack);


    if (!HW_SUCCESS(result))
    {
        hw_close_handle(*thread_handle);
    }

    // dereference for the tcb pointer this function owns
    ke_dereference_obj(&tcb->ref_node);

    return result;
}

hw_result_t hw_thread_start(hw_handle_t thread_handle)
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);
    if (!is_thread_initialized())
        return THREAD_STATUS_UNINITIALIZED;
    hw_result_t result;
    hw_ref_node_t *ref;
    result = hw_open_obj_by_handle(thread_handle, &ref);

    if (HW_SUCCESS(result))
    {
        hw_tcb_t *tcb = OBTAIN_STRUCT_ADDR(ref, ref_node, hw_tcb_t);
        result = ke_thread_start(tcb);
        ke_dereference_obj(&tcb->ref_node);
    }

    return result;
}

hw_result_t hw_thread_terminate(hw_handle_t thread_handle)
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);
    if (!is_thread_initialized())
        return THREAD_STATUS_UNINITIALIZED;
    hw_result_t result;
    hw_ref_node_t *ref;
    result = hw_open_obj_by_handle(thread_handle, &ref);

    if (HW_SUCCESS(result))
    {
        hw_tcb_t *tcb = OBTAIN_STRUCT_ADDR(ref, ref_node, hw_tcb_t);
        result = ke_thread_terminate(tcb);
        ke_dereference_obj(&tcb->ref_node);
    }

    return result;
}

hw_result_t hw_thread_get_exit_code(hw_handle_t thread_handle, int32_t *exit_code)
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);
    if (!is_thread_initialized())
        return THREAD_STATUS_UNINITIALIZED;
    hw_result_t result;
    hw_ref_node_t *ref;
    result = hw_open_obj_by_handle(thread_handle, &ref);

    if (HW_SUCCESS(result))
    {
        hw_tcb_t *tcb = OBTAIN_STRUCT_ADDR(ref, ref_node, hw_tcb_t);
        result = ke_thread_get_exit_code(tcb, exit_code);
        ke_dereference_obj(&tcb->ref_node);
    }

    return result;
}

void hw_thread_exit(int32_t exit_code)
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);
    if (is_thread_initialized())
    {
        ke_thread_exit(exit_code);
    }
    return;
}

hw_result_t hw_thread_block(hw_handle_t thread_handle)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);
    if (!is_thread_initialized())
        return THREAD_STATUS_UNINITIALIZED;
    hw_result_t result;
    hw_ref_node_t *ref;
    result = hw_open_obj_by_handle(thread_handle, &ref);

    if (HW_SUCCESS(result))
    {
        hw_tcb_t *tcb = OBTAIN_STRUCT_ADDR(ref, ref_node, hw_tcb_t);
        result = ke_thread_block(tcb);
        ke_dereference_obj(&tcb->ref_node);
    }

    return result;
}

hw_result_t hw_thread_resume(hw_handle_t thread_handle)
{
    hw_assert(ke_get_irql() <= HW_IRQL_DPC_LEVEL);
    if (!is_thread_initialized())
        return THREAD_STATUS_UNINITIALIZED;
    hw_result_t result;
    hw_ref_node_t *ref;
    result = hw_open_obj_by_handle(thread_handle, &ref);

    if (HW_SUCCESS(result))
    {
        hw_tcb_t *tcb = OBTAIN_STRUCT_ADDR(ref, ref_node, hw_tcb_t);
        result = ke_thread_resume(tcb);
        ke_dereference_obj(&tcb->ref_node);
    }

    return result;
}

int32_t ke_current_thread_id()
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);
    // NO need to reference since current thread context
    hw_tcb_t *tcb = ke_current_thread();
    return tcb == NULL ? THREAD_INVALID_PID : tcb->thread_id;
}

int32_t hw_current_thread_id()
{
    return ke_current_thread_id();
}

hw_handle_t hw_current_thread()
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);
    return HW_HANDLE_CURRENT_THREAD;
}


bool ke_query_and_clear_scheduler_dpc(uint32_t core)
{
    bool result = _scheduler_queued[core];
    _scheduler_queued[core] = false;
    return result;
}

hw_result_t ke_queue_scheduler_dpc(uint32_t core)
{
    _scheduler_queued[core] = true;
    return STATUS_SUCCESS;
}

hw_result_t hw_wait_for_thread_exit(hw_handle_t thread_handle)
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);

    hw_result_t result;
    hw_ref_node_t *ref;
    result = hw_open_obj_by_handle(thread_handle, &ref);

    if (!HW_SUCCESS(result))
    {
        return result;
    }

    hw_event_node_t *event_node = (hw_event_node_t *) hw_alloc(sizeof(hw_event_node_t));

    if (event_node == NULL)
    {
        return THREAD_STATUS_OUT_OF_MEMORY;
    }

    // TODO: works but extremely bad
    event_node->free_func = tcb_free;

    hw_tcb_t *tcb = OBTAIN_STRUCT_ADDR(ref, ref_node, hw_tcb_t);

    result = ke_event_wait(&tcb->thread_exit_event, event_node);

    ke_dereference_obj(&tcb->ref_node);

    return result;
}

hw_result_t hw_thread_assert_state(hw_handle_t thread_handle, hw_thread_state_t state)
{
    if (state == STATE_OUTSIDE)
        return THREAD_STATUS_INVALID_ARGUMENT;
    hw_result_t result = STATUS_SUCCESS;

    hw_ref_node_t *ref;
    result = hw_open_obj_by_handle(thread_handle, &ref);

    if (HW_SUCCESS(result))
    {
        hw_tcb_t *tcb = OBTAIN_STRUCT_ADDR(ref, ref_node, hw_tcb_t);
        hw_irql_t irql;
        uint32_t coreid = tcb->core_id;
        irql = ke_spin_lock_raise_irql(&_scheduler_lock[coreid], HW_IRQL_DPC_LEVEL);

        if (tcb->state != state)
        {
            result = THREAD_STATUS_INVALID_STATE;
        }

        if (HW_SUCCESS(result))
        {
            result = THREAD_STATUS_INVALID_STATE;
            if (state == STATE_RUN)
            {
                if (_current_thread[tcb->core_id] == tcb)
                {
                    result = STATUS_SUCCESS;
                }
            }
            else
            {
                linked_list_node_t *node = linked_list_first(&_scheduler_queue[coreid][state][tcb->priority]);
                while (node != NULL)
                {
                    hw_tcb_t *tcb2 = OBTAIN_STRUCT_ADDR(node, list_node, hw_tcb_t);
                    if (tcb2 == tcb)
                    {
                        result = STATUS_SUCCESS;
                        break;
                    }
                    node = linked_list_next(node);
                }
            }
        }
        ke_spin_unlock_lower_irql(&_scheduler_lock[coreid], irql);

        ke_dereference_obj(&tcb->ref_node);
    }

    return result;
}

hw_result_t hw_thread_sleep(uint32_t millis)
{
    if (millis == 0)
        return STATUS_SUCCESS;
    hw_handle_t timer;
    hw_result_t result;
    result = hw_timer_create(&timer, TIMER_TYPE_MANUAL_RESET);

    if (!HW_SUCCESS(result))
    {
        return result;
    }

    result = hw_timer_set(timer, millis, false);

    if (!HW_SUCCESS(result))
    {
        hw_close_handle(timer);
        return result;
    }

    result = hw_timer_wait(timer);

    hw_close_handle(timer);

    return result;
}


hw_result_t hw_thread_open(int32_t thread_id, hw_handle_t *out)
{
    hw_assert(ke_get_irql() <= HW_IRQL_APC_LEVEL);

    if (!is_thread_initialized())
        return THREAD_STATUS_UNINITIALIZED;

    if (out == NULL)
    {
        return THREAD_STATUS_INVALID_ARGUMENT;
    }

    hw_result_t result;
    hw_tcb_t *target = NULL;

    result = ke_thread_open(thread_id, &target);

    if (HW_SUCCESS(result))
    {
        // only enter here if ptr ref
        // if we successfully referenced it, then create a handle
        result = hw_create_handle(&target->ref_node, out);

        // don't need ptr anymore, deref the pointer
        ke_dereference_obj(&target->ref_node);
    }

    return result;
}
