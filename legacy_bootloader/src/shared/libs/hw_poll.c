/*-----------------------------------------------------
 |
 |      hw_poll.c
 |
 |    Contains functions for registering and
 |    clearing poll handlers.
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2012  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

#include "bifrost_private.h"
#include <bifrost_lock.h>

MEM_SRAM_UC hw_lock_t hw_pollLock[HW_PROC_CNT] = PROC_CNT_INITIALIZER(HW_LOCK_INITIALIZER);

HW_RESULT hw_register_poll_handler(HW_POLL_CB_FUNC* poll_cb, HW_POLL_HANDLER_CB_FUNC* handler_cb, VOID* cb_buffer, UINT32 core)
{
    if(core == HW_CORE_DEFAULT)
    {
        core = hw_getCoreNum();
    }
    
    HW_TESTSLAVE_VARS* tsVars = &testSlaveVarTable[core];
    
    HW_POLL_CONTEXT* handlerList = tsVars->polls.pollHandlers;
    
    HW_RESULT rc = HW_S_OK;
    BOOL found = FALSE;
    BOOL duplicate = FALSE;
    UINT32 position;
    
    // Invalidate the cache beforehand so we don't use any stale data
    hw_cacheInvalidateBuffer(handlerList, sizeof(HW_POLL_CONTEXT) * HW_TS_MAX_POLL_HANDLERS);
    
    if(hw_read32(&hw_pTestConfigs->bfinit.KERNEL_LOCKS_EN))
    {
        hw_lock(&hw_pollLock[core]);
    }
    else
    {
        if(core != hw_getCoreNum())
        {
            hw_errmsg("%s: trying to modify poll handlers on another core when kernel locks are disabled!\n", __func__);
        }
    }
    
    //
    // Find a spot in the handler list to place the polling handler
    //
    for(position = 0; position < HW_TS_MAX_POLL_HANDLERS; position++)
    {
        found = (hw_readptr(&handlerList[position].pPollCallback) == (UINTPTR)NULL);
        duplicate = (!found &&
                     hw_readptr(&handlerList[position].pPollCallback) == (UINTPTR)poll_cb &&
                     hw_readptr(&handlerList[position].callbackBuffer) == (UINTPTR)cb_buffer);
        
        if(found || duplicate)
        {
            break;
        }
    }
    
    if(duplicate)
    {
        hw_errmsg("%s: duplicate detected! Could not register a poll handler for poll_cb 0x%llx cb 0x%llx cb_buffer 0x%llx\n", __func__, (PTR_FIELD)(UINTPTR)poll_cb, (PTR_FIELD)(UINTPTR)handler_cb, (PTR_FIELD)(UINTPTR)cb_buffer);
        rc = HW_E_OTHER;
    }
    else if(!found)
    {
        hw_errmsg("%s: all poll handler slots are used up, could not register a poll handler for poll_cb 0x%llx cb 0x%llx cb_buffer 0x%llx\n", __func__, (PTR_FIELD)(UINTPTR)poll_cb, (PTR_FIELD)(UINTPTR)handler_cb, (PTR_FIELD)(UINTPTR)cb_buffer);
        rc = HW_E_OVERFLOW;
    }
    else
    {
        //
        // Assign it to that placement
        //
        hw_write32(&handlerList[position].testIdx, 0);
        hw_writeptr(&handlerList[position].pHandlerCallback, (UINTPTR)handler_cb);
        hw_writeptr(&handlerList[position].callbackBuffer, (UINTPTR)cb_buffer);
        hw_cacheFlushBuffer(&handlerList[position], sizeof(HW_POLL_CONTEXT));
        
        // Write pPollCallback last of all because that marks
        // this slot as valid
        hw_writeptr(&handlerList[position].pPollCallback, (UINTPTR)poll_cb);
        hw_cacheFlushAddr(&handlerList[position].pPollCallback);
    }
    
    if(hw_read32(&hw_pTestConfigs->bfinit.KERNEL_LOCKS_EN))
    {
        hw_unlock(&hw_pollLock[core]);
    }
    
    hw_debug("%s: registered slot %d to test %d, poll_cb 0x%llx cb 0x%llx cb_buffer 0x%llx\n", __func__, position, 0, (PTR_FIELD)(UINTPTR)poll_cb, (PTR_FIELD)(UINTPTR)handler_cb, (PTR_FIELD)(UINTPTR)cb_buffer);
    
    return rc;
}

HW_RESULT hw_deregister_poll_handler(HW_POLL_CB_FUNC* poll_cb, VOID* cb_buffer, UINT32 core)
{
    if(core == HW_CORE_DEFAULT)
    {
        core = hw_getCoreNum();
    }
    
    HW_TESTSLAVE_VARS* tsVars = &testSlaveVarTable[core];
    
    HW_RESULT rc = HW_S_OK;
    BOOL found = FALSE;
    UINT32 position = 0;
    HW_POLL_CONTEXT * handlerList = (HW_POLL_CONTEXT*)tsVars->polls.pollHandlers;
    
    // Invalidate the cache beforehand so we don't use any stale data
    hw_cacheInvalidateBuffer(handlerList, sizeof(HW_POLL_CONTEXT) * HW_TS_MAX_POLL_HANDLERS);
    
    if(hw_read32(&hw_pTestConfigs->bfinit.KERNEL_LOCKS_EN))
    {
        hw_lock(&hw_pollLock[core]);
    }
    else
    {
        if(core != hw_getCoreNum())
        {
            hw_errmsg("%s: trying to modify poll handlers on another core when kernel locks are disabled!\n", __func__);
        }
    }
    
    for(position = 0; position < HW_TS_MAX_POLL_HANDLERS; position++)
    {
        found = (hw_readptr(&handlerList[position].pPollCallback) == (UINTPTR)poll_cb &&
                 hw_readptr(&handlerList[position].callbackBuffer) == (UINTPTR)cb_buffer);
        
        if(found)
        {
            break;
        }
    }
    
    if(!found)
    {
        hw_errmsg("%s: No poll handler was found for poll_cb 0x%llx cb_buffer 0x%llx\n", __func__, (PTR_FIELD)(UINTPTR)poll_cb, (PTR_FIELD)(UINTPTR)cb_buffer);
        rc = HW_E_NOT_FOUND;
    }
    else
    {
        hw_writeptr(&handlerList[position].pPollCallback, (UINTPTR)NULL);
        hw_cacheFlushAddr(&handlerList[position].pPollCallback);
    }
    
    if(hw_read32(&hw_pTestConfigs->bfinit.KERNEL_LOCKS_EN))
    {
        hw_unlock(&hw_pollLock[core]);
    }
    
    return rc;
}

