/*-------------------------------------------------------
 |
 |      bifrost_thread.h
 |
 |      Contains Bifrost threading APIs,
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2016  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#ifndef _BIFROST_THREAD_H_
#define _BIFROST_THREAD_H_

#include "linked_list.h"
#include "avl_tree.h"
#include "bifrost_statuscode.h"
#include "bifrost_lock.h"
#include "bifrost_event.h"
#include "bifrost_ref.h"

#define THREAD_DEFAULT_STACK_SIZE 0x4000
#define THREAD_INVALID_PID -1

#define THREAD_EXIT_CODE_TERMINATED 0xDEADDEAD

typedef enum
{
    STATE_NEW,
    STATE_BLOCK,
    STATE_READY,
    STATE_EXIT,
    STATE_RUN,
    STATE_NUM,
    STATE_OUTSIDE
} hw_thread_state_t;

typedef enum
{
    PRIORITY_HIGHEST = 0,
    PRIORITY_HIGH,
    PRIORITY_DEFAULT,
    PRIORITY_LOW,
    PRIORITY_LOWEST,
    PRIORITY_LEVEL_NUM
} hw_thread_priority_t;

typedef struct
{
    void *prev_context;
    void *next_context;
} hw_thread_schedule_info_t;

typedef struct
{
    linked_list_node_t list_node;
    avl_tree_node_t tree_node;
    int32_t thread_id;
    uint32_t core_id;
    void *stack_ptr;
    uint64_t regs[16];

    void (*proc)(void *);

    uint32_t stack_size;
    void *args;
    int32_t exit_code;
    _Bool initialized;

    // apc stuff
    hw_spin_lock_t apc_lock;
    linked_list_t apc_list;

    // state is guarded by the scheduler lock
    hw_thread_state_t state;
    linked_list_node_t scheduler_queue_node;
    bool in_scheduler_queue;
    hw_thread_priority_t priority;
    hw_thread_state_t location;

    // thread exit event obj
    hw_event_t thread_exit_event;

    // reference manager
    hw_ref_node_t ref_node;
} hw_tcb_t;

// ==================
// HW Functions
// ==================

hw_result_t hw_thread_create(void (*proc)(void *),
                             void *args,
                             hw_thread_priority_t priority,
                             uint32_t stack_size,
                             hw_handle_t *thread_handle);

hw_result_t hw_thread_start(hw_handle_t thread_handle);

hw_result_t hw_thread_block(hw_handle_t thread_handle);

hw_result_t hw_thread_resume(hw_handle_t thread_handle);

hw_result_t hw_thread_terminate(hw_handle_t thread_handle);

hw_result_t hw_thread_get_exit_code(hw_handle_t thread_handle, int32_t *exit_code);

hw_result_t hw_thread_open(int32_t thread_id, hw_handle_t *out);

hw_result_t hw_thread_assert_state(hw_handle_t thread_handle, hw_thread_state_t state);

// ==================
// Ke Functions
// ==================

hw_result_t ke_thread_open(int32_t id, hw_tcb_t **out);


hw_result_t ke_thread_create(hw_tcb_t *tcb,
                             void (*proc)(void *),
                             void *args,
                             hw_thread_priority_t priority,
                             uint32_t stack_size,
                             void *stack_ptr);

hw_result_t ke_thread_start(hw_tcb_t *tcb);

hw_result_t ke_thread_terminate(hw_tcb_t *tcb);

hw_result_t ke_thread_get_exit_code(hw_tcb_t *tcb, int32_t *exit_code);

void ke_thread_exit(int32_t exit_code);

hw_result_t ke_thread_block(hw_tcb_t *tcb);

hw_result_t ke_thread_resume(hw_tcb_t *tcb);

hw_result_t ke_thread_yield(uint32_t core);

bool ke_query_and_clear_scheduler_dpc(uint32_t core);

hw_result_t ke_queue_scheduler_dpc(uint32_t core);

// ==========================
// Thread Context Routines. No Ref/Deref needed
// N.B. current impl of hw_current_thread needs close_handle
// ==========================

int32_t hw_current_thread_id();

hw_handle_t hw_current_thread();

hw_result_t hw_wait_for_thread_exit(hw_handle_t handle);

hw_result_t hw_thread_sleep(uint32_t millis);

void hw_thread_exit(int32_t exit_code);


int32_t ke_current_thread_id();

hw_tcb_t *ke_current_thread();

void ke_thread_exit(int32_t exit_code);

// ==========================
// MISC Routines
// ==========================

hw_result_t hw_thread_setup();

void ke_thread_schedule(void *info, void *up);

#endif
