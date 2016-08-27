/*-------------------------------------------------------
 |
 |      bifrost_types_constants.h
 |
 |      Contains all enums, typedef primitives, and
 |      defined constants.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2011  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#ifndef _BIFROST_TYPES_CONSTANTS_H
#define _BIFROST_TYPES_CONSTANTS_H

#define HW_FORCE (true)
#define HW_RELEASE (false)

// type aliases for externed test functions.
// cleanup just uses the generic HW_FUNC()
typedef void HW_FUNC();
typedef void HW_RUN_FUNC(UINT32 iteration);
#ifdef __cplusplus
// type alias for interrupt handler
typedef void (HW_INT_HANDLER_FUNC)(...);
typedef void (HW_EXC_HANDLER_FUNC)(...);
// type alias for poll handling
#endif
typedef BOOL HW_POLL_CB_FUNC(void* arg);
typedef void HW_POLL_HANDLER_CB_FUNC(void* arg);

//
// Bifrost parameter struct
//
typedef union {
    struct {
        union {
            UINT32 i;
            FLOAT32 f;
            char c[4];
        };
        UINT32 key;
    };
    UINT64 raw;
} HW_PARAM;

//
// Base class for V3 parameters
//
#ifdef __cplusplus

class BifrostTestParams
{
 public:

  // Bifrost compile flow creates
  // to set parameter values suppolied via yaml
  virtual void initParams(void) = 0;

  // Bifrost compile flow creates
  // to set parameter values suppolied via yaml
  virtual void yamlParams(void) = 0;

  // Optional function that user can implement to
  // tweake paramters values after they have initialized
  // as per contructor and YamlParams.
  virtual BOOL finalize() { return TRUE; }

  // If a subset of parameters needs to be randomized
  // every iteration, that randomization logic can go here
  virtual UINT32 randomize() { return 0; }

  VOID   init_done(UINT32 d) { m_init_done = d;}
  UINT32 init_done()         { return m_init_done;}
//
protected:
  UINT32 m_init_done;

};

class BifrostTestVars
{
 public:
    virtual VOID initVars() {;}

    VOID   init_done(UINT32 d) { m_init_done = d;}
    UINT32 init_done()         { return m_init_done;}

protected:
  UINT32 m_init_done;
};

#endif


//
// Bifrost states
//
typedef enum {
//
// Test driver states
//
HW_TD_IDLE = 3,
HW_TD_SETUP,
HW_TD_INIT,
HW_TD_RUN,
HW_TD_CLEANUP,

//
// Test slave states
//
HW_TS_IDLE = 127,
HW_TS_SETUP,
HW_TS_INIT,
HW_TS_RUN,
HW_TS_CLEANUP,
HW_TS_KILL,
HW_TS_REBOOT,
HW_TS_EXIT,

//
// Test slaves should block on these states as they wait for
// the next command from the test driver
//
HW_TS_DONE_IDLE,
HW_TS_DONE_SETUP,
HW_TS_DONE_INIT,
HW_TS_DONE_RUN,
HW_TS_DONE_CLEANUP,
HW_TS_DONE_KILL

} HW_BIFROST_STATE;

//
// Test pass/fail results
//

typedef enum {

HW_TEST_RUN_ELIGIBLE,
HW_TEST_SUSPENDED,
HW_TEST_SKIP_ITERATION,
HW_TEST_RERUN,
HW_TEST_END,
HW_TEST_FAIL,
HW_TEST_PASS

} HW_TEST_RUN_STATE;

//
// Timeouts
//

#define HW_MB_MAILBOX_TIMEOUT           (10000000)
#define HW_TEST_HARD_TIMEOUT            (280000000000llu)  // 280 seconds
#define HW_TEST_PLAYNICE_TIMEOUT        (40000000000llu)  // 40 seconds

//
// Sizes of various things
//

// In REDUCED_MEMORY_FOOTPRINT mode, reduce
// sizes of most arrays.
#ifdef REDUCED_MEMORY_FOOTPRINT
#define HW_ALLOC_NUM_BLOCKS             (100)
#define HW_TS_MAX_POLL_HANDLERS         (32)
#define HW_TS_MAX_TESTS                 (2)
#define HW_TS_STACK_SIZE                ((2 * HW_PRINTF_BUF_SIZE) + (8192)) // The stack size for each test
#define HW_KERNEL_STACK_SIZE            ((2 * HW_PRINTF_BUF_SIZE) + (1024)) // The stack size for the kernel code
#define HW_MAX_TEST_CNT                 (HW_PROC_CNT * HW_TS_MAX_TESTS)
#define HW_PRINTF_BUF_SIZE              (0x200 - (2 * HW_MAX_CACHELINE_SIZE)) // There are two cachelines' worth of control fields. Total size
                                                                                // needs to be divisible by the default page size (4096).
#define HW_MB_MAX_SIZE                  ((0x200 - (2 * sizeof(PTR_FIELD))) / sizeof(HW_MESSAGE))
                                                                                // There are two cachelines' worth of control fields. Total size
                                                                                // needs to be divisible by the default page size (4096).
#define HW_MAX_EVENT_LOG_ENTRIES        (1)
#define HW_MAX_EVENT_LOG_DATA           (4)

#else // REDUCED_MEMORY_FOOTPRINT

#define HW_ALLOC_NUM_BLOCKS             (0x2000 - 1)
#define HW_TS_MAX_POLL_HANDLERS         (32)
#define HW_TS_MAX_TESTS                 (16)
#define HW_TS_STACK_SIZE                (1024 * 1024) // The stack size for each test
#define HW_KERNEL_STACK_SIZE            ((2 * HW_PRINTF_BUF_SIZE) + (64 * 1024)) // The stack size for the kernel code
#define HW_MAX_TEST_CNT                 (HW_PROC_CNT * HW_TS_MAX_TESTS)
#define HW_PRINTF_BUF_SIZE              (0x20000 - (2 * HW_MAX_CACHELINE_SIZE)) // There are two cachelines' worth of control fields. Total size
                                                                                // needs to be divisible by the default page size (4096).
#define HW_MB_MAX_SIZE                  ((0x8000 - (2 * sizeof(PTR_FIELD))) / sizeof(HW_MESSAGE))
                                                                                // There are two cachelines' worth of control fields. Total size
                                                                                // needs to be divisible by the default page size (4096).
#define HW_MAX_EVENT_LOG_ENTRIES        (2 * 1024)
#define HW_MAX_EVENT_LOG_DATA           (4)

#endif // REDUCED_MEMORY_FOOTPRINT

//
// Other constant sizes
//
#define HW_MAX_TESTNAME_LEN             (50)

#define HW_NO_TEST_INDEX                (HW_TS_MAX_TESTS + 1)

#define HW_MAX_CACHELINE_SIZE           (64)
#define HW_PAGE_SIZE_4_KB               (4 * 1024)
#define HW_PAGE_SIZE_2_MB               (2 * 1024 * 1024)
#define HW_PAGE_SIZE_1_GB               (1024 * 1024 * 1024)
#define HW_PAGE_SIZE_DEFAULT            (HW_PAGE_SIZE_4_KB)
#define HW_PT_TABLE_SIZE                (4096)

//
// Struct sizes
//
#define HW_TESTXENTRY_SIZE              ((7 * sizeof(UINT32)) + sizeof(HW_PATHID))
#define HW_TESTXLIST_SIZE               (HW_TESTXENTRY_SIZE * HW_MAX_TEST_CNT)
#define HW_PRINTF_BUFFER_SIZE           (roundUpToMultipleUnsigned((2 * HW_MAX_CACHELINE_SIZE) + (sizeof(UCHAR) * HW_PRINTF_BUF_SIZE), HW_MAX_CACHELINE_SIZE))
#define HW_XLIST_SIZE                   (5 * sizeof(UINTPTR))
#define HW_INT_CONTEXT_SIZE             ((2 * sizeof(PTR_FIELD)) + (2 * sizeof(UINT32)))
#define HW_INT_VECTOR_MASK_SIZE         (sizeof(UINT32))
#define HW_TESTSLAVE_INT_SIZE           (roundUpToMultipleUnsigned((HW_INT_CONTEXT_SIZE * HW_TS_NUM_INTVECTORS) + HW_INT_VECTOR_MASK_SIZE + sizeof(UINT32), HW_MAX_CACHELINE_SIZE))
#define HW_EXC_CONTEXT_SIZE             (sizeof(PTR_FIELD) + (2 * sizeof(UINT32)))
#define HW_EXC_VECTOR_MASK_SIZE         (sizeof(UINT64))
#define HW_TESTSLAVE_EXC_SIZE           ((HW_EXC_CONTEXT_SIZE * HW_TS_NUM_EXCVECTORS) + HW_EXC_VECTOR_MASK_SIZE + sizeof(UINT32))
#define HW_RANDSTATE_SIZE               (60 * sizeof(UINT32))
#define HW_EVENT_LOG_ENTRY_SIZE         ((2 * sizeof(UINT64)) + (HW_MAX_EVENT_LOG_DATA * sizeof(UINT64)))
#define HW_EVENT_LOG_SIZE               (roundUpToMultipleUnsigned((HW_EVENT_LOG_ENTRY_SIZE * HW_MAX_EVENT_LOG_ENTRIES) + sizeof(UINT32), HW_MAX_CACHELINE_SIZE))
#define HW_TS_TESTDATA_SIZE             (roundUpToMultipleUnsigned(sizeof(UINT64) + (2 * sizeof(PTR_FIELD)) + sizeof(BOOL32) + (7 * sizeof(UINT32)), HW_MAX_CACHELINE_SIZE))
#define HW_TESTSLAVE_VARS_SIZE          (roundUpToMultipleUnsigned((4 * HW_MAILBOX_SIZE) + HW_PRINTF_BUFFER_SIZE + (3 * sizeof(UINT32)) + (65 * sizeof(UINT64)) + (3 * sizeof(PTR_FIELD)) + HW_TESTSLAVE_INT_SIZE + sizeof(HW_TESTSLAVE_EXC) + (sizeof(HW_POLL_CONTEXT) * HW_TS_MAX_POLL_HANDLERS) + (2 * sizeof(UINTPTR)), HW_PAGE_SIZE_DEFAULT))
#define HW_SYSCOM_SIZE                  (sizeof(UINT32))
#define HW_TESTCFGS_SIZE                ((7 * sizeof(UINT32)) + (28 * sizeof(const char)) + sizeof(UINT64) + sizeof(HW_BFINIT) + sizeof(HW_BFCOMP) + (HW_MAX_TEST_CNT * sizeof(HW_TESTXENTRY)))

//
// Print levels
//
#define PRINT_TYPE(print_type) (0x1 << print_type)
typedef enum {
// Bits 0-3 are primary verbosity.
HW_CRITICAL,
HW_STATUS,
HW_DEBUG,
HW_VERBOSE,

// Bit 4 is print synchronization.
// Bits 5-7 are internal functionality debug.
HW_PRINT_SYNC,
HW_ALLOC_DEBUG,
HW_ALLOC_VERBOSE,
HW_MAILBOX_DEBUG,

// Bits 8-9 are trace logging levels.
// Bits 10-11 are support library debug.
HW_CHECKPOINT,
HW_TRACEPOINT,
HW_STUB_DEBUG,
HW_PERFMON_DEBUG,

// Bits 12-15 are internal functionality debug.
HW_VITAG,
HW_IO_DEBUG,
HW_BOOT_DEBUG,
HW_CONTEXT_DEBUG,
} HW_PRINT_TYPE;


//
// Error codes ... insert them as you need them
//
typedef enum
{
HW_S_OK = 0,
HW_E_TIMEOUT,
HW_E_MAILBOX_FULL,
HW_E_MAILBOX_EMPTY,
HW_E_OUT_OF_BOUNDS,
HW_E_NOT_FOUND,
HW_E_OVERFLOW,
HW_E_UNSUPPORTED_COMMAND,
HW_E_TERMINATE,
HW_E_ACCESS_VIOLATION,
HW_E_NOTIMPL,
HW_E_OTHER
} HW_ERROR_CODE;

#define HW_XLIST_TERMINATION            (0xDEADBEEF)

//
// Testslave run-end condition flags
//
typedef enum {
    HW_TS_SKIP_RERUN_BITPOS,
    HW_TS_SKIP_CLEANUP_BITPOS,
    HW_TS_SKIP_REBOOT_BITPOS
} HW_TS_RUNEND_FLAGS;

#define HW_TS_SKIP_RERUN   (1 << HW_TS_SKIP_RERUN_BITPOS)
#define HW_TS_SKIP_CLEANUP (1 << HW_TS_SKIP_CLEANUP_BITPOS)
#define HW_TS_SKIP_REBOOT (1 << HW_TS_SKIP_REBOOT_BITPOS)

//
// File states
//
typedef enum {
FILE_NOT_OPENED,
FILE_OPENED_R,
FILE_OPENED_W,
FILE_OPENED_A,
} HW_FILE_STATE;

//
// Static alloc buffer types
//
typedef enum {
HW_BUFFER_EACH_CORE,
HW_BUFFER_EACH_TEST,
} HW_BUFFER_TYPE;


//
// Mutual Exclusion
//
typedef enum {
    HW_SYZMANSKI_STATE_NOT_WAITING = 0,
    HW_SYZMANSKI_STATE_WAITING_OPEN,
    HW_SYZMANSKI_STATE_WAITING_OTHERS,
    HW_SYZMANSKI_STATE_IN_DOORWAY,
    HW_SYZMANSKI_STATE_IN_ROOM,
} HW_SYZMANSKI_STATE;

//
// Host command
//
#define HW_HOSTCMD_DATA_BUF_SIZE (32)
#define HW_HOSTCMD_RESP_BUF_SIZE (32)

typedef enum{
    HOST_ERROR_NONE = 1,
    HOST_ERROR_UNSUPPORTED_CMD,
    HOST_ERROR_INVALID_PARAMETER,
    HOST_ERROR_OUT_OF_MEMORY,
    HOST_ERROR_FILE_IO,
    HOST_ERROR_PCIE,
}HW_HOSTCMD_RESP_HOST_ERROR;


typedef enum
{
    PCIE_BURST_MODE_1BYTE,
    PCIE_BURST_MODE_2BYTE,
    PCIE_BURST_MODE_4BYTE,
    PCIE_BURST_MODE_DMA,
}HW_HOSTCMD_PCIE_BURST_MODE;

typedef enum
{
    PCIE_DATA_PATTERN_RAND,
    PCIE_DATA_PATTERN_RAMP,
    PCIE_DATA_PATTERN_ARBITRARY,
}HW_HOSTCMD_PCIE_DATA_PATTERN_TYPE;

typedef enum
{
    HW_HOSTCMD_MEMCMP,
    HW_HOSTCMD_CRC32,
    HW_HOSTCMD_FILE_READ,
    HW_HOSTCMD_FILE_WRITE,
    HW_HOSTCMD_PCIE_READ,
    HW_HOSTCMD_PCIE_WRITE,
    HW_HOSTCMD_PCIE_MEMCPY,
    HW_HOSTCMD_PCIE_SUSPEND,
    HW_HOSTCMD_PCIE_RESUME,
    HW_HOSTCMD_NOTIFY_STANDBY,
    HW_HOSTCMD_QUERY_MSI,
    HW_HOSTCMD_NOTIFY_FIR,
    HW_HOSTCMD_NOTIFY_HOSTMEM_ALLOC,
    HW_HOSTCMD_FORCE_RELEASE_SIGNAL,
    HW_HOSTCMD_CDMA_CRC32
}HW_HOSTCMD;

#endif
