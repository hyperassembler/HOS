#ifndef _K_REF_H_
#define _K_REF_H_

#include "g_type.h"
#include "k_status.h"

typedef struct
{
    int32_t ref_count;
    k_callback_func_t free_routine;
} k_ref_node_t;

#define K_HANDLE_BASE 0x80000000

//
// All functions are sx since users or kernel devs should not be
// specifying where the allocations take place
//

k_status_t KAPI ke_reference_setup();

k_status_t KAPI ke_reference_create(k_ref_node_t *ref,
                               k_callback_func_t free_func);

k_status_t KAPI ke_reference_obj(k_ref_node_t *ref);

k_status_t KAPI ke_dereference_obj(k_ref_node_t *ref);

// HANDLES
k_status_t KAPI sx_open_obj_by_handle(k_handle_t handle, k_ref_node_t **out);

k_status_t KAPI sx_create_handle(k_ref_node_t *ref, k_handle_t *out);

k_status_t KAPI sx_close_handle(k_handle_t handle);

#endif
