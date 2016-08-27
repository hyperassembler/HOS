/*-----------------------------------------------------
 |
 |      hw_settimeout.c
 |
 |  Set various time limits.
 |
 |------------------------------------------------------
 |
 |  Revision History :
 |
 |    #5 - 15 February 2005 J. Hanes
 |         Drop hw_api_trace_enable; use SHARED_ERRMSG, SHARED_TRACE
 |
 |    #4 - 3 March 2004 J. Hanes
 |         Add "Error" to the error message.
 |
 |    #3 - 12 January 2004  J. Hanes
 |         Use hw_api_trace_enable to control interface trace.
 |         Use ctp_printf() instead of HW_printf()
 |
 |    #2 - 8 January 2004   J. Hanes
 |         Use stop-on-error to control stop on error.
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2003  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

#include "bifrost_private.h"


void
hw_setTimeout( UINT64 value )
{
    //HW_TS_PUBLIC_TESTDATA* publicTestData = (HW_TS_PUBLIC_TESTDATA*)hw_readptr(&testSlaveVars->publicTestData);
    //UINT32 currentTestNum = hw_read32(&testSlaveVars->dwCurrentXTest);
    //UINT64* timeoutField = &publicTestData[currentTestNum].testTimeout;

    //hw_write64(timeoutField, value);
    //hw_cacheFlushAddr(timeoutField);
}  /*  hw_settimeout()  */

