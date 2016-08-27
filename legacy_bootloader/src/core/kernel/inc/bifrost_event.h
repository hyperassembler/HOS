#ifndef _BIFROST_EVENT_H
#define _BIFROST_EVENT_H

#include "linked_list.h"
#include "bifrost_ref.h"

typedef enum
{
    EVENT_TYPE_MANUAL,
    EVENT_TYPE_AUTO
} hw_event_type_t;

typedef struct
{
    hw_ref_node_t ref_node;
    bool signaled;
    linked_list_t waiting_threads;
    hw_event_type_t type;
    hw_spin_lock_t lock;
} hw_event_t;

typedef struct
{
    linked_list_node_t list_node;
    void* tcb;
    hw_callback_func_t free_func;
} hw_event_node_t;

hw_result_t hw_event_wait(hw_handle_t event);

hw_result_t hw_event_reset(hw_handle_t event);

hw_result_t hw_event_signal(hw_handle_t handle);

hw_result_t hw_event_create(hw_handle_t *out, hw_event_type_t event_type);

hw_result_t ke_event_wait(hw_event_t *event, hw_event_node_t *node);

hw_result_t ke_event_reset(hw_event_t *event);

hw_result_t ke_event_signal(hw_event_t *event);

hw_result_t ke_event_init(hw_event_t *event, hw_event_type_t event_type);

#endif // _BIFROST_EVENT_H
