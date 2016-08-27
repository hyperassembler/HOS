/*-------------------------------------------------------
 |
 |  platform_init.c
 |
 |  Platform specific initialization for RTL emulation
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2014  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
*/

#include "bifrost.h"

#define HUP_CN_MISC_CN_CTRL_OFFSET   0x10
#define HUP_CN_MISC_CN_CTRL_PERF_EN   0x1
#define HUP_CN_MISC_CN_CTRL_DTB_EN    0x2
#define HUP_CN_MISC_CN_CTRL_P0_DRESET 0x10
#define HUP_CN_MISC_CN_CTRL_P1_DRESET 0x20

// Called from the architecture specific initialization
void platform_init()
{
    //
    // De-assert Reset and enable performance counters
    //
    UINT32 cn_ctrl_reg = hw_reg_read32 (HW_ADDR_NODE_LOCAL_BASE, HUP_CN_MISC_CN_CTRL_OFFSET);

    // Enable Debug/Trace Unit & Perfmon clock
    cn_ctrl_reg |= HUP_CN_MISC_CN_CTRL_DTB_EN | HUP_CN_MISC_CN_CTRL_PERF_EN;

    // De-assert core0 and core1 debug logic reset
    cn_ctrl_reg &= ~(HUP_CN_MISC_CN_CTRL_P0_DRESET | HUP_CN_MISC_CN_CTRL_P1_DRESET);

    // Might do this twice, once for each core
    hw_reg_write32 (HW_ADDR_NODE_LOCAL_BASE, HUP_CN_MISC_CN_CTRL_OFFSET, cn_ctrl_reg);
}

