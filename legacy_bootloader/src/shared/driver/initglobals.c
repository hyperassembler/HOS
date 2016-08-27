/*-------------------------------------------------------
 |
 |     initglobals.c
 |
 |     At the start of execution, sets up Bifrost
 |     globals, memory map and configuration
 |     file. At the end of execution, frees them.
 |     initGlobals is the entry point from
 |     Verilog testbench.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2011-2013  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#include "bifrost_private.h"

extern void checkStructSizes();

void sys_postParam_hook() __attribute__ ((weak));
void sys_postParam_hook() {}

void print_build_info()
{
    char config[28];
    hw_strncpyfrommem(config, hw_pTestConfigs->config, _countof(config));
    
    hw_critical("\n-------------------------\n");
    hw_critical("Bifrost build information\n");
    hw_critical("-------------------------\n\n");
    //
    // Print build information from external file
    //
    print_extern_build_info();
    hw_critical("SEED=%u\n", hw_read32(&hw_pTestConfigs->seed));
    hw_critical("SUITE_RAND_SEED=%u\n", hw_read32(&hw_pTestConfigs->suite_rand_seed));
    hw_critical("VERBOSITY=0x%08x\n\n", hw_read32(&hw_pTestConfigs->verbosity));
    hw_critical("PLATFORM=%u\n\n", hw_read32(&hw_pTestConfigs->platform));
    hw_critical("CONF=%s\n\n", config);
    hw_critical("TIME=%llu\n\n", hw_read64(&hw_pTestConfigs->runtime));
    hw_critical("-------------------------\n");
    hw_critical("End Bifrost build information\n");
    hw_critical("-------------------------\n\n\n");
}

void readConfig()
{
    //
    // Make sure the config binary is compatible with this code's definition of HW_TESTCFGS.
    //
    hw_assert(hw_read32(&hw_pTestConfigs->version) == HW_TESTCFGS_VERSION);
    
    //
    // Now that the test configs are set up,
    // check what thread the driver is on.
    //
    bifrostCachedGlobals.tpid = hw_read32(&hw_pTestConfigs->tpid);
    
    //
    // Check whether sync mode is enabled or not
    //
    bifrostCachedGlobals.hw_bRunInSync = hw_read32(&hw_pTestConfigs->sync);
    
    // Disable non-test prints if we're conserving runtime,
    // because they're too slow. But, leave error messages
    // enabled so we know if something went wrong.
    if(hw_pTestConfigs->bfinit.CONSERVE_RUNTIME)
    {
        bifrostCachedGlobals.hw_global_verbosity = 0x1;
    }
    else
    {
        bifrostCachedGlobals.hw_global_verbosity = hw_read32(&hw_pTestConfigs->verbosity);
    }
}

void setupBifrostGlobalPointers()
{
    // Initialize internal and external print buffer pointer
    hw_writeptr(&testSlaveVars->internalPrintBuffer, (UINTPTR)td_internal_print_buffer);
    
    hw_writeptr(&hw_extcomSpace.ext_buf, (UINTPTR)&ext_print_buffer);
    
    //
    // Initialize global state pointers
    //
    if(hw_read32(&hw_pTestConfigs->bfinit.POLL_ADDRS_IN_DRAM) == TRUE)
    {
        p_td_global_state = &td_global_state_dram;
        for(UINT32 ctr = 0; ctr < HW_PROC_CNT; ctr++)
        {
            p_ts_global_states[ctr] = &ts_global_states_dram[ctr];
        }
    }
    else
    {
        p_td_global_state = &td_global_state;
        for(UINT32 ctr = 0; ctr < HW_PROC_CNT; ctr++)
        {
            p_ts_global_states[ctr] = &ts_global_states[ctr];
        }
    }
    
    HW_MAILBOX_IDXS (*idxs)[HW_PROC_CNT][HW_MAILBOX_ID_CNT];
    if(hw_read32(&hw_pTestConfigs->bfinit.POLL_ADDRS_IN_DRAM) == TRUE)
    {
        idxs = &mailboxIdxs_dram;
    }
    else
    {
        idxs = &mailboxIdxs;
    }
    
    for(UINT32 i = 0; i < HW_PROC_CNT; i++)
    {
        hw_writeptr(&testSlaveVarTable[i].driverMailbox.headPtr, (UINTPTR)&(*idxs)[i][HW_DRIVER_MAILBOX].head);
        hw_writeptr(&testSlaveVarTable[i].driverMailbox.tailPtr, (UINTPTR)&(*idxs)[i][HW_DRIVER_MAILBOX].tail);
        hw_writeptr(&testSlaveVarTable[i].driverLowPrioMailbox.headPtr, (UINTPTR)&(*idxs)[i][HW_DRIVER_LOW_PRIO_MAILBOX].tail);
        hw_writeptr(&testSlaveVarTable[i].driverLowPrioMailbox.tailPtr, (UINTPTR)&(*idxs)[i][HW_DRIVER_LOW_PRIO_MAILBOX].head);
        hw_writeptr(&testSlaveVarTable[i].slaveMailbox.headPtr, (UINTPTR)&(*idxs)[i][HW_SLAVE_MAILBOX].head);
        hw_writeptr(&testSlaveVarTable[i].slaveMailbox.tailPtr, (UINTPTR)&(*idxs)[i][HW_SLAVE_MAILBOX].tail);
        hw_writeptr(&testSlaveVarTable[i].slaveLowPrioMailbox.headPtr, (UINTPTR)&(*idxs)[i][HW_SLAVE_LOW_PRIO_MAILBOX].head);
        hw_writeptr(&testSlaveVarTable[i].slaveLowPrioMailbox.tailPtr, (UINTPTR)&(*idxs)[i][HW_SLAVE_LOW_PRIO_MAILBOX].tail);
        
        hw_cacheFlushAddr(&testSlaveVarTable[i].driverMailbox.headPtr);
        hw_cacheFlushAddr(&testSlaveVarTable[i].driverMailbox.tailPtr);
        hw_cacheFlushAddr(&testSlaveVarTable[i].driverLowPrioMailbox.headPtr);
        hw_cacheFlushAddr(&testSlaveVarTable[i].driverLowPrioMailbox.tailPtr);
        hw_cacheFlushAddr(&testSlaveVarTable[i].slaveMailbox.headPtr);
        hw_cacheFlushAddr(&testSlaveVarTable[i].slaveMailbox.tailPtr);
        hw_cacheFlushAddr(&testSlaveVarTable[i].slaveLowPrioMailbox.headPtr);
        hw_cacheFlushAddr(&testSlaveVarTable[i].slaveLowPrioMailbox.tailPtr);
    }
    
    // Each core sets up its own necessary struct pointers
    UINT32 dwpid = hw_getCoreNum();
    
    hw_writeptr(&testSlaveVars->testData, (UINTPTR)__bf_kernelHW_TS_TESTDATA_pointers[dwpid]);
    hw_writeptr(&testSlaveVars->publicTestData, (UINTPTR)__bf_kernelHW_TS_PUBLIC_TESTDATA_pointers[dwpid]);
    hw_cacheFlushAddr(&testSlaveVars->publicTestData);
    hw_writeptr(&testSlaveVars->allocTracker, (UINTPTR)__bf_kernelHW_ALLOC_TRACKER_pointers[dwpid]);
    if(dwpid == bifrostCachedGlobals.tpid)
    {
        hw_writeptr(&testSlaveVars->eventLog, (UINTPTR)&td_eventLog);
    }
    else
    {
        hw_writeptr(&testSlaveVars->eventLog, (UINTPTR) __bf_kernelHW_EVENT_LOG_pointers[dwpid]);
        hw_cacheFlushAddr(&testSlaveVars->eventLog);
    }
}

void initGlobals()
{
    UINT32 dwpid = hw_getCoreNum();
    
    readConfig();
    setupBifrostGlobalPointers();
    
    sys_postParam_hook();
    
    //
    // We only want these to be done once,
    // so let the testdriver do it.
    //
    if(dwpid == bifrostCachedGlobals.tpid)
    {
        //
        // Initialize clock.
        //
        bifrostCachedGlobals.hw_beginning_cycle_count = hw_getCycleCount();
        
        // Skip printing build info when conserving
        // runtime because it's too slow.
        if(!hw_pTestConfigs->bfinit.CONSERVE_RUNTIME)
        {
            print_build_info();
        }
    }
    
    checkStructSizes();
    
    hw_post(TD_STATUS_INITGLOBALS_FINISH);
}

