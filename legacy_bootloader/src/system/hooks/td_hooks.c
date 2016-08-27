/*-------------------------------------------------------
 |
 |     td_hooks.c
 |
 |     Project-specific implementations for
 |     testdriver hooks.
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

typedef struct {
UINT64 lastDumpTime;
UINT64 lastPerfMonTime;
UINT64 lastTempMonTime;
UINT64 lastPCIeCheckTime;
UINT64 lastPCIeReportTime;
UINT32 lastPCIeSpeed;
UINT64 jtm_interval;
UINT64 lastLedTime;
UINT64 curLedTime;
UINT32 lastLedIdx;
UINT32 lastLedMask;
UINT32 order;
} __attribute__ ((aligned(HW_MAX_CACHELINE_SIZE))) HW_SYSTEM_TD_DATA;

static MEM_DDR_WB HW_SYSTEM_TD_DATA system_td_data;

extern volatile UINT8 sys_tsStallFlag[HW_PROC_CNT];

void system_td_preloop()
{
    for(UINT32 ctr = 0; ctr < HW_PROC_CNT; ctr++)
    {
        sys_tsStallFlag[ctr] = 1;
    }
    
    // Initialize performance monitoring
    if (hw_read32(&hw_pTestConfigs->bfinit.PERFMON_ON) && 
        (hw_read32(&hw_pTestConfigs->platform) == CHIP_PLATFORM || 
         hw_read32(&hw_pTestConfigs->platform) == EMU_PLATFORM || 
         hw_read32(&hw_pTestConfigs->platform) == RPP_PLATFORM))
    {
        hw_status ("Perf monitoring enabled: initializing\n");
        // TODO: enable perfmon
        //td_perfmon_init();
    }
    
    //TODO: system-specific functions
    //if ((hw_get_jtm_interval() > 0) && (hw_read32(&hw_pTestConfigs->platform) == CHIP_PLATFORM))
    //{
    //    td_tempmon_init();
    //}
    
    //TODO: system-specific functions
    //if(hw_is_silicon())
    //{
    //#ifdef PCIE_DIAG
    //    hw_pcie_clear_correctable_error();
    //    hw_pcie_clear_uncorrectable_error();        
    //    gPCIeCorrectableErr = 0;
    //    gPCIeUncorrectableErr = 0;
    //    lastPCIeSpeed = hw_pcie_check_speed();
    //    hw_pcie_parse_speed(lastPCIeSpeed);
    //    gPCIeLinkSpeedChangeCnt = 0;
    //    gPCIeLinkTrainingCnt = 0;
    //#endif
    //#ifdef LED_DISPLAY
    //    hw_i2c_fpga_set_led(0xFFFFFF);
    //#endif
    //}
    
    // Initialize project specific variables needed by later hooks
    system_td_data.lastDumpTime = 0ull;
    system_td_data.lastPerfMonTime = 0ull;
    system_td_data.lastTempMonTime = hw_getTime();
    system_td_data.lastPCIeCheckTime = system_td_data.lastTempMonTime;
    system_td_data.lastPCIeReportTime = system_td_data.lastPCIeCheckTime;
    system_td_data.lastPCIeSpeed = 0;
    system_td_data.jtm_interval = 1000000000ull * hw_get_jtm_interval();
    system_td_data.lastLedTime = 0ull;
    system_td_data.curLedTime = 0ull;
    system_td_data.lastLedIdx = 0;
    system_td_data.lastLedMask = 0x7;
    system_td_data.order = 0;
}

void system_td_periodic()
{
    const UINT64 ledInterval = 50000000ull;
    
    //TODO: system-specific functions
    //if(hw_is_silicon())
    //{
    //#ifdef PCIE_DIAG
    //    //Check PCIe status
    //    UINT64 curTime = hw_getTime();
    //    if((curTime - lastPCIeCheckTime) >= PCIE_STATUS_CHECK_INTERVAL)
    //    {
    //        UINT32 curSpeed = hw_pcie_check_speed();
    //        if(curSpeed != lastPCIeSpeed)
    //        {
    //            hw_errmsg("PCIE: speed mode changed from Gen%d to Gen%d\n", lastPCIeSpeed, curSpeed);
    //            lastPCIeSpeed = curSpeed;
    //            gPCIeLinkSpeedChangeCnt ++;
    //        }
    //        
    //        if(hw_pcie_check_training())
    //        {
    //            hw_errmsg("PCIE: link re-training\n");
    //            gPCIeLinkTrainingCnt ++;
    //        }

    //        gPCIeUncorrectableErr = hw_pcie_check_uncorrectable_error(gPCIeUncorrectableErrCnt);
    //        if(gPCIeUncorrectableErr != 0)
    //        {
    //            hw_pcie_parse_uncorrectable_error_counter(gPCIeUncorrectableErrCnt);
    //            hw_pcie_clear_uncorrectable_error();                    
    //            gPCIeUncorrectableErr = 0;
    //        }
    //
    //        gPCIeCorrectableErr |= hw_pcie_check_correctable_error(gPCIeCorrectableErrCnt);
    //        lastPCIeCheckTime = curTime;
    //    }
    //    
    //    if((curTime - lastPCIeReportTime) >= PCIE_STATUS_REPORT_INTERVAL )
    //    {
    //        if(gPCIeCorrectableErr != 0)
    //        {
    //            hw_pcie_parse_correctable_error_counter(gPCIeCorrectableErrCnt);
    //            hw_pcie_clear_correctable_error();                  
    //            gPCIeCorrectableErr = 0;
    //        }
    //        lastPCIeReportTime = curTime;
    //    }
    //#endif
    //    //Led display
    //#ifdef LED_DISPLAY          
    //    if((curTime - lastLedTime) >= ledInterval)
    //    {
    //        hw_i2c_fpga_set_led(lastLedMask << lastLedIdx);
    //        if(order == 0)
    //        {
    //            lastLedIdx ++;
    //            if(lastLedIdx == 21)
    //            {
    //                order = 1;
    //            }
    //        }
    //        else
    //        {
    //            lastLedIdx --;
    //            if(lastLedIdx == 0)
    //            {
    //                order = 0;
    //            }
    //        }       
    //        lastLedTime = curTime;
    //    }
    //#endif          
    //}
    
    //
    // Periodically manage and dump performance counters
    // 
    // TODO: enable perfmon
    //if (hw_read32(&hw_pTestConfigs->bfinit.PERFMON_ON) && 
    //   (hw_read32(&hw_pTestConfigs->platform) == CHIP_PLATFORM || 
    //    hw_read32(&hw_pTestConfigs->platform) == EMU_PLATFORM || 
    //    hw_read32(&hw_pTestConfigs->platform) == RPP_PLATFORM))
    //{           
    //    if (hw_getTime() - lastPerfMonTime >= PERFMON_COLLECT_INTERVAL)
    //    {
    //        lastPerfMonTime = hw_getTime();
    //        td_perfmon_update(lastPerfMonTime);
    //    }
    //}

    // JTM
    //TODO: system-specific functions
    //if (jtm_interval && (hw_read32(&hw_pTestConfigs->platform) == CHIP_PLATFORM))
    //{
    //    if (hw_getTime() - lastTempMonTime >= jtm_interval)
    //    {
    //        lastTempMonTime = hw_getTime(); 
    //        td_tempmon_report_temps (lastTempMonTime);
    //    }
    //}
    
}

void system_td_postloop()
{
    //TODO: system-specific functions
    //if (jtm_interval && (hw_read32(&hw_pTestConfigs->platform) == CHIP_PLATFORM))
    //{    
    //    lastTempMonTime = hw_getTime();
    //    td_tempmon_report_temps (lastTempMonTime);    
    //}
    //
    //if(hw_is_silicon())
    //{
    //#ifdef PCIE_DIAG
    //    hw_pcie_check_correctable_error(gPCIeCorrectableErrCnt);
    //    hw_pcie_check_uncorrectable_error(gPCIeUncorrectableErrCnt);
    //    
    //    hw_pcie_parse_correctable_error_counter(gPCIeCorrectableErrCnt);
    //    hw_pcie_parse_uncorrectable_error_counter(gPCIeUncorrectableErrCnt);

    //    if(gPCIeLinkSpeedChangeCnt > 0)
    //    {
    //        hw_critical("ERROR: PCIE: speed mode changed count (%u)\n", gPCIeLinkSpeedChangeCnt);
    //    }

    //    if(gPCIeLinkTrainingCnt > 0)
    //    {
    //        hw_critical("ERROR: PCIE: link re-training count (%u)\n", gPCIeLinkTrainingCnt);
    //    }
    //#endif
    //#ifdef LED_DISPLAY
    //    hw_i2c_fpga_set_led(0x0);
    //#endif
    //}
}

// Node 0 is barred from power cycling in this project,
// and so is the node containing the testdriver
BOOL sys_td_reboot_condition_hook(UINT32 coreNum)
{
    return HW_SUPERNODE(coreNum) != HW_SUPERNODE(bifrostCachedGlobals.tpid) && HW_SUPERNODE(coreNum) != 0;
}

