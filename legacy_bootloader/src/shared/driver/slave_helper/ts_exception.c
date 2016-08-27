/*-----------------------------------------------------
 |
 |      ts_exception.c
 |
 |  Contains secondary functions for the Bifrost test
 |  slave, which initialize exceptions and handle
 |  exception flag checking.
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2013  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */
 
#include "bifrost_private.h"

BOOL ts_exc_handlerEntry(HW_EXC_VECTOR vector)
{
    BOOL handled = FALSE;
    
    HW_EXC_HANDLER_FUNC* handler = NULL;
    UINT32 dwIntOwner= 0;
    UINT32 dwPreviousTest = 0;
    BOOL preemptive = TRUE;
    
    if(hw_read32(&testSlaveVars->exceptions.isInitialized) == HW_EXC_INITIALIZED &&
       ts_exc_getHandler(vector, &handler, &dwIntOwner, &preemptive) != HW_E_NOT_FOUND)
    {
        handled = TRUE;
        
        //
        // If it's not preemptive exception handler, then it should just
        // mark itself in the pending exception mask and then return
        //
        if(preemptive == 0)
        {
            ts_exc_vectorMaskSet(vector, &testSlaveVars->exceptions.pendingExceptionMask);
        }
        else
        //
        // If this is an preemptive exception handler
        //
        {
            //
            // Store what test index is currently running and
            // then mark which test index is running a handler
            //
            //ts_set_test_index(dwIntOwner);

            (*handler)();
                
            //ts_set_test_index(dwPreviousTest);
        }
    }
    else
    //
    // Internally handled exceptions
    //
    {
        hw_log(HW_EVENT_UNHANDLED_EXCEPTION, (UINT64)vector);
    }
    
    return handled;
}

HW_RESULT ts_exc_init()
{
    // Initialize exception variables that would be placed in the private page
    UINT32 i = 0;
    for (i = 0; i < HW_TS_NUM_EXCVECTORS; i++)
    {
        hw_writeptr(&testSlaveVars->exceptions.contexts[i].pHandler,(UINTPTR) NULL);
        hw_write32(&testSlaveVars->exceptions.contexts[i].preemptive, 0);
        hw_write32(&testSlaveVars->exceptions.contexts[i].dwXTestOwner, 0);
    }

    hw_write32(&testSlaveVars->exceptions.isInitialized, HW_EXC_INITIALIZED);

    return HW_S_OK;
}

HW_RESULT ts_exc_getHandler(HW_EXC_VECTOR exception, HW_EXC_HANDLER_FUNC ** handler, UINT32 * dwOwner, BOOL * preemptive)
{
    UINTPTR pHandler = hw_readptr(&testSlaveVars->exceptions.contexts[exception].pHandler);
    
    if(pHandler == (UINTPTR) NULL)
    {
        return HW_E_NOT_FOUND;
    }
    
    *dwOwner = hw_read32(&testSlaveVars->exceptions.contexts[exception].dwXTestOwner);
    *handler = (HW_EXC_HANDLER_FUNC*)pHandler;
    *preemptive = hw_read32(&testSlaveVars->exceptions.contexts[exception].preemptive);

    return HW_S_OK;
}

void ts_exc_vectorMaskSet(HW_EXC_VECTOR exception, HW_EXC_VECTOR_MASK * mask)
{
    UINT64 temp;
    temp = hw_read64(mask);
    temp |= 0x1 << exception;
    hw_write64(mask, temp);
}

void ts_exc_vectorMaskClear(HW_EXC_VECTOR exception, HW_EXC_VECTOR_MASK * mask)
{
    UINT64 temp;
    temp = hw_read64(mask);
    temp &= ~(0x1 << exception);
    hw_write64(mask, temp);
}

HW_EXC_VECTOR_MASK ts_exc_queryNonpreemptiveExceptions()
{
    return hw_read64(&testSlaveVars->exceptions.pendingExceptionMask);
}

void ts_exc_handleNonpreemptiveExceptions()
{
    HW_EXC_VECTOR_MASK exceptionVector = {0};

    UINT32 exception = 0;
    UINT64 exceptionMask = 0;

    exceptionVector = ts_exc_queryNonpreemptiveExceptions();
    while (exceptionVector)
    {
        exceptionMask = exceptionVector & (0x1 << exception);
        if(exceptionMask)
        {
            exceptionVector &= ~exceptionMask;
                
            HW_EXC_HANDLER_FUNC * handler;
            UINT32 dwExcOwner;
            BOOL preemptive = false;
            if(ts_exc_getHandler((HW_EXC_VECTOR)exception, &handler, &dwExcOwner, &preemptive) != HW_E_NOT_FOUND && preemptive)
            {
                ts_set_test_index(dwExcOwner);

                (*handler)();
                        
                ts_set_test_index(HW_NO_TEST_INDEX);
            }
            else
            {
                hw_errmsg("ERROR: No handler registered for received exception %d\n", exception);
            }
        }
        exception++;
    }
}

