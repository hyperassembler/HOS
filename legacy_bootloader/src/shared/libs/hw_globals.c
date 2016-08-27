/*-----------------------------------------------------
 |
 |      hw_globals.c
 |
 |    Contains Bifrost global variables.
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2011  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

#include "bifrost_private.h"

//
// System structs, initialized in initGlobals
//
HW_EVENT_LOG td_eventLog __attribute__ ((section (".ddr_wb.bss")));
HW_BIFROST_SYSTEM_STRUCT hw_bifrostSystemStruct __attribute__ ((section (".bf_structs.bss")));
MEM_SRAM_UC HW_MAILBOX_IDXS mailboxIdxs[HW_PROC_CNT][HW_MAILBOX_ID_CNT];
HW_MAILBOX_IDXS mailboxIdxs_dram[HW_PROC_CNT][HW_MAILBOX_ID_CNT];

//
// Print buffer for communication with external host
//
HW_PRINTF_BUFFER ext_print_buffer;

//
// This variable tracks whether failed allocs
// should be fatal or not
//
volatile BOOL hw_bAllocFailuresAreFatal = TRUE;

//
// This variable tracks whether the global init
// is finished
//
volatile BOOL hw_bIntGlobalInitFinished = FALSE;

//
// This variable tracks whether the init
// for the interrupt module is finished
//
volatile BOOL hw_bIntrInitFinished = FALSE;

//
// This variable tracks whether the global init
// for the interrupt module is finished
//
volatile BOOL hw_bIntThreadInitFinished = FALSE;

//
// This variable determines whether to terminate all tests (e.g. due to unexpected exception)
//
volatile BOOL hw_bTerminateAllTests = FALSE;

//
// Bifrost globals
//
MEM_DDR_WB BIFROST_CACHED_GLOBALS bifrostCachedGlobals = {0, PRINT_TYPE(HW_CRITICAL), 0, TRUE};

//
// Array of cacheline sizes. The HW_CACHELINE_SIZE
// macro indexes into this array to locate the
// cacheline size for the local core.
//
// Cacheline sizes are:
// 64 bits for all cores.
//
const UINT32 hw_cachelineSizes[HW_PROC_CNT] = PROC_CNT_INITIALIZER(64);


//
// External communication struct
//
__attribute__ ((section (".extcom"))) volatile HW_EXTCOM_SPACE hw_extcomSpace;

