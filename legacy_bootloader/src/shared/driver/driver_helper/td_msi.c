/*-----------------------------------------------------
 |
 |      td_msi.c
 |
 |  Contains function for the Bifrost test
 |  driver to send MSI to host and respond to tests.
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

void td_msi_reply(UINT32 vector, HW_TESTID testId, UINT32 cmd, UINT32 dwpid)
{
    HW_MESSAGE message;
    message.metadata.command = MSG_TS_CMD_MSI;
    message.metadata.size = sizeof(UINT32);
    
    hw_write32(&hw_extcomSpace.core[dwpid].cmd_buf.cmd.AsUINT32, cmd);
    hw_raise_msi((UINT8)vector);
    *(UINT32*) message.data = HW_S_OK;
    hw_printf(HW_MAILBOX_DEBUG, "%s: MSI sent for core (%d) testId (%d)\n", __func__, dwpid, testId);
    
    HW_EXTCOM_SIGNAL tmp_signal;
    do
    {
        hw_cacheInvalidateAddr((void*)&hw_extcomSpace.core[dwpid].cmd_buf.cmd.AsUINT32);
        tmp_signal.AsUINT32 = hw_read32(&hw_extcomSpace.core[dwpid].cmd_buf.cmd.AsUINT32);
    } while(tmp_signal.go != 0);
    
    placeMessage(&testSlaveVarTable[dwpid].slaveMailbox, &message);
}

