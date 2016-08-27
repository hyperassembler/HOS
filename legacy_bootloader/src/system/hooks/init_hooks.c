/*-------------------------------------------------------
 |
 |     init_hooks.c
 |
 |     Project-specific implementations for
 |     initialization hooks.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2016  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#include <bifrost_private.h>

void sys_initHeapBlockInfo()
{
    if(hw_get_platform() == CHIP_PLATFORM)
    {
        hw_heapBlockInfo[HW_HEAP_BLOCK_HOSTMEM].base = HW_HEAP_HOSTMEM_CHIP_PLAT_BASE;
        hw_heapBlockInfo[HW_HEAP_BLOCK_HOSTMEM].size = HW_HEAP_HOSTMEM_CHIP_PLAT_SIZE;
    }
}

void sys_postParam_hook()
{
    // In ISS_RTL_MODE, overwrite ISS_PLATFORM to SIM_PLATFORM
    if(hw_get_platform() == ISS_PLATFORM && hw_read32(&hw_pTestConfigs->bfinit.ISS_RTL_MODE))
    {
        hw_write32(&hw_pTestConfigs->platform, SIM_PLATFORM);
    }
    
    //
    // Bifrost heap block setup, only test driver should do it
    //
    if(hw_getCoreNum() == bifrostCachedGlobals.tpid)
    {
        sys_initHeapBlockInfo();
    }
}

