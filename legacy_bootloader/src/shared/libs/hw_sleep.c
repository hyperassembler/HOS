/*-----------------------------------------------------
 |
 |      hw_sleep.c
 |
 |  Contains functions for a software time-based sleep
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


int hw_sleep(UINT64 nanoseconds)
{
    HW_INT_VECTOR_MASK intMask = hw_criticalSectionBegin();

    UINT64 startTime = hw_getTime();
    while((hw_getTime() - startTime) < nanoseconds);
    
    hw_criticalSectionEnd(intMask);
    return 0;
}

