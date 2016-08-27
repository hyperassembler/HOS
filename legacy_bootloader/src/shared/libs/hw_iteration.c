/*-----------------------------------------------------
 |
 |      hw_iteration.c
 |
 |  Contains functions which enable tests to
 |  modify the current Bifrost iteration count.
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
// Set the current test's iteration count.
// The next time Bifrost runs this test, the
// value of the iteration parameter will be
// this number.
//
void hw_setIterationCount(UINT32 count)
{
    HW_TS_TESTDATA* testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);
    UINT32 currentTestNum = hw_read32(&testSlaveVars->dwCurrentXTest);
    hw_write32(&testData[currentTestNum].dwIteration, count);
}
