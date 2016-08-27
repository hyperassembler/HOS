/*-----------------------------------------------------
 |
 |      hw_mailbox.c
 |
 |  Implementation of mailbox message sending functions.
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2011  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

#include "bifrost_private.h"
#include <bifrost_intr.h>

HW_RESULT placeMessage(HW_MAILBOX* box, HW_MESSAGE* send)
{
    //
    // Check for full mailbox.
    //
    UINT32 head, tail;
    UINT32* headPtr;
    UINT32* tailPtr;
    
    headPtr = (UINT32*)hw_readptr(&box->headPtr);
    tailPtr = (UINT32*)hw_readptr(&box->tailPtr);
    
    hw_irql_t prev_irql = hw_raise_irql(HW_IRQL_DISABLED_LEVEL);
    
    hw_cacheFlushAddr(headPtr);
    hw_cacheFlushAddr(tailPtr);
    head = hw_read32(headPtr);
    tail = hw_read32(tailPtr);
    
    if((head == 0 && tail == HW_MB_MAX_SIZE - 1) || tail == head - 1)
    {
        hw_log(HW_EVENT_CHECKPOINT, (UINTPTR)"mailbox_push_wait", (UINTPTR)"enter", 0, 0);
        do {
            hw_cacheFlushAddr(headPtr);
            hw_cacheFlushAddr(tailPtr);
            head = hw_read32(headPtr);
            tail = hw_read32(tailPtr);
        } while ((head == 0 && tail == HW_MB_MAX_SIZE - 1) || tail == head - 1);
        hw_log(HW_EVENT_CHECKPOINT, (UINTPTR)"mailbox_push_wait", (UINTPTR)"exit", 0, 0);
    }
    
    //
    // Write and flush the message
    //
    SIZE_T size = send->metadata.size + sizeof(HW_MSG_METADATA);
    size = roundUpToMultipleUnsigned(size, sizeof(UINT32));
    for(UINT32 ctr = 0; ctr < size / sizeof(UINT32); ctr++)
    {
        hw_write32((UINT32*)&box->messages[tail] + ctr, *((UINT32*)send + ctr));
    }
    hw_cacheFlushBuffer(&box->messages[tail], sizeof(HW_MESSAGE));
    
    //
    // Update the mailbox state
    //
    tail = (tail + 1) % HW_MB_MAX_SIZE;
    hw_write32(tailPtr, tail);
    
    //
    // Flush out changes.
    //
    hw_cacheFlushAddr(tailPtr);
    
    hw_lower_irql(prev_irql);
    
    return HW_S_OK;
}

BOOL getMessage(HW_MAILBOX* box, HW_MESSAGE* get)
{
    //
    // Check for empty mailbox
    //
    
    UINT32 head, tail;
    UINT32* headPtr;
    UINT32* tailPtr;
    
    headPtr = (UINT32*)hw_readptr(&box->headPtr);
    tailPtr = (UINT32*)hw_readptr(&box->tailPtr);
    
    hw_irql_t prev_irql = hw_raise_irql(HW_IRQL_DISABLED_LEVEL);
    
    //
    // Flush and update mailbox variables
    //
    hw_cacheFlushAddr(headPtr);
    hw_cacheFlushAddr(tailPtr);
    
    head = hw_read32(headPtr);
    tail = hw_read32(tailPtr);
    
    if (head == tail)
    {
        hw_lower_irql(prev_irql);
        return FALSE;
    }
    
    //
    // Flush and read the message.
    // Need to flush size first, then
    // use that to flush the rest of the message.
    //
    hw_cacheFlushBuffer(&box->messages[head], sizeof(HW_MESSAGE));
    SIZE_T size = hw_read32(&box->messages[head].metadata.size) + sizeof(HW_MSG_METADATA);
    size = roundUpToMultipleUnsigned(size, sizeof(UINT32));
    for(UINT32 ctr = 0; ctr < size / sizeof(UINT32); ctr++)
    {
        *((UINT32*)get + ctr) = hw_read32((UINT32*)&box->messages[head] + ctr);
    }
    
    //
    // Update the mailbox state
    //
    head = (head + 1) % HW_MB_MAX_SIZE;
    hw_write32(headPtr, head);
    
    //
    // Flush out changes.
    //
    hw_cacheFlushAddr(headPtr);
    
    hw_lower_irql(prev_irql);
    
    return TRUE;
}

#if 0
UINT32 hw_getMailboxNumberOfMessages()
{
    HW_TS_PUBLIC_TESTDATA* publicTestData = (HW_TS_PUBLIC_TESTDATA*)hw_readptr(&testSlaveVars->publicTestData);
    UINT32 currentTestNum = hw_read32(&testSlaveVars->dwCurrentXTest);
    HW_MAILBOX* currentTestMailbox = &publicTestData[currentTestNum].mailbox;
    UINT32* headPtr;
    UINT32* tailPtr;
    
    headPtr = (UINT32*)hw_readptr(&currentTestMailbox->headPtr);
    tailPtr = (UINT32*)hw_readptr(&currentTestMailbox->tailPtr);
    
    //
    // Flush and update mailbox variables
    //
    hw_cacheFlushAddr(headPtr);
    hw_cacheFlushAddr(tailPtr);
    
    //
    // Check number of unprocessed messages
    //
    UINT32 head, tail;
    
    head = hw_read32(headPtr);
    tail = hw_read32(tailPtr);
    
    UINT32 count = 0;
    // If the tail has looped around the buffer
    if(tail < head)
    {
        count = (HW_MB_MAX_SIZE - head) + tail;
    }
    else
    {
        count = tail - head;
    }
    
    return count;
}
#endif

void hw_flushMailbox()
{
    // Flush mailbox doesn't mean anything if we're the driver.
    // Otherwise, send a message and wait for an ACK
    if(hw_getCoreNum() != bifrostCachedGlobals.tpid)
    {
        HW_MESSAGE message;
        message.metadata.command = MSG_TS_CMD_FLUSH_MAILBOX;
        message.metadata.size = 0;
        placeMessage(&testSlaveVars->driverLowPrioMailbox, &message);
        while(!getMessage(&testSlaveVars->slaveLowPrioMailbox, &message));
    }
}

