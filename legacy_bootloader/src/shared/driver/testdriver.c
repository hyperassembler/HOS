/*-------------------------------------------------------
 |
 |     testdriver.c
 |
 |     The test manager that controls test setup and 
 |     and execution on all hardware threads.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2011  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#include "bifrost_thread.h"
#include "bifrost_private.h"

extern void arch_suspend();
#define PCIE_DIAG

// State of the testdriver's state machine:
// One copy in DRAM and one in SRAM
HW_BIFROST_STATE td_global_state_dram;
MEM_SRAM_UC HW_BIFROST_STATE td_global_state;
volatile HW_BIFROST_STATE* p_td_global_state;

HW_BIFROST_STATE ts_global_states_dram[HW_PROC_CNT];
MEM_SRAM_UC HW_BIFROST_STATE ts_global_states[HW_PROC_CNT];
volatile HW_BIFROST_STATE* p_ts_global_states[HW_PROC_CNT];

// Information about each testslave
HW_TD_TS_DATA td_ts_slaveData[HW_PROC_CNT] __attribute__ ((section (".ddr_wb.bss")));

// Number of active testslaves
static UINT32 td_ts_active_cnt = 0;

// Keeps track of global pass/fail state
BOOL td_testDriverGlobalPass;

// Determine whether to terminate all tests
extern volatile BOOL hw_bTerminateAllTests;

//Determine whether to stay in suspension mode
volatile BOOL td_inSuspensionMode = FALSE;

//Determine whether has standby request
volatile BOOL td_hasStandbyRequest = FALSE;

// Keep track of PCIe error
#ifdef PCIE_DIAG
static UINT32 gPCIeCorrectableErr = 0;
static UINT32 gPCIeUncorrectableErr = 0;
static UINT32 gPCIeCorrectableErrCnt[32] = {0};
static UINT32 gPCIeUncorrectableErrCnt[32] = {0};
static UINT32 gPCIeLinkSpeedChangeCnt = 0;
static UINT32 gPCIeLinkTrainingCnt = 0;
#endif

extern BOOL need_dram_heap_init();

// Periodic testdriver functions.
// To be overridden by project-specific
// functions, if necessary.
void system_td_preloop() __attribute__ ((weak));
void system_td_preloop() {}
void system_td_periodic() __attribute__ ((weak));
void system_td_periodic() {}
void system_td_postloop() __attribute__ ((weak));
void system_td_postloop() {}
BOOL sys_td_reboot_condition_hook(UINT32 coreNum) __attribute__ ((weak));
BOOL sys_td_reboot_condition_hook(UINT32 coreNum)
{
    return TRUE;
}

HW_RESULT init_testDriver()
{
    HW_RESULT hr = HW_S_OK;
    HW_TESTXENTRY* tsx = NULL;
    UINT32 coreNum = 0;
    
    hw_post(TD_STATUS_INIT_START);
    
    hw_status("%s: Entering\n", __func__);
    
    //
    // Initialize processor states
    //
    *p_td_global_state = HW_TS_IDLE;
    for(UINT32 dwpid = 0; dwpid < HW_PROC_CNT; dwpid++)
    {
        *p_ts_global_states[dwpid] = HW_TS_IDLE;
        td_ts_slaveData[dwpid].state = HW_TS_IDLE;
        td_ts_slaveData[dwpid].currentTest = HW_NO_TEST_INDEX;
        td_ts_slaveData[dwpid].current_playnice_exceed_count = 1;
        td_ts_slaveData[dwpid].testCnt = 0;
    }
    
    //
    // Analyze test configuration list to find active test slaves
    //
    tsx = hw_pTestConfigs->entryList;
    for(UINT32 i = 0; (coreNum = hw_read32(&tsx[i].coreNumber)) != HW_XLIST_TERMINATION; ++i)
    {
        hw_status("Reading test %d, assigning to core %d", i, coreNum);
        if(!td_ts_slaveData[coreNum].active)
        {
            hw_status(": Core %u enabled\n", coreNum);
            td_ts_slaveData[coreNum].active = TRUE;
        }
        else
        {
            hw_status("\n");
        }
        
        td_ts_slaveData[coreNum].testData[td_ts_slaveData[coreNum].testCnt].testInstanceID = hw_read32(&(tsx[i].testInstanceID));
        td_ts_slaveData[coreNum].testCnt++;
    }
    
    td_ts_active_cnt = 0;
    for(UINT32 dwpid = 0; dwpid < HW_PROC_CNT; dwpid++)
    {
        if(td_ts_slaveData[dwpid].active == TRUE)
        {
            td_ts_active_cnt++;
        }
    }

    hw_status("%d active processors.\n", td_ts_active_cnt);
    if(0 == td_ts_active_cnt)
    {
        hr = HW_E_TERMINATE;
    }
    hw_status("%s: Exiting\n", __func__);
    
    return hr;
}

//
// Performs a non-blocking state check across all test slaves
// to see whether they're synced to a given state
//
BOOL slavesInSync_testDriver(HW_BIFROST_STATE dwCondition, BOOL accept_reboot)
{
    UINT32 i = 0;
    
    //
    // Check that test slave state corresponds
    // with condition
    //
    for (i = 0; i < HW_PROC_CNT; i++)
    {
        // Don't check status if this core is not active or is the testdriver
        if (i != bifrostCachedGlobals.tpid && td_ts_slaveData[i].active == TRUE)
        {
            // If all of the following are NOT true then return false:
            // -Core state == dwCondition
            // -Core state == HW_TS_EXIT
            // -Core state == HW_TS_REBOOT and accept_reboot == TRUE
            if (!(td_ts_slaveData[i].state == dwCondition || td_ts_slaveData[i].state == HW_TS_EXIT || (accept_reboot && td_ts_slaveData[i].state == HW_TS_REBOOT)))
            {
                return FALSE;
            }
        }
    }
    
    return TRUE;
}

//
// Issue commands to all test slaves
//
HW_RESULT issueCmdSync_testDriver(HW_BIFROST_STATE state)
{
    UINT32 i;
    HW_MESSAGE send;
    hw_status("%s: Entering\n", __func__);
    
    for (i = 0; i < HW_PROC_CNT; i++)
    {
        if (i != bifrostCachedGlobals.tpid && td_ts_slaveData[i].active == TRUE && td_ts_slaveData[i].state != HW_TS_EXIT && td_ts_slaveData[i].state != HW_TS_REBOOT)
        {
            *p_ts_global_states[i] = state;
        }
    }
    
    return HW_S_OK;
}


HW_RESULT processMailbox_testDriver(UINT32 dwpid)
{
    static HW_RESULT hr = HW_S_OK;
    HW_RESULT call_rc;
    
    HW_MESSAGE latestMessage, tempOut;
    
    UINT32* data = (UINT32*) latestMessage.data;
    PTR_FIELD* ptrData = (PTR_FIELD*) latestMessage.data;
    
    //
    // Check for new messages
    //
    if (getMessage(&testSlaveVarTable[dwpid].driverMailbox, &latestMessage))
    {
        //
        // Parse command
        //
        UINT64 timeStamp = hw_getTime();
        switch (latestMessage.metadata.command)
        {
            case MSG_TS_ACK_TEST_SETUP:
                td_ts_slaveData[dwpid].state = HW_TS_SETUP;
                hw_status("Time %llu, %u MSG_TS_ACK_TEST_SETUP\n", timeStamp,
                          dwpid);
                break;
            case MSG_TS_ACK_INIT:
                td_ts_slaveData[dwpid].state = HW_TS_INIT;
                hw_status("Time %llu, %u MSG_TS_ACK_INIT\n", timeStamp,
                          dwpid);
                break;
            case MSG_TS_ACK_RUN:
                td_ts_slaveData[dwpid].state = HW_TS_RUN;
                hw_status("Time %llu, %u MSG_TS_ACK_RUN\n", timeStamp,
                          dwpid);
                break;
            case MSG_TS_ACK_CLEANUP:
                td_ts_slaveData[dwpid].state = HW_TS_CLEANUP;
                hw_status("Time %llu, %u MSG_TS_ACK_CLEANUP\n", timeStamp,
                          dwpid);
                break;
            case MSG_TS_DONE_TEST_SETUP:
                td_ts_slaveData[dwpid].state = HW_TS_DONE_SETUP;
                hw_status("Time %llu, %u MSG_TS_DONE_TEST_SETUP\n", timeStamp,
                          dwpid);
                break;
            case MSG_TS_DONE_INIT:
                td_ts_slaveData[dwpid].state = HW_TS_DONE_INIT;
                hw_status("Time %llu, %u MSG_TS_DONE_INIT\n", timeStamp,
                          dwpid);
                break;
            case MSG_TS_DONE_RUN:
                td_ts_slaveData[dwpid].state = HW_TS_DONE_RUN;
                hw_status("Time %llu, %u MSG_TS_DONE_RUN\n", timeStamp,
                          dwpid);
                break;
            case MSG_TS_DONE_CLEANUP:
                td_ts_slaveData[dwpid].state = HW_TS_DONE_CLEANUP;
                hw_status("Time %llu, %u MSG_TS_DONE_CLEANUP\n", timeStamp,
                          dwpid);
                break;
            case MSG_TS_EXIT:
                td_ts_slaveData[dwpid].testsPassed = *(UINT32*) latestMessage.data;
                hw_status("Time %llu, %u MSG_TS_EXIT\n", timeStamp,
                          dwpid);
                
                // Reboot the node if and only if ALL of the following conditions are true:
                // -The core has not already rebooted bfinit.POWER_MANAGER_CYC times
                // -The core has at least one test that passed
                // -Any additional project-specific conditions
                if(td_ts_slaveData[dwpid].power_cycle_cnt < hw_read32(&hw_pTestConfigs->bfinit.POWER_MANAGER_CYC) && td_ts_slaveData[dwpid].testsPassed >= 1 && sys_td_reboot_condition_hook(dwpid))
                {
                    td_ts_slaveData[dwpid].state = HW_TS_REBOOT;
                }
                else
                {
                    td_ts_slaveData[dwpid].state = HW_TS_EXIT;
                }
                break;
            case MSG_TS_FATAL:
                td_ts_slaveData[dwpid].active = HW_THREAD_TERMINATED;
                hw_status("Time %llu, %u MSG_TS_FATAL\n", timeStamp,
                          dwpid);
                break;
            //
            // Non-state messages
            //
            case MSG_TS_CMD_MESSAGE:
                td_sendMessage_reply(latestMessage);
                break;
            case MSG_TS_CMD_ALLOC:
                td_alloc_reply((HW_HEAP_BLOCK)ptrData[0], ptrData[1], ptrData[2], ptrData[3], ptrData[4], &testSlaveVarTable[dwpid].slaveMailbox, dwpid);
                break;
            case MSG_TS_CMD_FREE:
                call_rc = td_free((HW_HEAP_BLOCK)ptrData[0], ptrData[1], ptrData[2]);
                if(HW_S_OK != call_rc)
                {
                    hw_errmsg("%s: hw_free from core %d returns with error code %d!\n", __func__, dwpid, call_rc);
                }
                break;
            case MSG_TS_CMD_FREE_ALL:
                td_freeAll((HW_HEAP_BLOCK)ptrData[0], ptrData[1]);
                break;
                
            case MSG_TS_CMD_REG_INT:      
                break;
            case MSG_TS_CMD_CLEAR_INT:
                break;
                
            case MSG_TS_CMD_READ_INT:
                break;

            case MSG_TS_CMD_MSI:
                td_msi_reply(data[0], data[1], data[2], dwpid);
                break;

            case MSG_TS_CMD_SUSPEND_TD:
                td_suspend_reply(data[0], data[1], dwpid);
                break;

            case MSG_TS_CMD_STANDBY:
                td_standby_reply(data[0], dwpid);
                break;

            default:
                hw_errmsg("ERROR %s: unsupported command %d received from %d\n", __func__, latestMessage.metadata.command, dwpid);
                hr = HW_E_UNSUPPORTED_COMMAND;
                hw_post(TD_ERR_UNSUPPORTED_COMMAND_MESSAGE);
                break;
        }
    }
    else if (getMessage(&testSlaveVarTable[dwpid].driverLowPrioMailbox, &latestMessage))
    {
        switch(latestMessage.metadata.command)
        {
            case MSG_TS_CMD_PRINTF:
                td_printf_reply(latestMessage, dwpid);
                break;
                
            case MSG_TS_CMD_FLUSH_MAILBOX:
                td_flush_mailbox_reply(dwpid);
                break;
                
            default:
                hw_errmsg("ERROR %s: unsupported command %d received from %d\n", __func__, latestMessage.metadata.command, dwpid);
                hr = HW_E_UNSUPPORTED_COMMAND;
                hw_post(TD_ERR_UNSUPPORTED_COMMAND_MESSAGE);
                break;
        }
    }
    
    return hr;
}

void timeoutCheck_testDriver(UINT32 dwpid)
{
#if 0
    UINT64 curTime = hw_getTime();
    UINT64 timeout = 0;
    UINT64 elapsed = 0;
    UINT32 curTest = hw_read32(&testSlaveVarTable[dwpid].dwCurrentXTest);   
    HW_TS_PUBLIC_TESTDATA* publicTestData = (HW_TS_PUBLIC_TESTDATA*)hw_readptr(&testSlaveVarTable[dwpid].publicTestData);

    // If no test running
    if(curTest == HW_NO_TEST_INDEX)
    {
        return;
    }
    
    if(td_ts_slaveData[dwpid].currentTest == HW_NO_TEST_INDEX)
    {   
        td_ts_slaveData[dwpid].currentTest = curTest;
        td_ts_slaveData[dwpid].currentIterationStartTime = curTime;
        td_ts_slaveData[dwpid].current_playnice_exceed_count = 1;
        return;
    }
    else
    {
        elapsed = curTime - td_ts_slaveData[dwpid].currentIterationStartTime;
    }
    
    hw_cacheFlushAddr(&publicTestData[curTest].testTimeout);
    timeout = hw_read64(&publicTestData[curTest].testTimeout);
    
    //
    // If the test's running time hits the hard timeout
    // (either the default or the user specified one),
    // we'll kill the thread so it doesn't bring all of Bifrost down.
    // A timeout value of 0 means that timeout is disabled,
    // so don't bother checking.
    //
    if( timeout && (elapsed > timeout))
    {
        hw_errmsg("ERROR %s: test %d on thread %d, instance ID %d has exceeded the hard timeout of %llu nanoseconds, shutting that thread down.\n",
                   __func__, curTest, dwpid, td_ts_slaveData[dwpid].testData[curTest].testInstanceID, timeout);
        td_ts_slaveData[dwpid].active = HW_THREAD_TERMINATED;
        return;
    }
    
    if( elapsed >= td_ts_slaveData[dwpid].current_playnice_exceed_count * HW_TEST_PLAYNICE_TIMEOUT )
    {
        hw_status("STATUS %s: Hey...test %d on thread %d, instance ID %d is running continuously for %llu nanoseconds...\n",
                     __func__,  curTest, dwpid, td_ts_slaveData[dwpid].testData[curTest].testInstanceID,
                     td_ts_slaveData[dwpid].current_playnice_exceed_count * HW_TEST_PLAYNICE_TIMEOUT);
        td_ts_slaveData[dwpid].current_playnice_exceed_count++;
    }
#endif
}

HW_RESULT checkStates_testDriver(BOOL* bRunning, UINT32 dwpid)
{
    HW_RESULT hr = HW_S_OK;
    HW_MESSAGE outMessage;
    
    *bRunning = TRUE;
    
    if (bifrostCachedGlobals.hw_bRunInSync)
    {
        //
        // Run all tests in lock-step
        // Break into switch statement for
        // stage specific processing ...
        //
        UINT64 time = hw_getTime();
        switch (*p_td_global_state)
        {
            case HW_TS_SETUP:
                if (slavesInSync_testDriver(HW_TS_DONE_SETUP, FALSE))
                {
                    issueCmdSync_testDriver(HW_TS_INIT);
                    //TODO: system-specific functions
                    //sys_blockTest();
                    *p_td_global_state = HW_TS_INIT;
                    hw_post(TD_STATUS_CMD_INIT);
                    hw_status("Time %llu, HW_TS_INIT\n", time);
                }
                break;
            case HW_TS_INIT:
                if (slavesInSync_testDriver(HW_TS_DONE_INIT, FALSE))
                {
                    issueCmdSync_testDriver(HW_TS_RUN);
                    *p_td_global_state = HW_TS_RUN;
                    hw_post(TD_STATUS_CMD_RUN);
                    hw_status("Time %llu, HW_TS_RUN\n", time);
                }
                break;
            case HW_TS_RUN:
                // Check this testslave to see whether test timed out
                timeoutCheck_testDriver(dwpid);                
                if (slavesInSync_testDriver(HW_TS_DONE_RUN, TRUE))
                {
                    issueCmdSync_testDriver(HW_TS_CLEANUP);
                    *p_td_global_state = HW_TS_CLEANUP;
                    hw_post(TD_STATUS_CMD_CLEANUP);
                    hw_status("Time %llu, HW_TS_CLEANUP\n", time);
                }
                break;
            case HW_TS_CLEANUP:
                if(td_ts_slaveData[bifrostCachedGlobals.tpid].power_cycle_cnt < hw_read32(&hw_pTestConfigs->bfinit.POWER_MANAGER_CYC))
                {
                    if (slavesInSync_testDriver(HW_TS_REBOOT, FALSE))
                    {
                        // Reboot nodes
                        td_ts_slaveData[bifrostCachedGlobals.tpid].power_cycle_cnt++;
                        hw_status("Cycling all nodes, %d/%d times.\n", td_ts_slaveData[bifrostCachedGlobals.tpid].power_cycle_cnt, hw_read32(&hw_pTestConfigs->bfinit.POWER_MANAGER_CYC));
                        
                        UINT32 mask = 0x0;
                        for(UINT32 i = 0; i < HW_SUPERNODE_CNT; i++)
                        {
                            // TODO: abstract number of cores per node
                            UINT32 baseCoreId = i * 2;
                            UINT8 active_mask = 0;
                            UINT8 reboot_mask = 0;
                            
                            // TODO: abstract number of cores per node
                            for(UINT32 j = 0; j < 2; j++)
                            {
                                if(hw_activeCoreList[baseCoreId + j])
                                {
                                    active_mask |= 1 << j;
                                }
                                if(td_ts_slaveData[baseCoreId + j].state == HW_TS_REBOOT)
                                {
                                    reboot_mask |= 1 << j;
                                }
                            }
                            
                            // Reboot the node if at least one core is active
                            // and all active cores are in reboot state
                            if(active_mask != 0 && (active_mask & reboot_mask) == active_mask)
                            {
                                // Wait for the cores to enter final reboot-ready phase
                                for(UINT32 j = 0; j < 2; j++)
                                {
                                    if(hw_activeCoreList[baseCoreId + j])
                                    {
                                        while(*p_ts_global_states[baseCoreId + j] != HW_TS_EXIT);
                                    }
                                    *p_ts_global_states[baseCoreId + j] = HW_TS_IDLE;
                                    
                                    td_ts_slaveData[baseCoreId + j].state = HW_TS_IDLE;
                                    
                                    td_ts_slaveData[baseCoreId + j].power_cycle_cnt++;
                                }
                                
                                *p_td_global_state = HW_TS_SETUP;
                                
                                mask |= 0x1 << i;
                            }
                        }
                        
                        hw_shutdown_node(mask);
                        hw_wakeup_node(mask);
                    }
                }
                else
                {
                    if (slavesInSync_testDriver(HW_TS_EXIT, FALSE))
                    {
                        //
                        // All tests complete, break out of loop
                        //
                        *bRunning = FALSE;
                        hw_status("Tests complete\n");
                    }
                }
                break;
            // Don't do anything if the core is idling or killed
            case HW_TS_IDLE:
            case HW_TS_KILL:
                break;
            default:
                hw_errmsg("%s: core %u in unknown state %u!\n", __func__, dwpid, *p_td_global_state);
        }
    }
    else // Free-running mode
    {
        //
        // Check test slave with dwpid for a HW_TS_DONE_* step
        // If one is hit, then switch it to the next state
        //
        
        switch (td_ts_slaveData[dwpid].state)
        {
            case HW_TS_DONE_SETUP:
            case HW_TS_DONE_INIT:
            case HW_TS_DONE_RUN:
            case HW_TS_DONE_CLEANUP:
                td_ts_slaveData[dwpid].state = HW_TS_IDLE;
                break;
            case HW_TS_REBOOT:
            {
                // TODO: abstract number of cores per node
                UINT32 i = HW_SUPERNODE(dwpid);
                UINT32 baseCoreId = i * 2;
                UINT8 active_mask = 0;
                UINT8 reboot_mask = 0;
                
                // TODO: abstract number of cores per node
                for(UINT32 j = 0; j < 2; j++)
                {
                    if(hw_activeCoreList[baseCoreId + j])
                    {
                        active_mask |= 1 << j;
                    }
                    if(*p_ts_global_states[baseCoreId + j] == HW_TS_EXIT)
                    {
                        reboot_mask |= 1 << j;
                    }
                }
                
                // Reboot the node if at least one core is active
                // and all active cores are in reboot state
                if(active_mask != 0 && (active_mask & reboot_mask) == active_mask)
                {
                    // TODO: abstract number of cores per node
                    for(UINT32 j = 0; j < 2; j++)
                    {
                        *p_ts_global_states[baseCoreId + j] = HW_TS_IDLE;
                        
                        td_ts_slaveData[baseCoreId + j].state = HW_TS_IDLE;

                        td_ts_slaveData[baseCoreId + j].currentTest = HW_NO_TEST_INDEX;
                        
                        td_ts_slaveData[baseCoreId + j].power_cycle_cnt++;
                    }
                    hw_critical("Cycling node %d, %d/%d times.\n", i, td_ts_slaveData[baseCoreId].power_cycle_cnt, hw_read32(&hw_pTestConfigs->bfinit.POWER_MANAGER_CYC));
                    
                    hw_shutdown_node(0x1 << i);
                    hw_wakeup_node(0x1 << i);
                }
                break;
            }
            case HW_TS_EXIT:
                //
                // Check if all other tests are complete, if so
                // break out of the loop
                //
                if (slavesInSync_testDriver(HW_TS_EXIT, FALSE))
                {
                    *bRunning = FALSE;
                    hw_status("Tests complete\n");
                }
                break;
            case HW_TS_RUN:
                timeoutCheck_testDriver(dwpid);
            case HW_TS_IDLE:
            case HW_TS_SETUP:
            case HW_TS_INIT:
            case HW_TS_CLEANUP:
                // Test slave state is in progress
                break;
            default:
                hw_errmsg("%s: core %u in unknown state %u!\n", __func__, dwpid, td_ts_slaveData[dwpid].state);
        }
    } // bifrostCachedGlobals.hw_bRunInSync
    
    return hr;
}

//
// Test driver main loop
//
HW_RESULT run_testDriver()
{
    HW_RESULT hr = HW_S_OK;
    BOOL bRunning = TRUE;
    UINT32 dwpid = 0;
    UINT32 threadsChecked;
    
    hw_status("%s: Entering\n", __func__);
    
    if (bifrostCachedGlobals.hw_bRunInSync)
    {
        hw_status("%s: Sync mode\n", __func__);
    }
    else
    {
        hw_status("%s: Free-running mode\n", __func__);
    }
    
    *p_td_global_state = HW_TS_SETUP;
    hw_post(TD_STATUS_CMD_SETUP);
    hw_status("HW_TS_SETUP\n");
    
    // Project-specific preloop hook
    system_td_preloop();
    
    //
    // Loop around until it finds the first valid slave id
    //
    while (dwpid == bifrostCachedGlobals.tpid || td_ts_slaveData[dwpid].active != TRUE)
    {
        dwpid++;
        dwpid %= HW_PROC_CNT;
    }
    
    while (bRunning && (!hw_bTerminateAllTests))
    {
        if(HW_E_UNSUPPORTED_COMMAND == (hr = HR(processMailbox_testDriver(dwpid))))
        {
            break;
        }

        while(td_inSuspensionMode)
        {
            arch_suspend();
        }
        
        if(HW_E_TIMEOUT == (hr = HR(checkStates_testDriver(&bRunning, dwpid))))
        {
            break;
        }
        
        // Project-specific periodic hook
        system_td_periodic();
        
        threadsChecked = 0;
        
        //
        // Find the next slave
        //
        do
        {
            dwpid++;
            dwpid %= HW_PROC_CNT;
            threadsChecked++;
            // If no threads are active
            if(threadsChecked > HW_PROC_CNT)
            {
                hw_critical("%s: All cores completed execution.\n", __func__);
                bRunning = FALSE;
                break;
            }
        } while (dwpid == bifrostCachedGlobals.tpid || td_ts_slaveData[dwpid].active != TRUE);
    }
    
    // Project-specific postloop hook
    system_td_postloop();
    
    hw_post(TD_STATUS_RUN_DONE);
    
    hw_status("%s: Exiting\n", __func__);
    
    return hr;
}

//
// Test driver main loop
//
HW_RESULT cleanup_testDriver()
{
    HW_RESULT hr = HW_S_OK;

    return hr;
}

//
// Main entry point into the test driver
//
extern void core_cleanup();
extern void initGlobals();

VOID testDriver(void* _dwpid)
{
    initGlobals();
    hw_bIntGlobalInitFinished = TRUE;

    UINT32 dwpid = (UINT32)_dwpid;
    HW_RESULT hr = HW_S_OK;
    UINT32 ctr;
    td_testDriverGlobalPass = TRUE;

    hw_status("%s%u: Entering\n", __func__, dwpid);

    hr = HR(init_testDriver());
    
    // If cores other than the testdriver are active, then
    // begin the slave management state machine.
    if (hr == HW_S_OK && (td_ts_active_cnt > 1 || !td_ts_slaveData[dwpid].active))
    {
        hr = HR(run_testDriver());
    }

    // Always clean up, leave previous return codes intact.
    cleanup_testDriver();

    //
    // Final test results
    //
    for(ctr = 0; ctr < HW_PROC_CNT; ctr++)
    {
        if(td_ts_slaveData[ctr].active)
        {
            hw_critical("Test slave %d:\n %d out of %d tests passed.\n", ctr, td_ts_slaveData[ctr].testsPassed, td_ts_slaveData[ctr].testCnt);
            if(td_ts_slaveData[ctr].testsPassed != td_ts_slaveData[ctr].testCnt)
            {
                td_testDriverGlobalPass = FALSE;
            }
        }
    }

    //
    // Check testdriver status
    //
    if(hr != HW_S_OK)
    {
        td_testDriverGlobalPass = FALSE;
    }

    td_print_all_event_logs();

#ifndef BASRAM_ONLY
    //TODO: system-specific functions
    //if(hw_is_silicon() && sys_need_dram_heap_init())
    //{
    //    td_checkUnusedHeapSpace(HW_HEAP_TYPE_DRAM);
    //}
#endif

    if(td_testDriverGlobalPass)
    {
        hw_critical("Success. All tests passed.\n");
    }
    else
    {
        hw_critical("Failure. Not all tests passed.\n");
    }

    //TODO: system-specific functions
    //if(td_hasStandbyRequest)
    //{
    //    hw_critical("System going to standby \n");
    //    sys_standby();
    //}
    hw_critical("%s%u: Exiting\n", __func__, dwpid);
    core_cleanup();
}
