/*-----------------------------------------------------
 |
 |      td_standby.c
 |
 |  Contains function for the Bifrost test driver
 |  to respond to tests for standby request.
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2014  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */
 
#include "bifrost_private.h"

extern volatile BOOL td_hasStandbyRequest;

void td_standby_reply(HW_TESTID testId, UINT32 dwpid)
{
    HW_MESSAGE message;
    message.metadata.command = MSG_TS_CMD_STANDBY;
    message.metadata.size = sizeof(UINT32);
    *(UINT32*) message.data = HW_S_OK;
    hw_printf(HW_MAILBOX_DEBUG, "%s: Driver recieved standby request by core (%d) testId (%d)\n", __func__, dwpid, testId);  
    placeMessage(&testSlaveVarTable[dwpid].slaveMailbox, &message);
    
    td_hasStandbyRequest = TRUE;
}

