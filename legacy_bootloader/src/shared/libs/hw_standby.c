/*-----------------------------------------------------
 |
 |      hw_standby.c
 |
 |  Contains functions for request putting system into standby
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
#include <bifrost_intr.h>

BOOL32 hw_request_standby()
{
    BOOL32 ret = 0;
    UINT32 testID = hw_getMyInstanceID();

    HW_MESSAGE message = {{0}};
    message.metadata.command = MSG_TS_CMD_STANDBY;
    message.metadata.size = sizeof(UINT32);
    
    UINT32* datafield = (UINT32*) message.data;
    datafield[0] = testID;
    
    hw_irql_t prev_irql = hw_raise_irql(HW_IRQL_DISABLED_LEVEL);
    
    placeMessage(&testSlaveVars->driverMailbox, &message);
    
    while(!getMessage(&testSlaveVars->slaveMailbox, &message));
    ret = *(UINT32*) message.data;
    
    hw_lower_irql(prev_irql);
    
    if(HW_S_OK != ret)
    {
        hw_errmsg("%s: test driver didn't accept standby request(testID=%d)\n", __func__, testID);
        return FALSE;
    }
    
    return TRUE;
}

