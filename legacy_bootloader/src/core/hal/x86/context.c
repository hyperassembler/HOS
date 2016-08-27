/*-------------------------------------------------------
 |
 |    context.c
 |
 |    Thread switching functions for 'x86' architecture.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2013  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#include "bifrost_private.h"

EXTERN_C UINT32 ts_arch_startNextIteration_ASM(VOID* kernelRegs, VOID* testStackPtr, HW_RUN_FUNC* prunTest, UINT32 iteration);
EXTERN_C UINT32 ts_arch_runIdleThread_ASM(VOID* kernelRegs, VOID (*prunTest)());
EXTERN_C UINT32 ts_arch_enter_init_cleanup_ASM(VOID* kernelRegs, VOID* testStackPtr, HW_FUNC* prunTest);
EXTERN_C UINT32 ts_arch_resume_test_ASM(VOID* curTestRegs, VOID *targetTestRegs);
EXTERN_C VOID ts_arch_resume_kernel_ASM(VOID* testRegs, VOID *kernelRegs);

UINT32 ts_arch_enter_init_cleanup_test(HW_FUNC* prunTest, UINT32 testIdx)
{
    // Get the test's stack pointer
    HW_TS_TESTDATA* testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);
    VOID *testStackPtr = (VOID*)hw_readptr(&testData[testIdx].testStackPtr);   

    // Transfer control
    ts_arch_enter_init_cleanup_ASM(&testSlaveVars->kernelRegs, testStackPtr, prunTest);

    return 0; // We never actually hit this return, so the 0 is kind of bogus.  Return is done from the _ASM
}

UINT32 ts_arch_startNextIteration(HW_RUN_FUNC* prunTest, UINT32 iteration, UINT32 testIdx, VOID *curTestRegs)
{
    // Get the test's stack pointer
    HW_TS_TESTDATA* testData = (HW_TS_TESTDATA*)hw_readptr(&testSlaveVars->testData);
    VOID *testStackPtr = (VOID*)hw_readptr(&testData[testIdx].testStackPtr);   

    //Enable preemptive tasking timer
    if(hw_pTestConfigs->bfinit.PREEMPTION_ON)
    {
        arch_int_startPreemptionTimer();
    }
    
    hw_int_enable(1 << 0);
    
    // Transfer control
    hw_errmsg( "%s: Error: ts_arch_startNextIteration was run and is not yet implemented.\n", __func__ );
    ts_arch_startNextIteration_ASM(curTestRegs, testStackPtr, prunTest, iteration);

    return 0; // We never actually hit this return, so the 0 is kind of bogus.  Return is done from the _ASM
}

UINT32 ts_arch_runIdleThread(VOID (*prunTest)(), VOID *curTestRegs)
{
    // Transfer control
    hw_errmsg( "%s: Error: ts_arch_runIdleThread was run and is not yet implemented.\n", __func__ );
    ts_arch_runIdleThread_ASM(curTestRegs, prunTest);

    return 0; 
}

UINT32 ts_arch_resume_test(VOID* curTestRegs, VOID *targetTestRegs)
{
    ts_arch_resume_test_ASM(targetTestRegs, curTestRegs);

    return 0; // We never actually hit this return, so the 0 is kind of bogus.  Return is done from the _ASM
}

VOID ts_arch_resume_kernel(VOID* testRegs, VOID *kernelRegs)
{
    //Disable preemptive tasking timer
    if(hw_pTestConfigs->bfinit.PREEMPTION_ON)
    {
         arch_int_stopPreemptionTimer();
    }

    // Transfer control to assembly handler
    ts_arch_resume_kernel_ASM(testRegs, kernelRegs);
}

