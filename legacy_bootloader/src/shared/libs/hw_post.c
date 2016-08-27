/*-----------------------------------------------------
 |
 |      hw_post.c
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

// Displays POST codes or the environment equivalent.
// To be overridden by platform code if necessary.
void plat_post(HW_STATUSCODE code, UINT32 testID) __attribute__ ((weak));
void plat_post(HW_STATUSCODE code, UINT32 testID)
{
    // Standalone configs don't have any external support
    if(hw_pTestConfigs->bfinit.STANDALONE)
    {
        hw_printf(HW_BOOT_DEBUG, "core %02u, test ID %03u: Bifrost POST code 0x%x\n", hw_getCoreNum(), testID & 0xFF, code);
    }
    else if(hw_pTestConfigs->bfinit.HOSTCMD_EN)
    {
        hw_hostcmd_post(code, testID);
    }
}

void hw_post(HW_STATUSCODE code)
{
    plat_post(code, hw_getMyInstanceID());
}

