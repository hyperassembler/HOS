#ifndef _BIFROST_REF_H
#define _BIFROST_REF_H

#include "bifrost_types.h"
#include "bifrost_statuscode.h"
#include "avl_tree.h"
#include "bifrost_lock.h"

typedef struct
{
    int32_t ref_count;
    hw_callback_func_t callback;
} hw_ref_node_t;


#define HW_HANDLE_BASE 0x80000000
#define HW_HANDLE_CURRENT_THREAD 0x1

//
// All functions are hw since users or kernel devs should not be
// specifying where the allocations take place
//

hw_result_t ke_reference_setup();

hw_result_t ke_reference_create(hw_ref_node_t *ref,
                                hw_callback_func_t free_func);

hw_result_t ke_reference_obj(hw_ref_node_t *ref);

hw_result_t ke_dereference_obj(hw_ref_node_t *ref);

// HANDLES
hw_result_t hw_open_obj_by_handle(hw_handle_t handle, hw_ref_node_t **out);

hw_result_t hw_create_handle(hw_ref_node_t *ref, hw_handle_t *out);

hw_result_t hw_close_handle(hw_handle_t handle);

#endif
