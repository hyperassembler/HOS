#ifndef KERNEL_RF_REF_H
#define KERNEL_RF_REF_H

#include "type.h"
#include "status.h"

typedef uint32 handle_t;

typedef struct
{
	int32 ref_count;
	callback_func free_routine;
} ref_node_t;

#define K_HANDLE_BASE (0x80000000ul)

//
// All functions are sx since users or kernel devs should not be
// specifying where the allocations take place
//

sx_status SXAPI rf_reference_setup(void);

sx_status SXAPI rf_reference_create(ref_node_t *ref,
                                  callback_func free_func);

sx_status SXAPI rf_reference_obj(ref_node_t *ref);

sx_status SXAPI rf_dereference_obj(ref_node_t *ref);

// HANDLES
sx_status SXAPI sx_open_obj_by_handle(handle_t handle, ref_node_t **out);

sx_status SXAPI sx_create_handle(ref_node_t *ref, handle_t *out);

sx_status SXAPI sx_close_handle(handle_t handle);

#endif
