/*-------------------------------------------------------
 |
 |    hw_exception.c
 |
 |    Contains functions which enable tests to do exception handling
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2013  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#include "bifrost_private.h"

//
// Registers a function pointer that will be serviced if an exception occurs
//
HW_RESULT hw_exc_registerHandlerGeneric(HW_EXC_VECTOR exception, HW_EXC_HANDLER_FUNC * handler, BOOL preemptive)
{
    UINT32 currentTest = hw_read32(&testSlaveVars->dwCurrentXTest);
    UINT32 dwOwner = hw_read32(&testSlaveVars->exceptions.contexts[exception].dwXTestOwner);

    //
    // Check to make sure that no test on this
    // core already has a handler for this interrupt
    //
    if((UINTPTR) NULL != hw_readptr(&testSlaveVars->exceptions.contexts[exception].pHandler))
    {
        hw_errmsg("ERROR %s: test %d attempts to register exception handler for vector %d, already owned by test %d.\n", __func__, currentTest, exception, dwOwner);
        return HW_E_ACCESS_VIOLATION;
    }

    //
    // If unique, register the handler
    //
    hw_writeptr(&testSlaveVars->exceptions.contexts[exception].pHandler,(UINTPTR) handler);
    hw_write32(&testSlaveVars->exceptions.contexts[exception].preemptive, preemptive);
    hw_write32(&testSlaveVars->exceptions.contexts[exception].dwXTestOwner, currentTest);

    return HW_S_OK;

}

//
// Deregisters an interrupt handler
//
HW_RESULT hw_exc_deregisterHandler(HW_EXC_VECTOR exception)
{
    UINT32 currentTest = hw_read32(&testSlaveVars->dwCurrentXTest);
    UINT32 dwOwner = hw_read32(&testSlaveVars->exceptions.contexts[exception].dwXTestOwner);
    
    HW_RESULT rc = HW_S_OK;
    
    //
    // Check to make sure that a handler has actually
    // been registered for this exception
    //
    if((UINTPTR) NULL == hw_readptr(&testSlaveVars->exceptions.contexts[exception].pHandler))
    {
        hw_errmsg("%s: test %d attempts to clear exception handler for vector %d, for which no handler has been registered\n", __func__, exception);
        rc = HW_E_NOT_FOUND;
    }
    else if(dwOwner != currentTest && currentTest != HW_NO_TEST_INDEX)
    //
    // Unless this is an internal Bifrost operation, don't
    // let tests clear exception handlers they didn't
    // register
    //
    {
        hw_errmsg("ERROR %s: test %d attempts to clear exception handler for vector %d, owned by test %d.\n", __func__, currentTest, exception, dwOwner);
        return HW_E_ACCESS_VIOLATION;
    }
    else
    //
    // Clear the exception handler.
    //
    {
        hw_writeptr(&testSlaveVars->exceptions.contexts[exception].pHandler,(UINTPTR) NULL);
        hw_write32(&testSlaveVars->exceptions.contexts[exception].preemptive, 0);
        hw_write32(&testSlaveVars->exceptions.contexts[exception].dwXTestOwner, 0);
    }

    return rc;
}

//
// Deregister all exception handlers
//
VOID hw_exc_deregisterAllHandlers()
{
    UINT32 currentTest = hw_read32(&testSlaveVars->dwCurrentXTest);
    UINT32 dwOwner;
    
    // Loop through all exception vectors
    UINT64 i;
    for(i = 0; i < HW_TS_NUM_EXCVECTORS; i++)
    {
        // If a handler is registered for this vector and
        // the current test is the owner (or this is an internal
        // call), free the handler.
        dwOwner = hw_read32(&testSlaveVars->exceptions.contexts[i].dwXTestOwner);
        if((UINTPTR) NULL != hw_readptr(&testSlaveVars->exceptions.contexts[i].pHandler)
              && (dwOwner == currentTest || currentTest == HW_NO_TEST_INDEX) )
        {
            hw_exc_deregisterHandler((HW_EXC_VECTOR)i);
        }
    }
}
