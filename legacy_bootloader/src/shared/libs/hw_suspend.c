/*-----------------------------------------------------
 |
 |      hw_suspend.c
 |
 |  Contains functions for a thread suspension and 
 |      unsuspension
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

void hw_suspend()
{
    HW_TS_PUBLIC_TESTDATA* publicTestData = (HW_TS_PUBLIC_TESTDATA*)hw_readptr(&testSlaveVars->publicTestData);
    UINT32 curTestIdx = hw_read32(&testSlaveVars->dwCurrentXTest);

    // Set test status to suspended
    publicTestData[curTestIdx].testStatus = HW_TEST_SUSPENDED;

    // Kick off context switch
    hw_yield();
}

void hw_unsuspend()
{
    HW_TS_PUBLIC_TESTDATA* publicTestData = (HW_TS_PUBLIC_TESTDATA*)hw_readptr(&testSlaveVars->publicTestData);
    UINT32 curTestIdx = hw_read32(&testSlaveVars->dwCurrentXTest);

    // Make test run-eligible
    publicTestData[curTestIdx].testStatus = HW_TEST_RUN_ELIGIBLE;

}

VOID hw_yield()
{
    //Turn off preemptive interrupt timer (if enabled in the first place)
    if(hw_pTestConfigs->bfinit.PREEMPTION_ON)
    {
        arch_int_stopPreemptionTimer();
    }

    // Set test status to yielded
    HW_TS_TESTDATA* testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);
    UINT32 curTestIdx = hw_read32(&testSlaveVars->dwCurrentXTest);
    hw_write32(&testData[curTestIdx].yielded, TRUE);

    // Start context switch
    hw_int_set(1 << HW_INT_TASK_SWITCH);
}
