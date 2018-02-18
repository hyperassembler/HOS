#ifndef _KERNEL_RF_REF_H_
#define _KERNEL_RF_REF_H_

#include "type.h"
#include "status.h"

typedef uint32_t handle_t;

typedef struct
{
	int32_t ref_count;
	callback_func_t free_routine;
} ref_node_t;

#define K_HANDLE_BASE 0x80000000

//
// All functions are sx since users or kernel devs should not be
// specifying where the allocations take place
//

status_t KABI rf_reference_setup(void);

status_t KABI rf_reference_create(ref_node_t *ref,
                                  callback_func_t free_func);

status_t KABI rf_reference_obj(ref_node_t *ref);

status_t KABI rf_dereference_obj(ref_node_t *ref);

// HANDLES
status_t KABI sx_open_obj_by_handle(handle_t handle, ref_node_t **out);

status_t KABI sx_create_handle(ref_node_t *ref, handle_t *out);

status_t KABI sx_close_handle(handle_t handle);

#endif
