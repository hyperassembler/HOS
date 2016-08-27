/*-----------------------------------------------------
 |
 |      hw_setteststate.c
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2012  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

#include "bifrost_private.h"

void hw_setTestState(HW_TEST_RUN_STATE newState, UINT32 core, UINT32 testIdx)
{
    if(core == HW_CORE_DEFAULT)
    {
        core = hw_getCoreNum();
    }
    
    if(testIdx == HW_NO_TEST_INDEX)
    {
        testIdx = hw_read32(&testSlaveVars->dwCurrentXTest);
    }
    
    HW_TS_PUBLIC_TESTDATA* publicTestData = (HW_TS_PUBLIC_TESTDATA*)hw_readptr(&testSlaveVarTable[core].publicTestData);
    
    hw_cacheInvalidateAddr(&publicTestData[testIdx].testStatus);
    HW_TEST_RUN_STATE testStatus = (HW_TEST_RUN_STATE)hw_read32((UINT32*)&publicTestData[testIdx].testStatus);
    
    if (testStatus < HW_TEST_END)
    {
        switch(newState)
        {
        case HW_TEST_RUN_ELIGIBLE:
        case HW_TEST_SUSPENDED:
        case HW_TEST_SKIP_ITERATION:
            hw_write32((UINT32*)&publicTestData[testIdx].testStatus, newState);
            hw_cacheFlushAddr(&publicTestData[testIdx].testStatus);
            break;
        default:
            hw_errmsg("%s called for core %d, test ID %d with invalid state %d!\n", __func__, core, hw_read32((UINT32*)&publicTestData[testIdx].testInstanceID), newState);
            break;
        }
    }
    else
    {
        hw_errmsg("%s called for core %d, test ID %d after test has already ended!\n", __func__, core, hw_read32((UINT32*)&publicTestData[testIdx].testInstanceID));
    }
}

