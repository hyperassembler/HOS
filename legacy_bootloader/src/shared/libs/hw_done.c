/*-----------------------------------------------------
 |
 |      hw_done.c
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2003-2015  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

#include "bifrost_private.h"

void hw_doneBackend(HW_TEST_RUN_STATE dwCondition, const char* p_func, UINT32 runEndConditions)
{
    HW_TS_TESTDATA* p_testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);
    HW_TS_PUBLIC_TESTDATA* p_publicTestData = (HW_TS_PUBLIC_TESTDATA*)hw_readptr(&testSlaveVars->publicTestData);
    
    if(dwCondition < HW_TEST_FAIL)
    {
        hw_errmsg("%s: called with invalid condition %d\n", __func__, dwCondition);
        dwCondition = HW_TEST_FAIL;
    }
    
    UINT32 errorCnt = hw_getErrorCnt();
    if(errorCnt != 0)
    {
        dwCondition = HW_TEST_FAIL;
    }
    
    UINT32 dwCurrentXTest = hw_read32(&testSlaveVars->dwCurrentXTest);
    UINT32 dwIteration = hw_read32(&p_testData[dwCurrentXTest].dwIteration);
    UINT32 testIdx = hw_read32(&p_publicTestData[dwCurrentXTest].testPointerIndex);
    
    if (dwCondition == HW_TEST_FAIL)
    {
        hw_post(TS_ERR_TEST_FAIL);
        hw_log(HW_EVENT_FAILED_TEST, (UINT64)hw_tl_master[hw_archTypeList[hw_getCoreNum()]][testIdx].testPathName, (UINT64)p_func, dwIteration, errorCnt);
    }
    else if(dwCondition == HW_TEST_PASS)
    {
        hw_post(TS_STATUS_TEST_PASS);
    }
    
    UINT64 currentTime = hw_getTime();
    UINT64 runtimeStart = hw_read64(&p_testData[dwCurrentXTest].runtimeStart);
    UINT64 runtimeEnd = runtimeStart + hw_read64(&hw_pTestConfigs->runtime);
    
    // Disable interrupts while changing test status,
    // so that being preempted after writing status,
    // but before flushing it, doesn't cause the
    // test to be marked as failed.
    HW_INT_VECTOR_MASK oldMask = hw_int_disable(HW_INT_PROTECTION_MASK);
    
    if (dwCondition != HW_TEST_FAIL &&
        currentTime < runtimeEnd    &&
        !(runEndConditions & HW_TS_SKIP_RERUN))
    {
        // Set testStatus to re-run,
        // let the testslave run the
        // calling test again
        hw_status("MSG_TS_START_RERUN (%llu / %llu)\n", currentTime, runtimeEnd);
        hw_write32((UINT32*)&p_publicTestData[dwCurrentXTest].testStatus, HW_TEST_RERUN);
        hw_cacheFlushAddr(&p_publicTestData[dwCurrentXTest].testStatus);
    }
    else
    {
        //
        // Update the test status
        //
        hw_write32((UINT32*)&p_publicTestData[dwCurrentXTest].testStatus, dwCondition);
        hw_cacheFlushAddr(&p_publicTestData[dwCurrentXTest].testStatus);
        
        if (!(runEndConditions & HW_TS_SKIP_CLEANUP))
        {
            hw_write32(&testSlaveVars->dwTestCleanupCnt, hw_read32(&testSlaveVars->dwTestCleanupCnt) + 1);
        }
        hw_write32(&p_testData[dwCurrentXTest].runEndConditions, runEndConditions);
        hw_cacheFlushAddr((UINT32*)&p_testData[dwCurrentXTest].runEndConditions);
    }
    
    hw_int_enable(oldMask);
}

void hw_fatalBackend(const char* p_func)
{
    hw_flushMailbox();
    
    HW_MESSAGE errmsg;
    errmsg.metadata.command = MSG_TS_FATAL;
    errmsg.metadata.size = 0;
    placeMessage(&testSlaveVars->driverMailbox, &errmsg);
    
    HW_TS_TESTDATA* p_testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);
    HW_TS_PUBLIC_TESTDATA* p_publicTestData = (HW_TS_PUBLIC_TESTDATA*)hw_readptr(&testSlaveVars->publicTestData);
    UINT32 dwCurrentXTest = hw_read32(&testSlaveVars->dwCurrentXTest);
    UINT32 dwIteration = hw_read32(&p_testData[dwCurrentXTest].dwIteration);
    UINT32 testIdx = hw_read32(&p_publicTestData[dwCurrentXTest].testPointerIndex);
    UINT32 errorCnt = hw_getErrorCnt();
    
    hw_log(HW_EVENT_FAILED_TEST, (UINT64)hw_tl_master[hw_archTypeList[hw_getCoreNum()]][testIdx].testPathName, (UINT64)p_func, dwIteration, errorCnt);
    
    hw_terminate();
}

void hw_incrementErrorCnt()
{
    HW_TS_TESTDATA* p_testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);
    UINT32 dwCurrentXTest = hw_read32(&testSlaveVars->dwCurrentXTest);
    UINT32 errorCnt = hw_read32(&p_testData[dwCurrentXTest].errorCnt);
    hw_write32(&p_testData[dwCurrentXTest].errorCnt, errorCnt + 1);
}

UINT32 hw_getErrorCnt()
{
    HW_TS_TESTDATA* p_testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);
    UINT32 dwCurrentXTest = hw_read32(&testSlaveVars->dwCurrentXTest);
    return hw_read32(&p_testData[dwCurrentXTest].errorCnt);
}

