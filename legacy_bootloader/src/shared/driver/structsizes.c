/*-------------------------------------------------------
 |
 |     structsizes.c
 |
 |     At the start of execution, performs a check to
 |     ensure that all cores have the same view of
 |     Bifrost's struct setup.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2011  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#include "bifrost_private.h"
#include <bifrost_mem.h>

#define STRUCT_PRINT(struct_name) (hw_verbose(#struct_name ": %u %u\n", struct_name ## _SIZE, sizeof(struct_name)))

void printStructSizes()
{
    hw_verbose("System memory locations:\n");
    hw_verbose("Config base: 0x%x\n", HW_SYS_CONFIG_BASE + BIFROST_MEM_BASE );
    hw_verbose("Testslave var base: 0x%x\n", HW_SYS_TSVAR_BASE + BIFROST_MEM_BASE );
    hw_verbose("System comm base: 0x%x\n", HW_SYS_SYSCOM_BASE + BIFROST_MEM_BASE );
    hw_verbose("End of Bifrost addr. space: 0x%x\n\n", HW_SYS_BIFROST_END + BIFROST_MEM_BASE );
    
    hw_verbose("System struct sizes:\n");
    STRUCT_PRINT(HW_TESTXENTRY);
    STRUCT_PRINT(HW_TESTCFGS);
    STRUCT_PRINT(HW_PRINTF_BUFFER);
    STRUCT_PRINT(HW_XLIST);
    STRUCT_PRINT(HW_INT_CONTEXT);
    STRUCT_PRINT(HW_TESTSLAVE_INT);
    STRUCT_PRINT(HW_TS_TESTDATA);
    STRUCT_PRINT(HW_TESTSLAVE_VARS);
    STRUCT_PRINT(HW_MSG_METADATA);
    STRUCT_PRINT(HW_MESSAGE);
    STRUCT_PRINT(HW_MAILBOX);
    
    hw_verbose("System struct offsets:\n");
    hw_verbose("HW_PRINTF_BUFFER: head: %u tail: %u buffer: %u\n",
               offsetof(HW_PRINTF_BUFFER, head), offsetof(HW_PRINTF_BUFFER, tail), offsetof(HW_PRINTF_BUFFER, buffer));
    
    hw_verbose("HW_MAILBOX: headPtr: %u tailPtr: %u messages: %u\n",
               offsetof(HW_MAILBOX, headPtr), offsetof(HW_MAILBOX, tailPtr), offsetof(HW_MAILBOX, messages));
    
    hw_verbose("HW_TESTSLAVE_VARS: slaveMailbox: %u driverMailbox: %u printBuffer: %u dwTestCnt: %u interrupts: %u\n",
               offsetof(HW_TESTSLAVE_VARS, slaveMailbox), offsetof(HW_TESTSLAVE_VARS, driverMailbox), offsetof(HW_TESTSLAVE_VARS, printBuffer),
               offsetof(HW_TESTSLAVE_VARS, dwTestCnt),
               offsetof(HW_TESTSLAVE_VARS, interrupts));
    
    hw_verbose("HW_TS_TESTDATA: dwIteration: %u privateVars: %u\n",
               offsetof(HW_TS_TESTDATA, dwIteration), offsetof(HW_TS_TESTDATA, privateVars));
}

void checkStructSizes()
{
    //
    // Visual output of struct sizes, only done
    // by the testdriver.
    //
    if(hw_getCoreNum() == bifrostCachedGlobals.tpid)
    {
        // Uncomment this code in the rare case
        // that struct sizes need to be printed
    #if 0
        printStructSizes();
    #endif
    }
    
    hw_assert(HW_MAX_CACHELINE_SIZE >= HW_CACHELINE_SIZE);
    hw_assert(HW_MAX_CACHELINE_SIZE % HW_CACHELINE_SIZE == 0);
    
    static_assert(sizeof(FLOAT32) == 4);
    
    static_assert(HW_TESTXENTRY_SIZE == sizeof(HW_TESTXENTRY));
    static_assert(HW_TESTCFGS_SIZE == sizeof(HW_TESTCFGS));
    static_assert(HW_PRINTF_BUFFER_SIZE == sizeof(HW_PRINTF_BUFFER));
    static_assert(HW_XLIST_SIZE == sizeof(HW_XLIST));
    static_assert(HW_INT_CONTEXT_SIZE == sizeof(HW_INT_CONTEXT));
    static_assert(HW_TESTSLAVE_INT_SIZE == sizeof(HW_TESTSLAVE_INT));
    static_assert(HW_TS_TESTDATA_SIZE == sizeof(HW_TS_TESTDATA));
    static_assert(HW_TESTSLAVE_VARS_SIZE == sizeof(HW_TESTSLAVE_VARS));
    static_assert(HW_MSG_METADATA_SIZE == sizeof(HW_MSG_METADATA));
    static_assert(HW_MESSAGE_SIZE == sizeof(HW_MESSAGE));
    static_assert(HW_MAILBOX_SIZE == sizeof(HW_MAILBOX));
    static_assert(HW_SYSCOM_SIZE == sizeof(HW_SYSCOM));
    static_assert(HW_EVENT_LOG_ENTRY_SIZE == sizeof(HW_EVENT_LOG_ENTRY));
    static_assert(HW_EVENT_LOG_SIZE == sizeof(HW_EVENT_LOG));

    static_assert(offsetof(HW_TESTXENTRY, testPathID) == 0);
    static_assert(offsetof(HW_TESTXENTRY, testInstanceID) == sizeof(HW_PATHID));
    static_assert(offsetof(HW_TESTXENTRY, globalInstanceIdx) == sizeof(HW_PATHID) + sizeof(UINT32));
    static_assert(offsetof(HW_TESTXENTRY, localInstanceIdx) == sizeof(HW_PATHID) + (2 * sizeof(UINT32)));
    static_assert(offsetof(HW_TESTXENTRY, coreNumber) == sizeof(HW_PATHID) + (3 * sizeof(UINT32)));
    static_assert(offsetof(HW_TESTXENTRY, threadPriority) == sizeof(HW_PATHID) + (4 * sizeof(UINT32)));
    static_assert(offsetof(HW_TESTXENTRY, paramOffset) == sizeof(HW_PATHID) + (5 * sizeof(UINT32)));
    static_assert(offsetof(HW_TESTXENTRY, paramCount) == sizeof(HW_PATHID) + (6 * sizeof(UINT32)));
    
    static_assert(offsetof(HW_PRINTF_BUFFER, head) == 0);
    static_assert(offsetof(HW_PRINTF_BUFFER, tail) == HW_MAX_CACHELINE_SIZE);
    static_assert(offsetof(HW_PRINTF_BUFFER, buffer) == (2 * HW_MAX_CACHELINE_SIZE));
  
    static_assert(offsetof(HW_MSG_METADATA, from) == 0);
    static_assert(offsetof(HW_MSG_METADATA, to) == sizeof(HW_TESTID));
    static_assert(offsetof(HW_MSG_METADATA, command) == (2 * sizeof(HW_TESTID)));
   
    static_assert(offsetof(HW_MESSAGE, metadata) == 0);
    static_assert(offsetof(HW_MESSAGE, data) == sizeof(HW_MSG_METADATA));
    
    static_assert(offsetof(HW_TESTSLAVE_VARS, driverMailbox) == 0);
    static_assert(offsetof(HW_TESTSLAVE_VARS, driverLowPrioMailbox) == sizeof(HW_MAILBOX));
    static_assert(offsetof(HW_TESTSLAVE_VARS, slaveMailbox) == (2 * sizeof(HW_MAILBOX)));
    static_assert(offsetof(HW_TESTSLAVE_VARS, slaveLowPrioMailbox) == (3 * sizeof(HW_MAILBOX)));
    static_assert(offsetof(HW_TESTSLAVE_VARS, printBuffer) == (4 * sizeof(HW_MAILBOX)));
    static_assert(offsetof(HW_TESTSLAVE_VARS, interrupts) == ((4 * sizeof(HW_MAILBOX)) + sizeof(HW_PRINTF_BUFFER)));
    static_assert(offsetof(HW_TESTSLAVE_VARS, exceptions) == ((4 * sizeof(HW_MAILBOX)) + sizeof(HW_PRINTF_BUFFER) + sizeof(HW_TESTSLAVE_INT)));
    static_assert(offsetof(HW_TESTSLAVE_VARS, polls) == ((4 * sizeof(HW_MAILBOX)) + sizeof(HW_PRINTF_BUFFER) + sizeof(HW_TESTSLAVE_INT) + sizeof(HW_TESTSLAVE_EXC)));
    static_assert(offsetof(HW_TESTSLAVE_VARS, dwTestCnt) == ((4 * sizeof(HW_MAILBOX)) + sizeof(HW_PRINTF_BUFFER) + sizeof(HW_TESTSLAVE_INT) + sizeof(HW_TESTSLAVE_EXC) + sizeof(HW_TESTSLAVE_POLL)));
    static_assert(offsetof(HW_TESTSLAVE_VARS, dwTestCleanupCnt) == ((4 * sizeof(HW_MAILBOX)) + sizeof(HW_PRINTF_BUFFER) + sizeof(HW_TESTSLAVE_INT) + sizeof(HW_TESTSLAVE_EXC) + sizeof(HW_TESTSLAVE_POLL) + sizeof(UINT32)));
    static_assert(offsetof(HW_TESTSLAVE_VARS, pollIntTimeout) == ((4 * sizeof(HW_MAILBOX)) + sizeof(HW_PRINTF_BUFFER) + sizeof(HW_TESTSLAVE_INT) + sizeof(HW_TESTSLAVE_EXC) + sizeof(HW_TESTSLAVE_POLL) + (2 * sizeof(UINT32))));
    static_assert(offsetof(HW_TESTSLAVE_VARS, sharedDynamicCoreData) == ((4 * sizeof(HW_MAILBOX)) + sizeof(HW_PRINTF_BUFFER) + sizeof(HW_TESTSLAVE_INT) + sizeof(HW_TESTSLAVE_EXC) + sizeof(HW_TESTSLAVE_POLL) + (2 * sizeof(UINT32)) + sizeof(UINT64)));
    static_assert(offsetof(HW_TESTSLAVE_VARS, allocTracker) == ((4 * sizeof(HW_MAILBOX)) + sizeof(HW_PRINTF_BUFFER) + sizeof(HW_TESTSLAVE_INT) + sizeof(HW_TESTSLAVE_EXC) + sizeof(HW_TESTSLAVE_POLL) + (2 * sizeof(UINT32)) + sizeof(UINT64) + sizeof(PTR_FIELD)));
    static_assert(offsetof(HW_TESTSLAVE_VARS, eventLog) == ((4 * sizeof(HW_MAILBOX)) + sizeof(HW_PRINTF_BUFFER) + sizeof(HW_TESTSLAVE_INT) + sizeof(HW_TESTSLAVE_EXC) + sizeof(HW_TESTSLAVE_POLL) + (2 * sizeof(UINT32)) + sizeof(UINT64) + (2 * sizeof(PTR_FIELD))));
    static_assert(offsetof(HW_TESTSLAVE_VARS, internalPrintBuffer) == ((4 * sizeof(HW_MAILBOX)) + sizeof(HW_PRINTF_BUFFER) + sizeof(HW_TESTSLAVE_INT) + sizeof(HW_TESTSLAVE_EXC) + sizeof(HW_TESTSLAVE_POLL) + (2 * sizeof(UINT32)) + sizeof(UINT64) + (3 * sizeof(PTR_FIELD))));
    
    static_assert(offsetof(HW_EVENT_LOG_ENTRY, code) == (0));
    static_assert(offsetof(HW_EVENT_LOG_ENTRY, time) == (sizeof(UINT64)));
    static_assert(offsetof(HW_EVENT_LOG_ENTRY, data) == (2 * sizeof(UINT64)));

    static_assert(offsetof(HW_EVENT_LOG, entries) == (0));
    static_assert(offsetof(HW_EVENT_LOG, size) == (sizeof(HW_EVENT_LOG_ENTRY) * HW_MAX_EVENT_LOG_ENTRIES));

    static_assert(offsetof(HW_TESTCFGS, tpid) == (sizeof(UINT32)));
    static_assert(offsetof(HW_TESTCFGS, entryList) == ((7 * sizeof(UINT32)) + (28 * sizeof(const char)) + sizeof(UINT64) + sizeof(HW_BFINIT) + sizeof(HW_BFCOMP)));
}

