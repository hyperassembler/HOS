#ifndef _BIFROST_APC_H
#define _BIFROST_APC_H

#include "bifrost_statuscode.h"
#include "bifrost_thread.h"

hw_result_t hw_apc_queue(hw_handle_t thread_handle, hw_callback_func_t callback, void* args);

hw_result_t ke_apc_setup(uint32_t vec);

hw_result_t ke_apc_drain(uint32_t core);

#endif
