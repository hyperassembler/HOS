/*-----------------------------------------------------
 |
 |      ts_interrupt.c
 |
 |  Contains secondary functions for the Bifrost test
 |  slave, which initialize interrupts and handle
 |  interrupt flag checking.
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

BOOL ts_int_handlerEntry(HW_INT_VECTOR vector)
{
    BOOL handled = FALSE;
    
    HW_INT_HANDLER_FUNC* handler = NULL;
    VOID* buffer = NULL;
    UINT32 dwIntOwner = 0;
    UINT32 dwPreviousTest = 0;
    BOOL preemptive = TRUE;
    
    // Disable any further interrupts from preempting
    // us while we process the existing ones
    HW_INT_VECTOR_MASK oldMask = hw_int_disable(HW_INT_PROTECTION_MASK);
    
    if(hw_read32(&testSlaveVars->interrupts.isInitialized) == HW_INT_INITIALIZED)
    {
        // Test handled interrupt
        if(ts_int_getHandler(vector, &handler, &buffer, &dwIntOwner, &preemptive) != HW_E_NOT_FOUND)
        {
            handled = TRUE;
            
            //
            // If it's not preemptive interrupt handler, then it should just
            // mark itself in the pending interrupt mask and then return
            //
            if(preemptive == 0)
            {
                ts_int_vectorMaskSet(vector, &testSlaveVars->interrupts.pendingInterruptMask);
                hw_int_clear(1<<vector);
            }
            else
            //
            // If this is an preemptive interrupt handler
            //
            {
                //
                // Store what test index is currently running and
                // then mark which test index is running a handler
                //
                //ts_set_test_index(dwIntOwner);
                
                (*handler)(buffer);
                
                //ts_set_test_index(dwPreviousTest);
            }

        }
        else
        //
        // Internally handled interrupts
        //
        {
            hw_int_clear(1<<vector);
            hw_log(HW_EVENT_UNHANDLED_INTERRUPT, (UINT64)vector);
        }
    }
    
    // Reenable previous interrupt mask
    hw_int_enable(oldMask);
    
    return handled;
}

HW_RESULT ts_int_init()
{
    arch_int_init();
    
    UINT32 i = 0;
    for (i = 0; i < HW_TS_NUM_INTVECTORS; i++)
    {
        hw_writeptr(&testSlaveVars->interrupts.contexts[i].pHandler,(UINTPTR) NULL);
        hw_write32(&testSlaveVars->interrupts.contexts[i].preemptive, 0);
        hw_write32(&testSlaveVars->interrupts.contexts[i].dwXTestOwner, 0);
    }
    
    hw_cacheFlushBuffer(testSlaveVars->interrupts.contexts, HW_INT_CONTEXT_SIZE * HW_TS_NUM_INTVECTORS);
    
    hw_write32(&testSlaveVars->interrupts.isInitialized, HW_INT_INITIALIZED);
    
    return HW_S_OK;

}

BOOL ts_int_querySource(HW_INT_VECTOR* interrupt)
{
    UINT32 ints = hw_int_queryInterrupt();

    UINT32 intMask = 0;
    UINT32 intEnabled = 0;
    int i = 0;
    for (i = HW_TS_NUM_INTVECTORS-1; i >= 0; i--)
    {
        intMask = 1 << i;
        intEnabled = ints & intMask;
        if(intEnabled)
        {
            *interrupt = (HW_INT_VECTOR)i;
            return TRUE;
        }
    }
    return FALSE;
}

HW_RESULT ts_int_getHandler(HW_INT_VECTOR interrupt, HW_INT_HANDLER_FUNC** handler, VOID** buffer, UINT32* dwOwner, BOOL* preemptive)
{
    hw_cacheInvalidateBuffer(&testSlaveVars->interrupts.contexts[interrupt], HW_INT_CONTEXT_SIZE);
    UINTPTR pHandler = hw_readptr(&testSlaveVars->interrupts.contexts[interrupt].pHandler);
    
    if(pHandler == (UINTPTR) NULL)
    {
        return HW_E_NOT_FOUND;
    }
    
    *dwOwner = hw_read32(&testSlaveVars->interrupts.contexts[interrupt].dwXTestOwner);
    *handler = (HW_INT_HANDLER_FUNC*)pHandler;
    *buffer = (VOID*)hw_readptr(&testSlaveVars->interrupts.contexts[interrupt].buffer);
    *preemptive = hw_read32(&testSlaveVars->interrupts.contexts[interrupt].preemptive);
    
    return HW_S_OK;
}

void ts_int_vectorMaskSet(HW_INT_VECTOR interrupt, HW_INT_VECTOR_MASK * mask)
{
    UINT32 temp;
    temp = hw_read32(mask);
    temp |= 0x1 << interrupt;
    hw_write32(mask, temp);
}

void ts_int_vectorMaskClear(HW_INT_VECTOR interrupt, HW_INT_VECTOR_MASK * mask)
{
    UINT32 temp;
    temp = hw_read32(mask);
    temp &= ~(0x1 << interrupt);
    hw_write32(mask, temp);
}

HW_INT_VECTOR_MASK ts_int_queryNonpreemptiveInterrupts()
{
    return hw_read32(&testSlaveVars->interrupts.pendingInterruptMask);
}

void ts_int_handleNonpreemptiveInterrupts()
{
    HW_INT_VECTOR_MASK interruptVector = {0};
    
    UINT32 interrupt = 0;
    UINT32 interruptMask = 0;
    
    interruptVector = ts_int_queryNonpreemptiveInterrupts();
    while (interruptVector)
    {
        interruptMask = interruptVector & (0x1 << interrupt);
        if(interruptMask)
        {
            interruptVector &= ~interruptMask;
            
            HW_INT_HANDLER_FUNC* handler;
            VOID* buffer;
            UINT32 dwIntOwner;
            BOOL preemptive = false;
            if(ts_int_getHandler((HW_INT_VECTOR)interrupt, &handler, &buffer, &dwIntOwner, &preemptive) != HW_E_NOT_FOUND && preemptive)
            {
                ts_set_test_index(dwIntOwner);
                
                (*handler)(buffer);
                
                ts_set_test_index(HW_NO_TEST_INDEX);
            }
            else
            {
                hw_errmsg("ERROR: No handler registered for received interrupt %d\n", interrupt);
                hw_int_clear(1<<interrupt);
            }
        }
        interrupt++;
    }
}

