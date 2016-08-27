/*-----------------------------------------------------
 |
 |      td_perfmon.h
 |
 |    Header containing macros and enums 
 |    for Bifrost driver perf monitoring
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2014  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

#ifndef __TD_PERFMON_H__
#define __TD_PERFMON_H__

#define COUNTOF(A) ( (A != NULL) ? _countof(A) : 0)

#ifndef PERFMON_PRINT_LEVEL
#define PERFMON_PRINT_LEVEL       ((int) HW_STATUS)
#endif

#define PERFMON_SAMPLES_PER_EPOCH (PERFMON_REPORT_INTERVAL/PERFMON_COLLECT_INTERVAL)

//////////////////////////////////////////////////////////////////////////
//
// - PP macros to allow PERFMON event selection and over-rides
// - The following macros enables entries in the directory
// - Entire blocks can be disabled
// - Defaults to everything ON
//

#ifndef PERFMON_ENABLE_MASK
#define PERFMON_ENABLE_MASK               0xFFFFFFFF
#endif

#ifndef PERFMON_ENABLE_FABRIC_EVENTS
#define PERFMON_ENABLE_FABRIC_EVENTS      ((PERFMON_ENABLE_MASK >> 0) & 1)
#endif

#ifndef PERFMON_ENABLE_NODE00_EVENTS
#define PERFMON_ENABLE_NODE00_EVENTS      ((PERFMON_ENABLE_MASK >> 1) & 1)
#endif

#ifndef PERFMON_ENABLE_NODE01_EVENTS
#define PERFMON_ENABLE_NODE01_EVENTS      ((PERFMON_ENABLE_MASK >> 2) & 1)
#endif

#ifndef PERFMON_ENABLE_NODE02_EVENTS
#define PERFMON_ENABLE_NODE02_EVENTS      ((PERFMON_ENABLE_MASK >> 3) & 1)
#endif

#ifndef PERFMON_ENABLE_NODE03_EVENTS
#define PERFMON_ENABLE_NODE03_EVENTS      ((PERFMON_ENABLE_MASK >> 4) & 1)
#endif

#ifndef PERFMON_ENABLE_NODE04_EVENTS
#define PERFMON_ENABLE_NODE04_EVENTS      ((PERFMON_ENABLE_MASK >> 5) & 1)
#endif

#ifndef PERFMON_ENABLE_NODE05_EVENTS
#define PERFMON_ENABLE_NODE05_EVENTS      ((PERFMON_ENABLE_MASK >> 6) & 1)
#endif

#ifndef PERFMON_ENABLE_NODE06_EVENTS
#define PERFMON_ENABLE_NODE06_EVENTS      ((PERFMON_ENABLE_MASK >> 7) & 1)
#endif

#ifndef PERFMON_ENABLE_NODE07_EVENTS
#define PERFMON_ENABLE_NODE07_EVENTS      ((PERFMON_ENABLE_MASK >> 8) & 1)
#endif

#ifndef PERFMON_ENABLE_NODE08_EVENTS
#define PERFMON_ENABLE_NODE08_EVENTS      ((PERFMON_ENABLE_MASK >> 9) & 1)
#endif

#ifndef PERFMON_ENABLE_NODE09_EVENTS
#define PERFMON_ENABLE_NODE09_EVENTS      ((PERFMON_ENABLE_MASK >> 10) & 1)
#endif

#ifndef PERFMON_ENABLE_NODE10_EVENTS
#define PERFMON_ENABLE_NODE10_EVENTS      ((PERFMON_ENABLE_MASK >> 11) & 1)
#endif

#ifndef PERFMON_ENABLE_NODE11_EVENTS
#define PERFMON_ENABLE_NODE11_EVENTS      ((PERFMON_ENABLE_MASK >> 12) & 1)
#endif

#ifndef PERFMON_ENABLE_HANA0_EVENTS
#define PERFMON_ENABLE_HANA0_EVENTS      ((PERFMON_ENABLE_MASK >> 13) & 1)
#endif

#ifndef PERFMON_ENABLE_KULA0_EVENTS
#define PERFMON_ENABLE_KULA0_EVENTS      ((PERFMON_ENABLE_MASK >> 14) & 1)
#endif

#ifndef PERFMON_ENABLE_KULA1_EVENTS
#define PERFMON_ENABLE_KULA1_EVENTS      ((PERFMON_ENABLE_MASK >> 15) & 1)
#endif

#ifndef PERFMON_ENABLE_PALA0_EVENTS
#define PERFMON_ENABLE_PALA0_EVENTS      ((PERFMON_ENABLE_MASK >> 16) & 1)
#endif

#ifndef PERFMON_ENABLE_PALA1_EVENTS
#define PERFMON_ENABLE_PALA1_EVENTS      ((PERFMON_ENABLE_MASK >> 17) & 1)
#endif

#ifndef PERFMON_ENABLE_PALA2_EVENTS
#define PERFMON_ENABLE_PALA2_EVENTS      ((PERFMON_ENABLE_MASK >> 18) & 1)
#endif

#ifndef PERFMON_ENABLE_PALA3_EVENTS
#define PERFMON_ENABLE_PALA3_EVENTS      ((PERFMON_ENABLE_MASK >> 19) & 1)
#endif

////////////////////////////////////////////////////////////////////////////
//
// - PP macros that allow over-ride of individual event sets within a block
// - Macros might apply to Event sets in multiple nodes & ISPs
//

//Fabric 
#ifndef PERFMON_FABRIC_ENABLE_MASK
#define PERFMON_FABRIC_ENABLE_MASK        0x7
#endif

#ifndef PERFMON_FABRIC_ENABLE_BW_SET
#define PERFMON_FABRIC_ENABLE_BW_SET      ((PERFMON_FABRIC_ENABLE_MASK >> 0) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_MEMMAX_SET
#define PERFMON_FABRIC_ENABLE_MEMMAX_SET  ((PERFMON_FABRIC_ENABLE_MASK >> 1) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_LATENCY_SET
#define PERFMON_FABRIC_ENABLE_LATENCY_SET ((PERFMON_FABRIC_ENABLE_MASK >> 2) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_SFP0_SET
#define PERFMON_FABRIC_ENABLE_SFP0_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 3) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_SFP1_SET
#define PERFMON_FABRIC_ENABLE_SFP1_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 4) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_SFP2_SET
#define PERFMON_FABRIC_ENABLE_SFP2_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 5) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_SFP3_SET
#define PERFMON_FABRIC_ENABLE_SFP3_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 6) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_SFP4_SET
#define PERFMON_FABRIC_ENABLE_SFP4_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 7) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_SFP5_SET
#define PERFMON_FABRIC_ENABLE_SFP5_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 8) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_SFP6_SET
#define PERFMON_FABRIC_ENABLE_SFP6_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 9) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_VFP0_SET
#define PERFMON_FABRIC_ENABLE_VFP0_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 10) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_VFP1_SET
#define PERFMON_FABRIC_ENABLE_VFP1_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 11) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_VFP2_SET
#define PERFMON_FABRIC_ENABLE_VFP2_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 12) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_VFP3_SET
#define PERFMON_FABRIC_ENABLE_VFP3_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 13) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_VFP4_SET
#define PERFMON_FABRIC_ENABLE_VFP4_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 14) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_HAN0_SET
#define PERFMON_FABRIC_ENABLE_HAN0_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 15) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_KUL0_SET
#define PERFMON_FABRIC_ENABLE_KUL0_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 16) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_KUL1_SET
#define PERFMON_FABRIC_ENABLE_KUL1_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 17) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_PAL0_SET
#define PERFMON_FABRIC_ENABLE_PAL0_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 18) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_PAL1_SET
#define PERFMON_FABRIC_ENABLE_PAL1_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 19) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_PAL2_SET
#define PERFMON_FABRIC_ENABLE_PAL2_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 20) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_PAL3_SET
#define PERFMON_FABRIC_ENABLE_PAL3_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 21) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_PCIE_SET
#define PERFMON_FABRIC_ENABLE_PCIE_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 22) & 1)
#endif

#ifndef PERFMON_FABRIC_ENABLE_GDMA_SET
#define PERFMON_FABRIC_ENABLE_GDMA_SET    ((PERFMON_FABRIC_ENABLE_MASK >> 23) & 1)
#endif

#if (defined PERFMON_FABRIC_ENABLE_DRAM_FILTER || defined PERFMON_FABRIC_ENABLE_SRAM_FILTER || defined PERFMON_FABRIC_ENABLE_PCIE_FILTER)
#define PERFMON_FILTER_ENABLED
#endif

// Node
#ifndef PERFMON_NODE_ENABLE_MASK
#define PERFMON_NODE_ENABLE_MASK           0xFFFFFFFF
#endif

#ifndef PERFMON_NODE_ENABLE_CDMA_SET
#define PERFMON_NODE_ENABLE_CDMA_SET       ((PERFMON_NODE_ENABLE_MASK >> 0) & 1)
#endif

#ifndef PERFMON_NODE_ENABLE_CLASSIFIER_SET
#define PERFMON_NODE_ENABLE_CLASSIFIER_SET ((PERFMON_NODE_ENABLE_MASK >> 1) & 1)
#endif

//
// Result structure per event per epoch
//
typedef struct
{
    UINT64         minSample;
    UINT64         maxSample;
    UINT64         cumValue;
    UINT32         numSamples;
    UINT64         currSample;
} TD_PERFMON_DATA;

//
// Performance counter entry definition
//
typedef struct 
{
    const UINT32     nodeNum;
    const char      *eventDesc;
    const UINT32     eventSel;
    const UINT32     eventMask;
    const UINT32     filterMask;
    TD_PERFMON_DATA *pSaveData;
} TD_PERFMON_PC_LIST_ENTRY;

//
// Derived performance metric entry definition
//
typedef struct 
{
    const char      *eventDesc;
    UINT64 (* const pCalcFunction) (const TD_PERFMON_PC_LIST_ENTRY* const);
    TD_PERFMON_DATA *pSaveData;
} TD_PERFMON_DERIVED_PC_ENTRY;

//
// Event set includes the primary events (directly counted by HW) as well
// derived events which are calculated based on primary events
//
typedef struct
{
    TD_PERFMON_PC_LIST_ENTRY*    const pEventList;
    TD_PERFMON_DERIVED_PC_ENTRY* const pDerivedList;
    const UINT32                       baseSize;
    const UINT32                       derivedSize;
} TD_PERFMON_EVENT_SET;

//
// Directory of all counter blocks to be tracked
//
typedef struct
{
    const PERF_GROUP                 groupType;
    const char*                      prefixStr;
    TD_PERFMON_EVENT_SET* const      pEventSets;
    const UINT32                     listSize;
    UINT32                           readPtr;
    UINT32                           writePtr;
    BOOL                             enabled;
} TD_PERFMON_PC_DIR_ENTRY;

static const UINT32 FABRIC_CLK_IN_NS = 4;
static const UINT32 NODE_CLK_IN_NS   = 2;

#endif
