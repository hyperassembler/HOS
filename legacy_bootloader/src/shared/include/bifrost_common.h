/*-------------------------------------------------------
 |
 |     bifrost_common.h
 |
 |  Contains structs common to all layers
 |  of the Bifrost libraries.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2011-2014  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#ifndef _BIFROST_COMMON_H
#define _BIFROST_COMMON_H

// For human-readability in structure definitions
#define PORTABLE_PTR(real_type) PTR_FIELD

//
// Test execution entry.  This *must* match up with the configuration
// image binary generated from the YAML file
//
typedef struct {
    HW_PATHID testPathID;     // Used to locate this test's init_, runTest_ and cleanup_ functions
    HW_TESTID testInstanceID; // Test ID
    UINT32 globalInstanceIdx; // Index which is unique globally
    UINT32 localInstanceIdx;  // Index which is unique per test-source
    UINT32 coreNumber;        // Core this test will run on
    UINT32 threadPriority;    // Thread priority for scheduling
    UINT32 paramOffset;       // Offset of this test's parameters from the start of the parameter struct
    UINT32 paramCount;        // Number of parameters this test has
} __attribute__ ((packed)) HW_TESTXENTRY; // this struct is generated externally, need to pack
                                          // to ensure unified view of struct alignment and size

typedef struct {
    UINT32 version;                 // Must be the first field so that we have a consistent reference point in all versions
    UINT32 tpid;                    // Driver core number   
    UINT32 sync;                    // Flag to determine whether to run in synchronized mode
    UINT32 seed;                    // SEED master seed
    UINT32 suite_rand_seed;         // SUITE_RAND_SEED suite-specific seed
    UINT32 verbosity;               // Bifrost VERBOSITY
    UINT32 platform;                // Bifrost PLATFORM
    char config[28];                // Indicates what the hardware configuration is
    UINT64 runtime;                 // Minimum runtime
    HW_BFINIT bfinit;               // Bifrost init flags
    HW_BFCOMP bfcomp;               // Bifrost compile mode
    HW_TESTXENTRY entryList[HW_MAX_TEST_CNT];
} __attribute__ ((packed)) HW_TESTCFGS;

#define HW_TESTCFGS_VERSION 14 // This value should be incremented whenever the memory layout of HW_TESTCFGS changes

typedef struct {
    PORTABLE_PTR(HW_BUFFER*) addr;
    HW_PATHID testPathID;
    UINT32 cnt;
} __attribute__ ((packed)) HW_BUFFER_TABLE;

typedef struct {
    PORTABLE_PTR(VOID*) addr;
    HW_BUFFER_TYPE type;
} __attribute__ ((packed)) HW_BUFFER;


// Formats for EXTCOM flags
typedef union {
    struct
    {
        UINT32 payload   : 16;
        UINT32 testID    : 8;
        UINT32 dwpid     : 6;
        UINT32 need_resp : 1;
        UINT32 go        : 1;
    };
    UINT32 AsUINT32;
} __attribute__ ((packed)) HW_EXTCOM_SIGNAL;

typedef struct {
    HW_EXTCOM_SIGNAL flag;
    UINT32 fmt;
} __attribute__ ((packed)) HW_EXTCOM_PRINT_FLAG;

typedef struct {
    HW_EXTCOM_SIGNAL cmd;
    UINT32 rsvd1[7];
    UINT32 resp;
    UINT32 rsvd2[7];
    UINT8  cmd_data[HW_HOSTCMD_DATA_BUF_SIZE];
    UINT8  resp_data[HW_HOSTCMD_RESP_BUF_SIZE];
} __attribute__ ((packed)) HW_EXTCOM_CMD_BUF;

typedef struct {
    HW_EXTCOM_PRINT_FLAG buf;  // Print flags, one for each core
    UINT32 rsvd[6];            // Pad to cacheline size
    HW_EXTCOM_SIGNAL post;     // Flag for sending POST codes
    UINT32 rsvd2[7];           // Pad to cacheline size
    HW_EXTCOM_CMD_BUF cmd_buf; // host cmd buffer, one for each core
} __attribute__ ((packed)) HW_EXTCOM_CORE_FLAGS;

typedef struct {
    UINT32 version;                          // Must be the first field so that we have a consistent reference point in all versions
    UINT32 rsvd1[1];                         // Reserved
    UINT64 error;                            // Reserved for BFM to cause an error when a write occurs here
    UINT32 rsvd2[12];                        // Reserved
    HW_EXTCOM_CORE_FLAGS core[HW_PROC_CNT];  // Per-core flags
    UINT32 rsvd3[388];                       // Reserved
    UINT32 disp[8];                          // Disp Command interface 4 words each 2 instances
    PORTABLE_PTR(HW_PRINTF_BUFFER*) ext_buf; // Pointer to printf buffer between testdriver and host
    UINT32 eot_flag;                         // Flag for signaling end of test with coolcode or baadcode
    UINT32 rsvd4[1];                         // Reserved
} __attribute__ ((packed)) HW_EXTCOM_SPACE;

#define HW_EXTCOM_VERSION 4 // This value should be incremented whenever the memory layout of HW_EXTCOM_SPACE changes

extern volatile HW_EXTCOM_SPACE hw_extcomSpace;

typedef struct {
    UINT32 head __attribute__ ((aligned(HW_MAX_CACHELINE_SIZE)));
    UINT32 tail __attribute__ ((aligned(HW_MAX_CACHELINE_SIZE)));
    UCHAR buffer[HW_PRINTF_BUF_SIZE] __attribute__ ((aligned(HW_MAX_CACHELINE_SIZE)));
} __attribute__ ((packed, aligned(HW_MAX_CACHELINE_SIZE))) HW_PRINTF_BUFFER; // this struct is shared between multiple cores, need to pack
                                                                             // to ensure unified view of struct alignment and size.
//
// Test execution lists that are linked
// in from the ttab files
//

typedef struct {
    HW_FUNC* pinit;
    HW_RUN_FUNC* prunTest;
    HW_FUNC* pcleanup;
    const char* testPathName;
    UINT32 testPathHash;
} HW_XLIST;

extern HW_XLIST* hw_tl_master[];
extern HW_XLIST hw_tl_x86_64[];
extern HW_XLIST hw_tl_xtensa_sfp[];
extern HW_XLIST hw_tl_xtensa_vfp[];

typedef struct {
    SIZE_T size;
    HW_FILE_STATE state;
    BOOL plus;
    const char* name;
    UINT64 namehash;
    UCHAR* arr;
} HW_FILE;

// One array of HW_FILE pointers and file counts
// per test instance.
extern UINT32 hw_file_counts[];
extern HW_FILE** hw_file_lists[];

extern BOOL hw_activeCoreList[HW_PROC_CNT];

typedef struct {
    PORTABLE_PTR(HW_INT_HANDLER_FUNC*) pHandler; // Interrupt handler
    PTR_FIELD buffer;                            // Pointer to a buffer containing data for the interrupt handler
    UINT32 dwXTestOwner;                         // Index of test that owns it
    BOOL32 preemptive;                           // In ports that support real interrupts, a value of 1 here indicates that we wish to use that functionality
} __attribute__ ((packed)) HW_INT_CONTEXT;

typedef struct {
    PORTABLE_PTR(HW_EXC_HANDLER_FUNC*) pHandler; // Exception handler
    UINT32 dwXTestOwner;                         // Test on slave that owns it
    BOOL32 preemptive;                           // In ports that support real interrupts, a value of 1 here indicates that we wish to use that functionality
} __attribute__ ((packed)) HW_EXC_CONTEXT;

typedef UINT32 HW_INT_VECTOR_MASK;
typedef UINT64 HW_EXC_VECTOR_MASK;

typedef struct {
    HW_INT_CONTEXT contexts[HW_TS_NUM_INTVECTORS]; // Interrupts on the slave
    HW_INT_VECTOR_MASK pendingInterruptMask;     // Tracks interrupts that have arrived but haven't been handled yet
    BOOL32 isInitialized;
} __attribute__ ((packed,aligned(HW_MAX_CACHELINE_SIZE))) HW_TESTSLAVE_INT;

typedef struct {
    HW_EXC_CONTEXT contexts[HW_TS_NUM_EXCVECTORS]; // Exceptions on the slave
    HW_EXC_VECTOR_MASK pendingExceptionMask;     // Tracks exceptions that have arrived but haven't been handled yet
    BOOL32 isInitialized;
} __attribute__ ((packed,aligned(HW_MAX_CACHELINE_SIZE))) HW_TESTSLAVE_EXC;

typedef struct {
    PORTABLE_PTR(HW_POLL_FUNC*) pPollCallback;   // Polling function for test to register
    PORTABLE_PTR(HW_POLL_HANDLER_CB_FUNC*) pHandlerCallback; // Callback upon successful poll
    PTR_FIELD callbackBuffer;                    // Pointer to a buffer containing data for both callbacks
    UINT32 testIdx;                              // Index of test that owns it
} __attribute__ ((packed)) HW_POLL_CONTEXT;

typedef struct {
    HW_POLL_CONTEXT pollHandlers[HW_TS_MAX_POLL_HANDLERS]; // Poll contexts on the slave
} __attribute__ ((packed,aligned(HW_MAX_CACHELINE_SIZE))) HW_TESTSLAVE_POLL;

typedef struct
{
    PTR_FIELD addr;
    SIZE_FIELD size;
    HW_HEAP_TYPE type;
    HW_TESTID testInstanceID;
} HW_ALLOC_RECORD;

typedef struct
{
    HW_ALLOC_RECORD records[HW_ALLOC_NUM_BLOCKS];
    UINT32 next_unused;
    UINT32 count;
} __attribute__ ((packed,aligned(HW_MAX_CACHELINE_SIZE))) HW_ALLOC_TRACKER;

typedef struct {
    INT32  rand_j;
    INT32  rand_k;
    UINT32 seed_array[ 55 ];
    BOOL32 isInitialized;
    UINT32 initialSeed;
    UINT32 currentIteration;
    UINT32 rsvd[4]; // Pad out struct to 256 bytes
} __attribute__ ((packed,aligned(HW_MAX_CACHELINE_SIZE))) HW_RANDSTATE;


typedef struct {
    HW_EVENT_LOG eventLog;                          // Log for events that occurred during this test
    HW_MAILBOX mailbox;                             // Test's mailbox
    UINT32 head;
    UINT32 tail;
    HW_TEST_RUN_STATE testStatus;                   // State of test: running/passed/failed/etc.
    UINT64 testTimeout;                             // Timeout for this test.
    HW_TESTID testInstanceID;                       // unique identifier per test instance
    UINT32 testPointerIndex;                        // Index at which the function pointers for this test are found
} __attribute__ ((packed,aligned(HW_MAX_CACHELINE_SIZE))) HW_TS_PUBLIC_TESTDATA;

typedef struct {
    UINT64 runtimeStart;                            // first run phase time stamp

    PORTABLE_PTR(HW_PARAM*) paramPtr;               // Stores the location of this test's params.
    PORTABLE_PTR(void*) privateVars ;               // Address for this test's private variables section
    
    BOOL32 yielded;                                 // Tracks whether this test is in the middle of a yield
    UINT32 errorCnt;                                // Tracks number of errors encountered by the test
    UINT32 runEndConditions;                        // Tracks end of run conditions (do cleanup, can reboot, etc.)
    UINT32 dwIteration;                             // Current number of test iterations
    
    UINT32 testBufferIndex;                         // Index at which the buffer pointers for this test are found
    UINT32 testGlobalIndex;                         // Globally unique index
                                                    // NONE of these indexes are the same as the "dwCurrentXTest" index.
                                                    // And they are not the same as each other.
    
    UINT32 paramCount;                              // Stores the number of params this test has.
    
    UINT32 verbosity;                               // Verbosity level for this test.
} __attribute__ ((packed,aligned(HW_MAX_CACHELINE_SIZE))) HW_TS_TESTDATA;

typedef struct {
    // Structures that are required to be cacheline-aligned MUST be
    // placed at the beginning of this struct (before the UINT32's, etc.)
    // Since the outer structure is packed, the 'aligned' compiler
    // directive will be ignored, although the size will still be adjusted.
    HW_MAILBOX driverMailbox;                       // Mailboxes
    HW_MAILBOX driverLowPrioMailbox;
    HW_MAILBOX slaveMailbox;
    HW_MAILBOX slaveLowPrioMailbox;
    HW_PRINTF_BUFFER printBuffer;
    HW_TESTSLAVE_INT interrupts;                    // Stores data regarding interrupts
    HW_TESTSLAVE_EXC exceptions;                    // Stores data regarding exceptions
    HW_TESTSLAVE_POLL polls;                        // Stores data regarding polling
    UINT32 dwTestCnt;                               // Number of tests running on the slave
    UINT32 dwTestCleanupCnt;                        // Number of tests on the slave that need to cleanup
    UINT64 pollIntTimeout;                          // Timeout for polling and interrupts
    PTR_FIELD sharedDynamicCoreData;                // Pointer to space that will be allocated for shared per-test and per-testslave variables
    PORTABLE_PTR(HW_ALLOC_TRACKER*) allocTracker;   // Address for the core's alloc records
    PORTABLE_PTR(HW_EVENT_LOG*) eventLog;           // Log for events that occurred on this core
    PORTABLE_PTR(CHAR*) internalPrintBuffer;        // Internal print buffer, for when prints are not routed through the testdriver
    PORTABLE_PTR(HW_TS_TESTDATA*) testData;         // Data for individual tests running on this slave
    PORTABLE_PTR(HW_TS_PUBLIC_TESTDATA*) publicTestData; // Shared data for individual tests running on this slave
} __attribute__ ((packed,aligned(HW_PAGE_SIZE_DEFAULT))) HW_TESTSLAVE_VARS;
                                                    // this struct is shared between multiple cores, need to pack
                                                    // to ensure unified view of struct alignment and size. in
                                                    // addition, need to align to a cacheline to prevent
                                                    // cache coherency issues with Tensilica cores.

typedef struct {
    HW_TESTSLAVE_VARS testSlaveVarTable[HW_PROC_CNT] __attribute__ ((aligned(HW_PAGE_SIZE_DEFAULT)));
    union {
        HW_SYSCOM syscomSpace;
        UINT8 syscomSpaceArr[HW_SYS_SYSCOM_SIZE];
    };
} __attribute__ ((packed)) HW_BIFROST_SYSTEM_STRUCT;
extern HW_BIFROST_SYSTEM_STRUCT hw_bifrostSystemStruct;

typedef struct {
    UINTPTR base;
    SIZE_T size;
    BOOL cached;
} HW_HEAP_BLOCK_INFO;

#define HW_SUPERNODE(core) (HW_SUPERNODE_MAPPING[core])
static inline UINT32 hw_getSuperNodeNum()
{
    unsigned int core = hw_getCoreNum();
    return HW_SUPERNODE(core);
}

extern HW_TESTCFGS* hw_pTestConfigs;
#define testSlaveVarTable (hw_bifrostSystemStruct.testSlaveVarTable)
#define testSlaveVars     (&testSlaveVarTable[hw_getCoreNum()])

//
// Store frequently accessed globals in a cached struct
//
typedef struct {
    UINT32 tpid;                     // The thread ID of the testdriver
    UINT32 hw_global_verbosity;      // Verbosity level
    UINT64 hw_beginning_cycle_count; // Keep track of how many nanoseconds have passed since start of execution
    BOOL hw_bRunInSync;              // Determines whether run is synchronized or not
    UINT8 rsvd[47];                  // Pad to cacheline size
} __attribute__ ((aligned(HW_MAX_CACHELINE_SIZE))) BIFROST_CACHED_GLOBALS;

extern BIFROST_CACHED_GLOBALS bifrostCachedGlobals;

//
// Print buffer for communication with external host
//
extern HW_PRINTF_BUFFER ext_print_buffer;

//
// This variable tracks whether failed allocs
// should be fatal or not
//
extern volatile BOOL hw_bAllocFailuresAreFatal;

//
// This variable tracks whether the global init
// is finished
//
extern volatile BOOL hw_bIntGlobalInitFinished;

//
// This variable tracks whether the interrupt module
// is finished
//
extern volatile BOOL hw_bIntrInitFinished;

//
// This variable tracks whether the global init
// for interrupt module is finished
//
extern volatile BOOL hw_bIntThreadInitFinished;

//
// Array of cacheline sizes. The HW_CACHELINE_SIZE
// macro indexes into this array to locate the
// cacheline size for the local core.
//
extern const UINT32 hw_cachelineSizes[HW_PROC_CNT];

//
// This variable contains the bases
// and sizes for all of the heaps.
//
extern HW_HEAP_BLOCK_INFO hw_heapBlockInfo[HW_HEAP_BLOCK_CNT];


//
// host command data structures
//
typedef struct {
    UINT8* ptr1;
    UINT8* ptr2;
    UINT32 len;
}HW_HOSTCMD_DATA_MEMCMP;

typedef struct {
    UINT8* ptr;
    UINT32 len;
    UINT32 prevRemainder;
}HW_HOSTCMD_DATA_CRC32;

typedef struct {
    UINT8* ptr;
    UINT32 len;
    UINT32 prevRemainder;
    UINT32 frag_en;
    UINT32 frag_threshold_enc;
    UINT32 xdim;
    UINT32 ydim;
}HW_HOSTCMD_DATA_CDMA_CRC32;


typedef struct {
    CHAR* fileName;
    UINT32 fileOffset;
    CHAR* buffer;
    UINT32 size;
}HW_HOSTCMD_DATA_FILE_READ;

typedef struct {
    CHAR* fileName;
    UINT32 fileOffset;
    CHAR* buffer;
    UINT32 size;
}HW_HOSTCMD_DATA_FILE_WRITE;

typedef struct {
    UINT8* ptr;
    UINT32 len;
    UINT32 checksum;
    HW_HOSTCMD_PCIE_BURST_MODE burstMode;
}HW_HOSTCMD_DATA_PCIE_READ;

typedef struct {
    UINT8* ptr;
    UINT32 len;
    HW_HOSTCMD_PCIE_BURST_MODE burstMode;
    HW_HOSTCMD_PCIE_DATA_PATTERN_TYPE patternType;
    UINT32 patternData[2];
    BOOL32 needChecksum;
}HW_HOSTCMD_DATA_PCIE_WRITE;

typedef struct {
    UINT8 data[4];
    BOOL32 result;
}HW_HOSTCMD_RESP_PCIE_READ;

typedef struct {
    UINT32 checksum;
    BOOL32 result;
}HW_HOSTCMD_RESP_PCIE_WRITE;

typedef struct {
    UINT8* dest;
    UINT8* src;
    UINT32 len;
    HW_HOSTCMD_PCIE_BURST_MODE burstMode;
}HW_HOSTCMD_DATA_PCIE_MEMCPY;

typedef struct {
    UINT32 timeout;
}HW_HOSTCMD_DATA_PCIE_SUSPEND;

typedef struct {
    UINT8 vector;
}HW_HOSTCMD_DATA_QUERY_MSI;

typedef struct {
    UINT64 hostmemPhyAddrBase;
    BOOL32 result;
}HW_HOSTCMD_RESP_NOTIFY_HOSTMEM_ALLOC;

typedef struct {
  BOOL fr_n;
  CHAR * sig_name;
  UINT64 value;
}HW_HOSTCMD_DATA_FORCE_RELEASE;

// boot diag
typedef struct
{
    UINT64 start;
    UINT64 end;
}HW_BOOT_DIAG_DRAMC_INIT_TIME;

#endif
