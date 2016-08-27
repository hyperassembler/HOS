#ifndef _BIFROST_TIMER_H
#define _BIFROST_TIMER_H

#include "linked_list.h"
#include "bifrost_types.h"
#include "bifrost_statuscode.h"
#include "bifrost_lock.h"
#include "bifrost_ref.h"

typedef enum
{
    TIMER_TYPE_MANUAL_RESET,
    TIMER_TYPE_AUTO_RESET
} hw_timer_type_t;

typedef struct
{
    hw_ref_node_t ref_node;
    linked_list_node_t list_node;
    linked_list_t waiting_threads;
    uint32_t tick;
    uint32_t elapsed_tick;
    hw_spin_lock_t lock;
    hw_timer_type_t timer_type;
    bool signaled;
    bool periodic;
    bool active;
} hw_timer_t;

typedef struct
{
    linked_list_node_t list_node;
    void *tcb;
    hw_callback_func_t free_func;
} hw_timer_node_t;

// NOTE THAT timers are not shared between cores

hw_result_t hw_timer_create(hw_handle_t *out,
                            hw_timer_type_t type);

hw_result_t hw_timer_wait(hw_handle_t timer_handle);

hw_result_t hw_timer_set(hw_handle_t timer_handle, uint32_t tick, bool periodic);

hw_result_t hw_timer_cancel(hw_handle_t timer_handle);

// KEs

void ke_timer_tick(void *kp, void *up);

hw_result_t ke_timer_setup();

hw_result_t ke_timer_init(hw_timer_t *timer,
                          hw_timer_type_t type);

hw_result_t ke_timer_wait(hw_timer_t *timer, hw_timer_node_t *node);

hw_result_t ke_timer_set(hw_timer_t *timer, uint32_t tick, bool periodic);

hw_result_t ke_timer_cancel(hw_timer_t *timer);

bool ke_query_and_clear_timer_dpc(uint32_t core);

hw_result_t ke_queue_timer_dpc(uint32_t core);

#endif
