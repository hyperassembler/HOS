/*-----------------------------------------------------
 |
 |      hw_vitag.c
 |
 |  Provides a function to display POST codes
 |  or the environment equivalent.
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2013  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */


#include "bifrost_private.h"

VOID hw_vitag(UINT32 code)
{
#if 0
    UINT32 dwpid = hw_getCoreNum();
    UINT32 testIdx = hw_read32(&testSlaveVars->dwCurrentXTest);
    HW_TS_PUBLIC_TESTDATA* testData = (HW_TS_PUBLIC_TESTDATA*)hw_readptr(&testSlaveVars->publicTestData);
    UINT32 testPointerIndex = hw_read32(&testData[testIdx].testPointerIndex);
    UINT32 testPathCRC = hw_tl_master[hw_archTypeList[dwpid]][testPointerIndex].testPathHash;

    UINT64 vitag_code;
    
    // Code is formatted as:
    // [63:32] : Testname crc32
    // [31:24] : CPUID
    // [23:0]  : code
    vitag_code = (((UINT64)testPathCRC) << 32) | ((dwpid & 0x3F) << 24) | (code & 0xFFFFFF);
    hw_printf(HW_VITAG, "VITAG:0x%016llx\n", vitag_code);
#endif
}
