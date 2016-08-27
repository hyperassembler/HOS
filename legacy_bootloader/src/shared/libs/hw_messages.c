/*-------------------------------------------------------
 |
 |     hw_sendmessage.c
 |
 |     
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2011  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#include "bifrost_private.h"

void hw_sendMessage(HW_MESSAGE* message, HW_TESTID destinationID)
{
    HW_MAILBOX* outbox = &testSlaveVars->driverMailbox;
    
    UINT32 currentTest = hw_read32(&testSlaveVars->dwCurrentXTest);
    HW_TESTID myID = hw_getMyInstanceID();
    
    message->metadata.to = destinationID;
    message->metadata.from = myID;
    message->metadata.command = MSG_TS_CMD_MESSAGE;
    message->metadata.size = MSG_DATASIZE;
    placeMessage(outbox, message);
}

void hw_broadcastMessage(HW_MESSAGE* message)
{
    HW_MAILBOX* outbox = &testSlaveVars->driverMailbox;
    
    UINT32 currentTest = hw_read32(&testSlaveVars->dwCurrentXTest);
    HW_TESTID myID = hw_getMyInstanceID();
    HW_TS_PUBLIC_TESTDATA* publicTestData;
    
    message->metadata.from = myID;
    message->metadata.command = MSG_TS_CMD_MESSAGE;
    message->metadata.size = MSG_DATASIZE;
    
    // Loop through all the cores
    for(UINT32 i = 0; i < HW_PROC_CNT; i++)
    {
        publicTestData = (HW_TS_PUBLIC_TESTDATA*)hw_readptr(&testSlaveVarTable[i].publicTestData);
        // For each test on a core, place the message
        for(UINT32 j = 0; j < testSlaveVarTable[i].dwTestCnt; j++)
        {
            message->metadata.to = hw_read32(&publicTestData[j].testInstanceID);
            placeMessage(outbox, message);
        }
    }
}

BOOL hw_receiveMessage(HW_MESSAGE* ret)
{
    UINT32 testIdx;
    testIdx = hw_read32(&testSlaveVars->dwCurrentXTest);
    HW_TS_PUBLIC_TESTDATA* publicTestData = (HW_TS_PUBLIC_TESTDATA*)hw_readptr(&testSlaveVars->publicTestData);
    
    HW_MAILBOX* box = &publicTestData[testIdx].mailbox;
    
    return getMessage(box, ret);
}

