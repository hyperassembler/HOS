/*-----------------------------------------------------
 |
 |      time.c
 |
 |  Contains functions for obtaining the current time
 |      with the x86 architecture.
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

HW_RESULT arch_int_startPreemptionTimer()
{
    return S_OK;
}

HW_RESULT arch_int_stopPreemptionTimer()
{
    return S_OK;
}

UINT64 hw_getCycleCount()
{
    UINT64 currentTime = RDTSC();
    
    //
    // The TSC register increments at a constant rate regardless
    // of power state so we can generally count on it for a realtime
    // reading.
    //
    return currentTime - bifrostCachedGlobals.hw_beginning_cycle_count;
}

