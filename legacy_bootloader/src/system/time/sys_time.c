/*-----------------------------------------------------
 |
 |      sys_time.c
 |
 |  Contains functions for obtaining the current time.
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2016  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

#include "bifrost_private.h"
#include "hup_chip.h"

UINT64 sys_getTime()
{
    UINT64 cycleCount = 0;
    
    // TODO: when TCON is ready for use, update to use TCON for system timer
    //if(hw_pTestConfigs->bfinit.REAL_FABRIC)
    if(0)
    {
        const UINT32 cycleCountLow = hw_read32((UINT32 *)HUP_CHIP_TCON_TCON_TIMER_LO_ADDRESS);
        // Timer high bits and low bits overlap by one. Destructively shift away bottom bit.
        const UINT32 cycleCountHigh = (hw_read32((UINT32 *)HUP_CHIP_TCON_TCON_TIMER_HI_ADDRESS)) >> 1;
        // Combine the two 32 bit values into a 64 bit value
        // and shift left by 4 (timer only increments every 16ns)
        return (((UINT64)cycleCountHigh << 32) + (cycleCountLow)) << 4;
    }
    else
    {
        cycleCount = hw_getCycleCount();
        HW_PLATFORM plat = hw_get_platform();
        
        switch(plat)
        {
        case LINUX_PLATFORM:
            //
            // In the Linux platform we have no idea what the TSC
            // frequency is so we can't actually know what ratio
            // to multiply by to get realtime, nor should we need to.
            //
            break;
        case ISS_PLATFORM:
        case VISS_PLATFORM:
        case SIM_PLATFORM:
        case EMU_PLATFORM:
        case CHIP_PLATFORM:
        case RPP_PLATFORM:
            cycleCount *= 2;
            break;
        default:
            hw_errmsg("%s: invalid PLATFORM %u detected!\n", __func__, plat);
            break;
        }
    }
    
    return cycleCount;
}

