#include "bifrost_thread.h"
#include "avl_tree.h"
#include <stdint.h>

// global structures
static hw_tcb_t *g_core_current_thread[HW_PROC_CNT];

extern UINT32 ts_arch_context_switch_ASM(void *curTestRegs, void *targetTestRegs);
extern UINT32 ts_arch_resume_ASM(void *curTestRegs, void *targetTestRegs);

static HW_LOCK g_avl_lock;
static avl_tree_t g_global_thread_tree;

static HW_LOCK g_core_lock[HW_PROC_CNT];
static linked_list_t g_core_thread_list[HW_PROC_CNT][STATE_NUM - 1][PRIORITY_LEVEL_NUM];

static UINT32 g_thread_id_count;
static HW_LOCK g_thread_id_count_lock;
static _Bool initialized = false;

static INT32 _thread_id_compare(avl_tree_node_t *tree_node, avl_tree_node_t *my_node)
{
    hw_tcb_t *tcb = OBTAIN_STRUCT_ADDR(tree_node, tree_node, hw_tcb_t);
    hw_tcb_t *my_tcb = OBTAIN_STRUCT_ADDR(my_node, tree_node, hw_tcb_t);
    return tcb->thread_id - my_tcb->thread_id;
}

static _Bool _thread_id_equal(linked_list_node_t *list_node, linked_list_node_t *my_node)
{
    hw_tcb_t *tcb = OBTAIN_STRUCT_ADDR(list_node, list_node, hw_tcb_t);
    hw_tcb_t *my_tcb = OBTAIN_STRUCT_ADDR(my_node, list_node, hw_tcb_t);
    return tcb->thread_id == my_tcb->thread_id;
}

static hw_tcb_t *_get_tcb_by_id(uint32_t thread_id)
{
    avl_tree_node_t *temp;
    hw_tcb_t clone, *result;
    clone.thread_id = thread_id;
    temp = avl_tree_search(&g_global_thread_tree, &clone.tree_node);
    return temp == NULL ? NULL : OBTAIN_STRUCT_ADDR(temp, tree_node, hw_tcb_t);
}

static void _adjust_queue(int core_num, hw_thread_state_t old, hw_thread_state_t new, hw_thread_state_t cond)
{
    for (int priority = 0; priority < PRIORITY_LEVEL_NUM; priority++)
    {
        linked_list_t *list = &g_core_thread_list[core_num][old][priority];
        linked_list_node_t *cur = linked_list_first(list);
        uint32_t index = 0;
        while (cur != NULL)
        {
            hw_tcb_t *tcb = OBTAIN_STRUCT_ADDR(cur, list_node, hw_tcb_t);
            cur = linked_list_next(cur);
            if (tcb->state == cond)
            {
                linked_list_remove(list, index);
                linked_list_push_back(&g_core_thread_list[core_num][new][priority], &tcb->list_node);
                index--;
            }
            index++;
        }
    }
}

//
// THE NULL PROC
//
static UINT32 _null_proc_id;
static void _null_proc(void* par)
{
    while(1)
    {
        hw_printf("==NULL PROC==\n");
    }
}

// Absolutely unexposed routine. Only extern
// DPC Level
static UINT64 _dummy_regs[16];

void hw_thread_schedule()
{
    if(initialized)
    {
        const int core_num = hw_getCoreNum();
        hw_tcb_t *old_tcb = g_core_current_thread[core_num];
        hw_lock(&g_core_lock[core_num], false);
        // move anything ready from new queue to ready queue
        _adjust_queue(core_num, NEW, READY, READY);

        // move anything blocked from ready queue to block queue
        _adjust_queue(core_num, READY, BLOCK, BLOCK);

        // move anything ready from block queue to ready queue
        _adjust_queue(core_num, BLOCK, READY, READY);

        // check current thread -> block to block queue, run to ready queue, exit to exit queue
        if (old_tcb != NULL)
        {
            if (old_tcb->state == BLOCK)
            {
                linked_list_push_back(&g_core_thread_list[core_num][BLOCK][old_tcb->priority], &old_tcb->list_node);
            }
            else if (old_tcb->state == RUN)
            {
                old_tcb->state = READY;
                linked_list_push_back(&g_core_thread_list[core_num][READY][old_tcb->priority], &old_tcb->list_node);
            }
            else if (old_tcb->state == EXIT)
            {
                linked_list_push_back(&g_core_thread_list[core_num][EXIT][old_tcb->priority], &old_tcb->list_node);
            }
            else
            {
                // CRITICAL ERROR
                // DISCARD THE ERRONEOUS THREAD FOREVER
            }
        }

        // pick a new thread and run
        for (int i = 0; i < PRIORITY_LEVEL_NUM; i++)
        {
            linked_list_node_t *front;
            front = linked_list_pop_front(&g_core_thread_list[core_num][READY][i]);
            if (front != NULL)
            {
                g_core_current_thread[core_num] = OBTAIN_STRUCT_ADDR(front, list_node, hw_tcb_t);
                g_core_current_thread[core_num]->state = RUN;

                break;
            }
        }
        hw_unlock(&g_core_lock[core_num], false);

        if(g_core_current_thread[core_num] != NULL)
        {
            hw_printf("SELECTED: %d\n", g_core_current_thread[core_num]->thread_id);
            // Spills window registers to the current test's stack
            xthal_window_spill();
            if (g_core_current_thread[core_num]->initialized)
            {
                // Transfer control with a jump to prevent doing anything with window registers
                __asm volatile("mov a2, %0     \n"            \
                 "mov a3, %1     \n"            \
                 "j ts_arch_resume_ASM"
                ::"a" (old_tcb == NULL ? &_dummy_regs[0] : &old_tcb->regs), "a" (&g_core_current_thread[core_num]->regs)
                : "a2", "a3");
            }
            else
            {
                g_core_current_thread[core_num]->initialized = true;
                _xtos_ints_on(0x00200002);
                // Transfer control with a jump to prevent doing anything with window registers
                __asm volatile("mov a2, %0     \n"            \
                 "mov a3, %1     \n"            \
                 "j ts_arch_context_switch_ASM"
                ::"a" (old_tcb == NULL ? &_dummy_regs[0] : &old_tcb->regs), "a" (&g_core_current_thread[core_num]->regs)
                : "a2", "a3");
            }
        }
    }
    return;
}

extern HW_RESULT hw_int_set(HW_INT_VECTOR_MASK mask);

static void _yield()
{
    hw_int_set(1 << HW_INT_SW_5);
}

static void _ensure_priority(hw_tcb_t *cur, hw_tcb_t *next)
{
    if (cur != NULL && next != NULL)
    {
        if (cur->priority > next->priority)
        {
            _yield();
        }
    }
}

void hw_thread_init()
{
    if (!initialized)
    {
        g_thread_id_count_lock = 0;
        g_thread_id_count = 0;
        g_avl_lock = 0;
        avl_tree_init(&g_global_thread_tree, _thread_id_compare);
        for (int i = 0; i < HW_PROC_CNT; i++)
        {
            g_core_current_thread[i] = NULL;
            g_core_lock[i] = 0;
            for (int j = 0; j < STATE_NUM; j++)
            {
                for (int k = 0; k < PRIORITY_LEVEL_NUM; k++)
                {
                    linked_list_init(&g_core_thread_list[i][j][k]);
                }
            }
        }
        initialized = true;
        hw_thread_create(_null_proc,NULL, LOW, 0x2000, &_null_proc_id);
        hw_thread_start(_null_proc_id);
    }
}

int hw_thread_create(void (*proc)(void *),
                     void *args,
                     hw_thread_priority_t priority,
                     UINT32 stack_size,
                     UINT32 *thread_id)
{

    if (!initialized)
        return THREAD_STATUS_UNINITIALIZED;
    if (proc == NULL)
        return THREAD_STATUS_INVALID_ARGUMENT;
    // make one allocation to save time
    hw_tcb_t *tcb = (hw_tcb_t *) hw_alloc(sizeof(hw_tcb_t) + stack_size);
    if (tcb == NULL)
        return THREAD_STATUS_OUT_OF_MEMORY;
    HW_INT_VECTOR_MASK irql;

    tcb->proc = proc;
    tcb->stack_ptr = (char *) tcb + sizeof(hw_tcb_t) + stack_size;
    tcb->args = args;
    tcb->priority = priority;
    tcb->stack_size = stack_size;
    tcb->state = NEW;
    tcb->core_id = hw_getCoreNum();
    tcb->exit_code = 0;
    tcb->initialized = false;

    tcb->regs.pc = proc;
    tcb->regs.sp = tcb->stack_ptr;
    tcb->regs.ps = PS_WOE_MASK | PS_UM_MASK | PS_EXCM_MASK | (1 << PS_CALLINC_SHIFT);

    irql = hw_lock_irq_save(&g_thread_id_count_lock);
    tcb->thread_id = g_thread_id_count;
    g_thread_id_count++;
    hw_unlock_irq_restore(&g_thread_id_count_lock, irql);

    if (thread_id != NULL)
    {
        *thread_id = tcb->thread_id;
    }

    // write per core table
    irql = hw_lock_irq_save(&g_core_lock[tcb->core_id]);
    linked_list_push_back(&g_core_thread_list[tcb->core_id][tcb->state][tcb->priority],
                          &tcb->list_node);
    hw_unlock_irq_restore(&g_core_lock[tcb->core_id], irql);

    // write avl tree
    hw_lock(&g_avl_lock, false);
    avl_tree_insert(&g_global_thread_tree, &tcb->tree_node);
    hw_unlock(&g_avl_lock, false);

    return THREAD_STATUS_SUCCESS;
}

int hw_thread_start(UINT32 thread_id)
{
    if (!initialized)
        return THREAD_STATUS_UNINITIALIZED;
    int result = THREAD_STATUS_SUCCESS;
    hw_tcb_t *target = NULL;

    hw_lock(&g_avl_lock, false);
    target = _get_tcb_by_id(thread_id);
    if (target == NULL)
    {
        result = THREAD_STATUS_INVALID_ARGUMENT;
    }
    else
    {
        if (target->state == NEW)
        {
            target->state = READY;
        }
        else
        {
            result = THREAD_STATUS_ACTION_NOT_APPLICABLE;
        }
    }
    hw_unlock(&g_avl_lock, false);

    _ensure_priority(g_core_current_thread[hw_getCoreNum()], target);

    return result;
}

int hw_thread_destroy(UINT32 thread_id)
{
    if (!initialized)
        return THREAD_STATUS_UNINITIALIZED;
    // make one allocation to save time
    hw_tcb_t *tcb;
    int result = THREAD_STATUS_SUCCESS;
    HW_INT_VECTOR_MASK irql;

    // detach tcb
    hw_lock(&g_avl_lock, false);
    tcb = _get_tcb_by_id(thread_id);
    if (tcb == NULL)
    {
        result = THREAD_STATUS_INVALID_ARGUMENT;
    }
    else
    {
        if (tcb->state == EXIT)
        {
            avl_tree_delete(&g_global_thread_tree, &tcb->tree_node);
        }
        else
        {
            result = THREAD_STATUS_ACTION_NOT_APPLICABLE;
        }
    }
    hw_unlock(&g_avl_lock, false);


    if (result == THREAD_STATUS_SUCCESS)
    {
        irql = hw_lock_irq_save(&g_core_lock[tcb->core_id]);
        linked_list_remove(&g_core_thread_list[tcb->core_id][tcb->state][tcb->priority],
                           linked_list_search(&g_core_thread_list[tcb->core_id][tcb->state][tcb->priority],
                                              &tcb->list_node, _thread_id_equal));
        hw_unlock_irq_restore(&g_core_lock[tcb->core_id], irql);

        hw_free(tcb);
    }

    return result;
}

int hw_thread_get_exit_code(UINT32 thread_id, INT32 *exit_code)
{
    if (!initialized)
        return THREAD_STATUS_UNINITIALIZED;
    if (exit_code == NULL)
        return THREAD_STATUS_INVALID_ARGUMENT;

    int result = THREAD_STATUS_SUCCESS;
    hw_tcb_t *target = NULL;

    hw_lock(&g_avl_lock, false);
    target = _get_tcb_by_id(thread_id);
    if (target == NULL)
    {
        result = THREAD_STATUS_INVALID_ARGUMENT;
    }
    else
    {
        if (target->state == EXIT)
        {
            *exit_code = target->exit_code;
        }
        else
        {
            result = THREAD_STATUS_ACTION_NOT_APPLICABLE;
        }
    }
    hw_unlock(&g_avl_lock, false);

    return result;
}

void hw_thread_exit(INT32 exit_code)
{
    if (initialized)
    {
        HW_INT_VECTOR_MASK irql;
        int core_num = hw_getCoreNum();
        irql = hw_lock_irq_save(&g_core_lock[core_num]);
        g_core_current_thread[core_num]->state = EXIT;
        g_core_current_thread[core_num]->exit_code = exit_code;
        hw_unlock_irq_restore(&g_core_lock[core_num], irql);
        _yield();
    }
}

int hw_thread_block(UINT32 thread_id)
{
    if (!initialized)
        return THREAD_STATUS_UNINITIALIZED;
    int result = THREAD_STATUS_SUCCESS;
    hw_tcb_t *target = NULL;

    hw_lock(&g_avl_lock, false);
    target = _get_tcb_by_id(thread_id);
    if (target == NULL)
    {
        return THREAD_STATUS_INVALID_ARGUMENT;
    }
    else
    {
        if (target->state == RUN || target->state == READY)
        {
            target->state = BLOCK;
        }
        else
        {
            result = THREAD_STATUS_ACTION_NOT_APPLICABLE;
        }
    }
    hw_unlock(&g_avl_lock, false);
    hw_tcb_t *cur_tcb = g_core_current_thread[hw_getCoreNum()];
    if (cur_tcb != NULL && thread_id == cur_tcb->thread_id)
    {
        _yield();
    }
    return result;
}


int hw_thread_resume(UINT32 thread_id)
{
    if (!initialized)
        return THREAD_STATUS_UNINITIALIZED;
    int result = THREAD_STATUS_SUCCESS;
    hw_tcb_t *target = NULL;

    hw_lock(&g_avl_lock, false);
    target = _get_tcb_by_id(thread_id);
    if (target == NULL)
    {
        result = THREAD_STATUS_INVALID_ARGUMENT;
    }
    else
    {
        if (target->state == BLOCK)
        {
            target->state = READY;
        }
        else
        {
            result = THREAD_STATUS_ACTION_NOT_APPLICABLE;
        }
    }
    hw_unlock(&g_avl_lock, false);

    _ensure_priority(g_core_current_thread[hw_getCoreNum()], target);

    return result;
}
