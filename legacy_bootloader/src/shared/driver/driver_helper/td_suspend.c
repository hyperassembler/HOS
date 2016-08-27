/*-----------------------------------------------------
 |
 |      td_suspend.c
 |
 |  Contains function for the Bifrost test driver
 |  to respond to tests for suspension request.
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

extern volatile BOOL td_inSuspensionMode;

void td_suspend_reply(UINT32 timeout, HW_TESTID testId, UINT32 dwpid)
{
    HW_MESSAGE message;
    message.metadata.command = MSG_TS_CMD_SUSPEND_TD;
    message.metadata.size = sizeof(UINT32);
    *(UINT32*) message.data = HW_S_OK;
    hw_printf(HW_MAILBOX_DEBUG, "%s: Driver goes to sleep mode requested by core (%d) testId (%d)\n", __func__, dwpid, testId);  
    placeMessage(&testSlaveVarTable[dwpid].slaveMailbox, &message);
    
    td_inSuspensionMode = TRUE;
    //hw_int_enable(1 << HW_TD_SUSPEND_TIMER);
    //hw_int_timerSetTimeout(HW_TD_SUSPEND_TIMER, timeout);
}

