/*-----------------------------------------------------
 |
 |      preemption.c
 |
 |  Contains functions used in preemptive tasking
 |       configuration and setup.
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

HW_INT_VECTOR_MASK hw_criticalSectionBegin()
{

    return 0;
}

void hw_criticalSectionEnd(HW_INT_VECTOR_MASK intMask)
{

}

// Debug function. This is not public code currently.
void hw_resetPreemptionTimer(UINT32 ticks)
{

}
