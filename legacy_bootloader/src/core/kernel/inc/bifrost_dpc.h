/*-------------------------------------------------------
 |
 |      bifrost_dpc.h
 |
 |      Contains Bifrost deferred procedure call APIs,
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2016  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#ifndef _BIFROST_DPC_H_
#define _BIFROST_DPC_H_

#include "bifrost_types.h"
#include "bifrost_statuscode.h"

hw_result_t hw_dpc_queue(uint32_t core, hw_callback_func_t proc, void* args);

hw_result_t ke_dpc_setup(uint32_t vec);

hw_result_t ke_dpc_drain(uint32_t core);

#endif
