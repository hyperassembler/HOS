/*-----------------------------------------------------
 |
 |      hw_testid.c
 |
 |  Contains functions to obtain Bifrost test ID from
 |  Bifrost test name, and for a slave to get their test name
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
#include "bifrost_thread.h"

HW_PATHID hw_getPathIDFromPathName(const char* testName)
{
    return (HW_PATHID)hw_strhash(testName);
}

HW_TESTID hw_getMyInstanceID()
{
    return hw_current_thread_id();
#if 0
    HW_TESTID ret = HW_INTERNAL_TESTID;
    UINT32 testIdx = hw_read32(&testSlaveVars->dwCurrentXTest);
    HW_TS_PUBLIC_TESTDATA* publicTestData = (HW_TS_PUBLIC_TESTDATA*)hw_readptr(&testSlaveVars->publicTestData);
    
    //
    // testIdx values larger than HW_TS_MAX_TESTS indicate that
    // a test is not running; the core is in an internal state.
    //
    if(testIdx < HW_TS_MAX_TESTS)
    {
        ret = hw_read32(&publicTestData[testIdx].testInstanceID);
    }
    
    return ret;
#endif
}

UINT32 hw_getBufferIdx()
{
    UINT32 ret = ~0u;
    
    //UINT32 testIdx = hw_read32(&testSlaveVars->dwCurrentXTest);
    HW_TS_TESTDATA* testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);
    
    //
    // testIdx values larger than HW_TS_MAX_TESTS indicate that
    // a test is not running; the core is in an internal state.
    //
    //if(testIdx < HW_TS_MAX_TESTS)
    //{
    //    ret = hw_read32(&testData[testIdx].testBufferIndex);
    //}
    
    return ret;
}

UINT32 hw_getGlobalIdx()
{
    UINT32 ret = ~0u;
    
    //UINT32 testIdx = hw_read32(&testSlaveVars->dwCurrentXTest);
    HW_TS_TESTDATA* testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);
    
    //
    // testIdx values larger than HW_TS_MAX_TESTS indicate that
    // a test is not running; the core is in an internal state.
    //
    //if(testIdx < HW_TS_MAX_TESTS)
    //{
    //    ret = hw_read32(&testData[testIdx].testGlobalIndex);
    //}
    
    return ret;
}

UINT32 ts_getFuncIndexFromPathID(HW_PATHID pathID)
{
    UINT32 i = 0;
    UINT32 ret = ~0u;
    while(hw_tl_master[hw_archTypeList[hw_getCoreNum()]][i].testPathName != (const char*) HW_XLIST_TERMINATION)
    {
        if(hw_getPathIDFromPathName((const char*)hw_tl_master[hw_archTypeList[hw_getCoreNum()]][i].testPathName) == pathID)
        {
            ret = i;
            break;
        }
        i++;
    }
    
    if(ret == ~0u)
    {
        hw_errmsg("%s: no match found for path ID 0x%llx\n", __func__, pathID);
    }
    
    return ret;
}
