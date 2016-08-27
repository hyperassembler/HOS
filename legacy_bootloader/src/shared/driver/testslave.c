/*-------------------------------------------------------
 |
 |     testslave.c
 |
 |     Manages tests and Bifrost API on a single core.
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
#include <bifrost_mem.h>
#include "bifrost_private.h"

//
// Avoid using globals for the test slaves, since there could be many slaves.  If you have to
// create an array with the size HW_PROC_CNT, so that there's no thrashing between processes
//
BOOL ts_isWarmReboot[HW_PROC_CNT];

HW_RESULT idle_testSlave(HW_BIFROST_STATE targetState)
{
    HW_RESULT hr = HW_S_OK;
    BOOL idle = TRUE;
    
    // Save local (cached) copy of global state
    // pointers so we don't have to read them
    // every time we poll.
    volatile HW_BIFROST_STATE* p_td_global_state_local = p_td_global_state;
    volatile HW_BIFROST_STATE* p_ts_global_state_local = p_ts_global_states[hw_getCoreNum()];
    
    //
    // Skip the idle step if this is not synchronized mode,
    // or this code is running on the driver core.
    //
    if(hw_getCoreNum() != bifrostCachedGlobals.tpid && bifrostCachedGlobals.hw_bRunInSync)
    {
        // Loop until the slave global state matches the target condition,
        // and then loop again until the driver global state also matches.
        while (idle)
        {
            HW_BIFROST_STATE state = *p_ts_global_state_local;
            if(state == targetState)
            {
                idle = FALSE;
                
                switch(targetState)
                {
                case HW_TS_INIT:
                    while(*p_td_global_state_local != HW_TS_INIT);
                    break;
                case HW_TS_RUN:
                    while(*p_td_global_state_local != HW_TS_RUN);
                    break;
                case HW_TS_CLEANUP:
                    while(*p_td_global_state_local != HW_TS_CLEANUP);
                    break;
                default:
                    hw_errmsg("%s: invalid state %d!\n", __func__, targetState);
                    hr = HW_E_OTHER;
                }
            }
            else if(state == HW_TS_KILL)
            {
                return HW_E_TERMINATE;
            }
        }
    }
    
    return hr;
}

HW_RESULT setup_testSlave(UINT32 dwpid)
{
    HW_MESSAGE message = {};
    HW_RESULT hr = HW_S_OK;
    UINT32 testIdx = 0;
    UINT32 coreNum = 0;
    UINT32 testIndex = 0;
    HW_TESTXENTRY* tsx = NULL;
    HW_PATHID testPathID = 0x0;
    
    HW_TS_TESTDATA* testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);
    HW_TS_PUBLIC_TESTDATA* publicTestData = (HW_TS_PUBLIC_TESTDATA *)hw_readptr(&testSlaveVars->publicTestData);
    
    //
    // Acknowledge test slave setup
    //
    if(dwpid != bifrostCachedGlobals.tpid && bifrostCachedGlobals.hw_bRunInSync)
    {
        message.metadata.command = MSG_TS_ACK_TEST_SETUP;
        message.metadata.size = 0;
        placeMessage(&testSlaveVars->driverMailbox, &message);
    }
    
    //
    // Initialize private test slave variable struct
    //
    hw_write32(&testSlaveVars->dwTestCleanupCnt, 0);
    
    // Set up internal print buffer. Only required if
    // this core is not the testdriver, and if prints
    // are not being routed through the testdriver.
    VOID* internalPrintBuffer = (VOID*)hw_readptr(&testSlaveVars->internalPrintBuffer);
    if(!ts_isWarmReboot[dwpid] && dwpid != bifrostCachedGlobals.tpid && hw_pTestConfigs->bfinit.DIRECT_PRINT_EN)
    {
        internalPrintBuffer = hw_alloc(HW_PRINTF_BUF_SIZE);
        hw_writeptr(&testSlaveVars->internalPrintBuffer, (UINTPTR)internalPrintBuffer);
    }
    
    // Set up test timeout--if 'runtime' argument is non zero, set
    // timeout to 2 times that. Otherwise, use the default
    // hardcoded value.
    UINT64 testTimeout = hw_read64(&hw_pTestConfigs->runtime) * 2;
    if(testTimeout == 0ull || testTimeout < HW_TEST_HARD_TIMEOUT)
    {
        testTimeout = HW_TEST_HARD_TIMEOUT;
    }
    
    //
    // Figure out which tests this slave is supposed to run.
    //
    tsx = hw_pTestConfigs->entryList;
    
    coreNum = hw_read32(&tsx->coreNumber);
    while (coreNum != HW_XLIST_TERMINATION)
    {
        if ( coreNum == dwpid )
        {
            if (testIndex < HW_TS_MAX_TESTS)
            {
                // Find test functions and parameters
                testPathID = hw_read64(&tsx->testPathID);
                testIdx = ts_getFuncIndexFromPathID(testPathID);

                //hw_thread_create(&testData[testIdx].testStackPtr);
                UINT32 tid;
                //hw_thread_create(ptr, args, PRIORITY_DEFAULT, THREAD_DEFAULT_STACK_SIZE, &tid);

                if(!ts_isWarmReboot[dwpid])
                {
                    UINT8 *testName = (UINT8*)hw_tl_master[hw_archTypeList[dwpid]][testIdx].testPathName;
                    UINT32 testIDHash = hw_tl_master[hw_archTypeList[dwpid]][testIdx].testPathHash;
                    hw_critical("Loading test: ID is %d, source path: \"%s\", hash: 0x%x.\n", hw_read32(&tsx->testInstanceID), testName, testIDHash);
                }
                else
                {
                    hw_write32(&testData[testIndex].dwIteration, 0);
                }
                
                hw_write32(&testData[testIndex].yielded, FALSE);
                hw_write32(&testData[testIndex].testBufferIndex, hw_read32(&tsx->localInstanceIdx));
                hw_write32(&testData[testIndex].testGlobalIndex, hw_read32(&tsx->globalInstanceIdx));
                
                //
                // Get parameters to pass to test
                //
                hw_write32(&testData[testIndex].paramCount, hw_read32(&tsx->paramCount));
                hw_writeptr(&testData[testIndex].paramPtr, (UINTPTR)hw_pTestConfigs + hw_read32(&tsx->paramOffset));
                
                // Set test status to RUN_ELIGIBLE if and only if:
                // -This is the first boot (not a warm reboot)
                // -The test did not fail on a previous boot
                // -The test did not ask not to be rebooted
                // If one of these conditions fails, then also
                // set SKIP_CLEANUP and SKIP_REBOOT.
                hw_cacheInvalidateAddr(&publicTestData[testIndex].testStatus);
                if(!ts_isWarmReboot[dwpid] || (hw_read32((UINT32*)&publicTestData[testIndex].testStatus) != HW_TEST_FAIL && ((hw_read32(&testData[testIndex].runEndConditions) & HW_TS_SKIP_REBOOT) == 0)))
                {
                    hw_write32((UINT32*)&publicTestData[testIndex].testStatus, HW_TEST_RUN_ELIGIBLE);
                    hw_cacheFlushAddr(&publicTestData[testIndex].testStatus);
                }
                else
                {
                    hw_write32(&testData[testIndex].runEndConditions, HW_TS_SKIP_CLEANUP | HW_TS_SKIP_REBOOT);
                }
                hw_write32(&publicTestData[testIndex].testPointerIndex, testIdx);
                hw_write32(&publicTestData[testIndex].testInstanceID, hw_read32(&tsx->testInstanceID));
                hw_write64(&publicTestData[testIndex].testTimeout, testTimeout);
                hw_cacheFlushAddr(&publicTestData[testIndex].testPointerIndex);
                hw_cacheFlushAddr(&publicTestData[testIndex].testInstanceID);
                hw_cacheFlushAddr(&publicTestData[testIndex].testTimeout);
                
                // Set up test verbosity
                hw_write32((UINT32*)&testData[testIndex].verbosity, hw_read32(&hw_pTestConfigs->verbosity));
                
                testIndex++;
            }
            else
            {
                hw_errmsg("%s: Too many tests assigned to this slave.\n", __func__);
                hr = HW_E_OUT_OF_BOUNDS;
            }
        }
        tsx++;
        coreNum = hw_read32(&tsx->coreNumber);
    } 
    hw_write32(&testSlaveVars->dwTestCnt, testIndex);
    
    if(testIndex != 0)
    {
        // Set up space for dynamically allocated data:
        // -An alloc tracker for the core
        // -A kernel stack for each test, followed by a mailbox for each test
        
        // Set up individual test stack, mailbox,
        // and alloc record pointers
        for(UINT32 i = 0; i < testIndex; i++)
        {
            UINT32* headPtr = &publicTestData[i].head;
            UINT32* tailPtr = &publicTestData[i].tail;
            
            hw_writeptr(&publicTestData[i].mailbox.headPtr, (UINTPTR)hw_uncached((VOID*)headPtr));
            hw_writeptr(&publicTestData[i].mailbox.tailPtr, (UINTPTR)hw_uncached((VOID*)tailPtr));
            hw_cacheFlushAddr(&publicTestData[i].mailbox.headPtr);
            hw_cacheFlushAddr(&publicTestData[i].mailbox.tailPtr);
        }
    }
    
    //
    // Flush all the changes we just wrote to memory so that
    // testdriver has access to them.
    //
    hw_cacheFlushBuffer(&testSlaveVars->dwTestCnt, offsetof(HW_TESTSLAVE_VARS, testData) - offsetof(HW_TESTSLAVE_VARS, dwTestCnt));

    // Set 'warm reboot' flag, so that if this core
    // is rebooted and starts from the beginning,
    // it will know when it checks this flag.
    ts_isWarmReboot[dwpid] = TRUE;
    
    //
    // Send completion message
    //
    if(dwpid != bifrostCachedGlobals.tpid && bifrostCachedGlobals.hw_bRunInSync)
    {
        message.metadata.command = MSG_TS_DONE_TEST_SETUP;
        placeMessage(&testSlaveVars->driverMailbox, &message);
    }

    return hr;
}

HW_RESULT init_testSlave()
{
    HW_RESULT hr = HW_S_OK;
    HW_MESSAGE message;
    UINT32 dwpid = hw_getCoreNum();
    UINT32 i = 0;
    
    const HW_XLIST* tlp = NULL;
    HW_TESTXENTRY* tConfig = hw_pTestConfigs->entryList;
    UINT32 testCnt = 0;
    UINT32 testPointerIndex = 0;
    
    HW_TS_TESTDATA* testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);
    HW_TS_PUBLIC_TESTDATA* publicTestData = (HW_TS_PUBLIC_TESTDATA*)hw_readptr(&testSlaveVars->publicTestData);
    
    //
    // Acknowledge test slave initialization
    //
    if(dwpid != bifrostCachedGlobals.tpid && bifrostCachedGlobals.hw_bRunInSync)
    {
        message.metadata.command = MSG_TS_ACK_INIT;
        message.metadata.size = 0;
        placeMessage(&testSlaveVars->driverMailbox, &message);
    }
    
    hw_post(TS_STATUS_ACK_INIT);
    
    //
    // Run the test init routine for all tests assigned to this test slave
    //
    testCnt = hw_read32(&testSlaveVars->dwTestCnt);
    for (i = 0; i < testCnt; i++)
    {
        //
        // Check for test completion
        //
        hw_cacheInvalidateAddr(&publicTestData[i].testStatus);
        UINT32 testStatus = hw_read32((UINT32*)&publicTestData[i].testStatus);
        
        //
        // Tests that have called hw_done will have their testStatus
        // set so that they will not enter this block. 
        //
        if (testStatus < HW_TEST_END)
        {
            testPointerIndex = hw_read32(&publicTestData[i].testPointerIndex);
            tlp = &(hw_tl_master[hw_archTypeList[hw_getCoreNum()]][testPointerIndex]);
            
            ts_set_test_index( i );
            hw_log(HW_EVENT_CHECKPOINT, (UINTPTR)"init", (UINTPTR)"enter", 0, 0);
            ts_arch_enter_init_cleanup_test(tlp->pinit, i); // Arch specific to handle context switching
            hw_log(HW_EVENT_CHECKPOINT, (UINTPTR)"init", (UINTPTR)"exit", 0, 0);
            
            ts_set_test_index(HW_NO_TEST_INDEX);
        }
    }
    
    //
    // Send completion message
    //
    if(dwpid != bifrostCachedGlobals.tpid && bifrostCachedGlobals.hw_bRunInSync)
    {
        message.metadata.command = MSG_TS_DONE_INIT;
        placeMessage(&testSlaveVars->driverMailbox, &message);
    }
    
    hw_post(TS_STATUS_DONE_INIT);
    
    return hr;
}

HW_RESULT cleanup_testSlave()
{
    HW_RESULT hr = HW_S_OK;
    HW_MESSAGE message;
    UINT32 dwpid = hw_getCoreNum();
    
    UINT32 i = 0;
    
    const HW_XLIST* tlp = NULL;
    UINT32 testCnt = hw_read32(&testSlaveVars->dwTestCnt);
    UINT32 testIdx = 0;
    
    HW_TS_TESTDATA* testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);
    HW_TS_PUBLIC_TESTDATA* publicTestData = (HW_TS_PUBLIC_TESTDATA*)hw_readptr(&testSlaveVars->publicTestData);
    
    //
    // Acknowledge start of cleanup
    //
    if(dwpid != bifrostCachedGlobals.tpid && bifrostCachedGlobals.hw_bRunInSync)
    {
        message.metadata.command = MSG_TS_ACK_CLEANUP;
        message.metadata.size = 0;
        placeMessage(&testSlaveVars->driverMailbox, &message);
    }
    
    hw_post(TS_STATUS_ACK_CLEANUP);
    
    //
    // Perform actual clean-up processing here
    //
    for (i = 0; i < testCnt; i++)
    {
        if((hw_read32(&testData[i].runEndConditions) & HW_TS_SKIP_CLEANUP) == 0)
        {
            ts_set_test_index( i );
            testIdx = hw_read32(&publicTestData[i].testPointerIndex);
            tlp = &(hw_tl_master[hw_archTypeList[hw_getCoreNum()]][testIdx]);
            
            hw_log(HW_EVENT_CHECKPOINT, (UINTPTR)"cleanup", (UINTPTR)"enter", 0, 0);
            ts_arch_enter_init_cleanup_test(tlp->pcleanup, i); // Arch specific to handle context switching
            hw_log(HW_EVENT_CHECKPOINT, (UINTPTR)"cleanup", (UINTPTR)"exit", 0, 0);
            ts_set_test_index(HW_NO_TEST_INDEX);
        }
    }
    
    //
    // Send completion message
    //
    if(dwpid != bifrostCachedGlobals.tpid && bifrostCachedGlobals.hw_bRunInSync)
    {
        message.metadata.command = MSG_TS_DONE_CLEANUP;
        placeMessage(&testSlaveVars->driverMailbox, &message);
    }
    
    hw_post(TS_STATUS_DONE_CLEANUP);
    
    return hr;
}

extern void core_cleanup();

HW_RESULT testSlave(void* _dwpid)
{
    UINT32 tid;
    while (!hw_bIntGlobalInitFinished);
    UINT32 dwpid = (UINT32)_dwpid;
    HW_RESULT hr = HW_S_OK;

    //
    // Check for cacheline alignment of
    // system structs.
    //
    assert((UINTPTR) testSlaveVars % HW_CACHELINE_SIZE == 0);
    assert((UINTPTR) &testSlaveVars->driverMailbox % HW_CACHELINE_SIZE == 0);
    assert((UINTPTR) &testSlaveVars->slaveMailbox % HW_CACHELINE_SIZE == 0);
    assert((UINTPTR) &testSlaveVars->printBuffer % HW_CACHELINE_SIZE == 0);


    //
    // Report complete testslave exit, along with number
    // of tests passed. After this, testdriver will
    // no longer process any mailbox messages.
    //
    HW_TS_PUBLIC_TESTDATA* publicTestData = (HW_TS_PUBLIC_TESTDATA*)hw_readptr(&testSlaveVars->publicTestData);
    UINT32 testsPassed = 0;
    for (UINT32 i = 0; i < hw_read32(&testSlaveVars->dwTestCnt); i++)
    {
        hw_cacheInvalidateAddr(&publicTestData[i].testStatus);
        if (hw_read32((UINT32*)&publicTestData[i].testStatus) == HW_TEST_PASS)
        {
            testsPassed++;
        }
    }

    if(dwpid != bifrostCachedGlobals.tpid)
    {
        hw_flushMailbox();

        HW_MESSAGE message;
        message.metadata.command = MSG_TS_EXIT;
        message.metadata.size = sizeof(UINT32);
        *(UINT32*) message.data = testsPassed;
        placeMessage(&testSlaveVars->driverMailbox, &message);
    }
    else
    {
        td_ts_slaveData[dwpid].testsPassed = testsPassed;
    }

    hw_post(TS_STATUS_EXIT);

    // Flush dynamic test data from the cache so that in case
    // of a warm reboot, the data doesn't just disappear--
    // this should be done AFTER sending the 'HW_TS_EXIT'
    // message so that in case all tests are done (meaning
    // no more reboots will happen), the testdriver doesn't
    // wait for this to finish before signaling end of run,
    // but BEFORE setting the global state to HW_TS_EXIT,
    // which would tell the testdriver to power off this core
    hw_cacheFlushBuffer((void*)hw_readptr(&testSlaveVars->allocTracker), sizeof(HW_ALLOC_TRACKER));

    *p_ts_global_states[dwpid] = HW_TS_EXIT;

    return 0;
}

HW_RESULT testslave_body()
{
    HW_RESULT hr = HW_S_OK;
    
    UINT32 dwpid = hw_getCoreNum();
    
    //
    // We need to check return codes and establish a system for when to
    // bail out (e.g. if init_testSlave succeeds, we should probably call
    // cleanup_testSlave no matter what happens in between). See TFS
    // task 17042.
    //
    //hr = HR(setup_testSlave(dwpid));
    //
    //if(hw_read32(&testSlaveVars->dwTestCnt) != 0)
    //{
    //    hr = HR(idle_testSlave(HW_TS_INIT));
    //    hr = HR(init_testSlave());
    //    
    //    hr = HR(idle_testSlave(HW_TS_RUN));
    //    hr = HR(run_testSlave());
    //    
    //    //
    //    // If no tests need to cleanup, skip cleanup phase.
    //    //
    //    if(hw_read32(&testSlaveVars->dwTestCleanupCnt) != 0)
    //    {
    //        hr = HR(idle_testSlave(HW_TS_CLEANUP));
    //        hr = HR(cleanup_testSlave());
    //    }
    //}
    
    return hr;
}

