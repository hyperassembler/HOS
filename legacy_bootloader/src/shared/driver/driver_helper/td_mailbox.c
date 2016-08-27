/*-----------------------------------------------------
 |
 |      td_mailbox.c
 |
 |  Contains testdriver-specific functions for
 |  managing Bifrost's mailbox system
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

//
// Handles hw_flushMailbox command
//
void td_flush_mailbox_reply(UINT32 dwpid)
{
    HW_MESSAGE reply;
    reply.metadata.command = MSG_TD_ACK_CMD;
    reply.metadata.size = 0;
    placeMessage(&testSlaveVarTable[dwpid].slaveLowPrioMailbox, &reply);
}

