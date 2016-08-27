/*-----------------------------------------------------
 |
 |      ts_interrupts.c
 |
 |    Contains functions for the testslave
 |    to check registered poll handlers.
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

// Cycles through registered poll handlers and returns.
void ts_poll()
{
    HW_POLL_CONTEXT* handlerList = testSlaveVars->polls.pollHandlers;
    HW_POLL_CB_FUNC* handler = NULL;
    UINT32 testIdx;
    VOID* cb_buffer;
    
    // Invalidate the cache beforehand so we don't use any stale data
    hw_cacheInvalidateBuffer(handlerList, sizeof(HW_POLL_CONTEXT) * HW_TS_MAX_POLL_HANDLERS);
    
    //
    // Test every valid poll address. If any of them
    // are true, run the associated handler.
    //
    for(UINT32 position = 0; position < HW_TS_MAX_POLL_HANDLERS; position++)
    {
        handler = (HW_POLL_CB_FUNC*)hw_readptr(&handlerList[position].pPollCallback);
        if(handler)
        {
            cb_buffer = (VOID*)hw_readptr(&handlerList[position].callbackBuffer);
            testIdx = hw_read32(&handlerList[position].testIdx);
            
            ts_set_test_index(testIdx); 
            
            if((*handler)(cb_buffer))
            {
                ((HW_POLL_HANDLER_CB_FUNC*)hw_readptr(&handlerList[position].pHandlerCallback))(cb_buffer);
            }
            
            ts_set_test_index(HW_NO_TEST_INDEX);
        }
    }
}

