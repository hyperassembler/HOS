/*-------------------------------------------------------
 |
 |      bifrost_driver.h
 |
 |      Contains driver-only functions and definitions.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2011-2014  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
*/

#ifndef _BIFROST_DRIVER_H
#define _BIFROST_DRIVER_H

//
// Constants, struct definitions,
// and variable declarations
//
#define HW_THREAD_TERMINATED (0xDB)

#ifndef PERFMON_COLLECT_INTERVAL
#define PERFMON_COLLECT_INTERVAL     10000000ull
#endif 

#ifndef PERFMON_REPORT_INTERVAL
#define PERFMON_REPORT_INTERVAL   10000000000ull
#endif

#define PCIE_STATUS_CHECK_INTERVAL    100000000ull
#define PCIE_STATUS_REPORT_INTERVAL 10000000000ull

typedef    struct {
    HW_TESTID testInstanceID;
    UINT64 totalTimer;
} HW_TD_TESTDATA;

// This struct is cached so it needs to be aligned and
// padded to the size of a cacheline.
typedef struct {
    HW_BIFROST_STATE state;
    BOOL active;
    UINT64 currentIterationStartTime;
    UINT32 currentTest;
    UINT32 current_playnice_exceed_count;
    HW_TD_TESTDATA testData[HW_TS_MAX_TESTS];
    UINT32 testsPassed;
    UINT32 testCnt;
    UINT32 power_cycle_cnt;
} __attribute__ ((aligned(HW_MAX_CACHELINE_SIZE))) HW_TD_TS_DATA;
static_assert(sizeof(HW_TD_TS_DATA) % HW_MAX_CACHELINE_SIZE == 0);

extern BOOL td_testDriverGlobalPass;
extern HW_BIFROST_STATE td_global_state_dram;
extern MEM_SRAM_UC HW_BIFROST_STATE td_global_state;
extern HW_BIFROST_STATE ts_global_states_dram[HW_PROC_CNT];
extern MEM_SRAM_UC HW_BIFROST_STATE ts_global_states[HW_PROC_CNT];
extern volatile HW_BIFROST_STATE* p_td_global_state;
extern volatile HW_BIFROST_STATE* p_ts_global_states[HW_PROC_CNT];
extern HW_TD_TESTDATA td_testdata[];
extern HW_EVENT_LOG td_eventLog;
extern HW_TD_TS_DATA td_ts_slaveData[HW_PROC_CNT];
extern UINT32 td_testCntGlobal;
extern CHAR td_internal_print_buffer[HW_PRINTF_BUF_SIZE] __attribute__ ((aligned(HW_MAX_CACHELINE_SIZE)));


//
// Function declarations
//
extern void td_printf_reply(HW_MESSAGE tempIn, UINT32 dwpid);
extern void td_flush_mailbox_reply(UINT32 dwpid);

extern void*
td_allocAligned( HW_HEAP_BLOCK heapBlockID, 
                 SIZE_FIELD size, 
                 PTR_FIELD pattern, 
                 PTR_FIELD mask,
                 HW_TESTID testID,
                 UINT32 core);
extern void td_alloc_reply(HW_HEAP_BLOCK heapBlockID, PTR_FIELD size, PTR_FIELD pattern, PTR_FIELD mask, HW_TESTID testID, HW_MAILBOX* box, UINT32 core);
extern int td_free( HW_HEAP_BLOCK heapBLockID, PTR_FIELD addr, HW_TESTID testID );
extern int td_freeAll(HW_HEAP_BLOCK heapBLockID, HW_TESTID testID);
extern int td_globalFreeAll();
extern SIZE_FIELD td_unusedHeapSpace(HW_HEAP_BLOCK heapBlockID, void** addr);
extern BOOL td_checkUnusedHeapSpace(HW_HEAP_TYPE type);

extern void td_hostPhysicalAlloc_reply(PTR_FIELD size, PTR_FIELD pattern, PTR_FIELD mask, HW_TESTID testID, HW_MAILBOX* box, UINT32 core);
extern int td_hostPhysicalFree_reply( void* addr, HW_TESTID testID );
extern int td_hostPhysicalFreeAll_reply( HW_TESTID testID );

extern void td_sendMessage_reply(HW_MESSAGE tempIn);

extern void td_getInstanceName_reply(HW_TESTID testID, UINT32 dwpid);

extern void td_msi_reply(UINT32 vector, HW_TESTID testId, UINT32 cmd, UINT32 dwpid);

extern void td_suspend_reply(UINT32 timieout, HW_TESTID testId, UINT32 dwpid);

extern void td_standby_reply(HW_TESTID testId, UINT32 dwpid);

extern void td_initTime();

extern void td_print_all_event_logs();

extern int
td_printf( const char* fmt, ... );

extern void td_perfmon_init();
extern void td_perfmon_update(UINT64 cur_time);

#endif
