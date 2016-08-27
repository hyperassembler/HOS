/*-----------------------------------------------------
 |
 |    td_perfmon.c
 |    ============
 |    Contains automatic performance monitoring
 |    functionality including scheduling, collecting
 |    and reporting of all the counter blocks 
 |    present on the chip
 | 
 |------------------------------------------------------
 |
 |    Copyright (C) 2014  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

/* 
 * TERMINOLOGY:
 * ------------
 *              Interval => Represents PERFMON_COLLECT_INTERVAL, period of time during which one sample 
 *                          of a given set of events are collected
 *
 *              Epoch    => Represents PERFMON_REPORT_INTERVAL, period of time between stats dump. 
 *                          Usually contains multiple samples of every event being monitored by the library
 *
 *              Default settings: Epoch = 10 x Interval
 */

// TODO: enable perfmon
#if 0

#include "bifrost_private.h"

#define BDIGITS(x)  ((x) - (((x)>>1)&0x77777777) - (((x)>>2)&0x33333333) - (((x)>>3)&0x11111111))
#define BITCOUNT(x)   (((BDIGITS(x)+(BDIGITS(x)>>4)) & 0x0F0F0F0F) % 0xFF)

//
// Global constants, and variables
//

static UINT64 prev_time    = 0ull;
static UINT32 interval_cnt = 0;

//
// - Event set definitions - contains groups of primary events needed to calculate derived metrics
// - A set will always be scheduled atomically (i.e, they will always be scheduled together)
// - Try to keep setSize == # HW counters, or else there will be wasted perf counter slots
// - Not all events in a set need to be related - just fill up empty slots with single events
//
enum Bandwidth_Set_Offsets
{
   BANDWIDTH_SET_CYCLES_OFFSET,
   BANDWIDTH_SET_DRAM_RD_WORDS_OFFSET,
   BANDWIDTH_SET_DRAM_WR_WORDS_OFFSET,
   BANDWIDTH_SET_DRAM_RDWR_WORDS_OFFSET,
   BANDWIDTH_SET_SRAM_RD_WORDS_OFFSET,
   BANDWIDTH_SET_SRAM_WR_WORDS_OFFSET,
   BANDWIDTH_SET_PCIE_RD_WORDS_OFFSET,
   BANDWIDTH_SET_PCIE_WR_WORDS_OFFSET,
};

#define FABRIC_BANDWIDTH_SET_LIST(s,m)                              \
{                                                                   \
    {0, s "Cycles",          PERF_SELECT_FABRIC_DRAM_EVENT , PERF_MASK_FABRIC_CYCLES  ,   m, NULL }, \
    {0, s "Dram Rd Words",   PERF_SELECT_FABRIC_DRAM_EVENT , PERF_MASK_FABRIC_RD_WORDS,   m, NULL }, \
    {0, s "Dram Wr Words",   PERF_SELECT_FABRIC_DRAM_EVENT , PERF_MASK_FABRIC_WR_WORDS,   m, NULL }, \
    {0, s "Dram RdWr Words", PERF_SELECT_FABRIC_DRAM_EVENT , PERF_MASK_FABRIC_RDWR_WORDS, m, NULL }, \
    {0, s "Sram Rd Words",   PERF_SELECT_FABRIC_SRAM_EVENT , PERF_MASK_FABRIC_RD_WORDS,   m, NULL }, \
    {0, s "Sram Wr Words",   PERF_SELECT_FABRIC_SRAM_EVENT , PERF_MASK_FABRIC_WR_WORDS,   m, NULL }, \
    {0, s "Pcie Rd Words",   PERF_SELECT_FABRIC_PCIE_EVENT , PERF_MASK_FABRIC_RD_WORDS,   m, NULL }, \
    {0, s "Pcie Wr Words",   PERF_SELECT_FABRIC_PCIE_EVENT , PERF_MASK_FABRIC_WR_WORDS,   m, NULL }, \
}                                                           \

static TD_PERFMON_PC_LIST_ENTRY td_fabric_all_bandwidth_set_base[]  = FABRIC_BANDWIDTH_SET_LIST ("Chip ", 0);

#ifndef REDUCED_MEMORY_FOOTPRINT
static TD_PERFMON_PC_LIST_ENTRY td_fabric_sfp0_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Sfp0 ", 1 << PMU_MSTRID_SFP0);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_sfp1_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Sfp1 ", 1 << PMU_MSTRID_SFP1);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_sfp2_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Sfp2 ", 1 << PMU_MSTRID_SFP2);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_sfp3_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Sfp3 ", 1 << PMU_MSTRID_SFP3);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_sfp4_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Sfp4 ", 1 << PMU_MSTRID_SFP4);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_sfp5_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Sfp5 ", 1 << PMU_MSTRID_SFP5);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_sfp6_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Sfp6 ", 1 << PMU_MSTRID_SFP6);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_vfp0_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Vfp0 ", 1 << PMU_MSTRID_VFP0);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_vfp1_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Vfp1 ", 1 << PMU_MSTRID_VFP1);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_vfp2_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Vfp2 ", 1 << PMU_MSTRID_VFP2);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_vfp3_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Vfp3 ", 1 << PMU_MSTRID_VFP3);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_vfp4_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Vfp4 ", 1 << PMU_MSTRID_VFP4);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_han0_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Han0 ", 1 << PMU_MSTRID_HANA0);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_kul0_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Kul0 ", 1 << PMU_MSTRID_KULA0);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_kul1_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Kul1 ", 1 << PMU_MSTRID_KULA1);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_pal0_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Pal0 ", 1 << PMU_MSTRID_PALA0);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_pal1_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Pal1 ", 1 << PMU_MSTRID_PALA1);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_pal2_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Pal2 ", 1 << PMU_MSTRID_PALA2);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_pal3_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Pal3 ", 1 << PMU_MSTRID_PALA3);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_pcie_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Pcie ", 1 << PMU_MSTRID_PCIEC);
static TD_PERFMON_PC_LIST_ENTRY td_fabric_gdma_bandwidth_set_base[] = FABRIC_BANDWIDTH_SET_LIST ("Gdma ", 1 << PMU_MSTRID_GDMA);

enum Memmax_Set_Offsets
{
    MEMMAX_SET_PGHIT_OFFSET,
    MEMMAX_SET_PGMISS_OFFSET,
    MEMMAX_SET_PGCFLT_OFFSET,
    MEMMAX_SET_DRAM_RD_REQS_OFFSET,
    MEMMAX_SET_DRAM_WR_REQS_OFFSET,
    MEMMAX_SET_DRAM_RD_OUTS_OFFSET,
    MEMMAX_SET_DRAM_WR_OUTS_OFFSET,
    MEMMAX_SET_DRAM_NUM_NEG_OFFSET,
};

static TD_PERFMON_PC_LIST_ENTRY td_fabric_memmax_set_base[] =
{
    {0, "Memmax Pghit",  PERF_SELECT_FABRIC_MEMMAX_PGHIT , PERF_MASK_FABRIC_NO_EVENT,        0, NULL },
    {0, "Memmax Pgmiss", PERF_SELECT_FABRIC_MEMMAX_PGMISS, PERF_MASK_FABRIC_NO_EVENT,        0, NULL },
    {0, "Memmax Pgcflt", PERF_SELECT_FABRIC_MEMMAX_PGCFLT, PERF_MASK_FABRIC_NO_EVENT,        0, NULL },    
    {0, "Dram Rd Reqs",  PERF_SELECT_FABRIC_DRAM_EVENT   , PERF_MASK_FABRIC_RD_REQS,         0, NULL },
    {0, "Dram Wr Reqs",  PERF_SELECT_FABRIC_DRAM_EVENT   , PERF_MASK_FABRIC_WR_REQS,         0, NULL },
    {0, "Dram Rd Outs",  PERF_SELECT_FABRIC_DRAM_EVENT   , PERF_MASK_FABRIC_RD_OUTSTANDING,  0, NULL },
    {0, "Dram Wr Outs",  PERF_SELECT_FABRIC_DRAM_EVENT   , PERF_MASK_FABRIC_WR_OUTSTANDING,  0, NULL },
    {0, "Dram Neg Evnt", PERF_SELECT_FABRIC_DRAM_EVENT   , PERF_MASK_FABRIC_NUM_NEG_COUNT,   0, NULL },
};

enum AvgLat_Set_Offsets
{
    AVGLAT_SET_SRAM_RD_REQS_OFFSET,
    AVGLAT_SET_SRAM_WR_REQS_OFFSET,
    AVGLAT_SET_SRAM_RD_OUTS_OFFSET,
    AVGLAT_SET_SRAM_WR_OUTS_OFFSET,
    AVGLAT_SET_PCIE_RD_REQS_OFFSET,
    AVGLAT_SET_PCIE_WR_REQS_OFFSET,
    AVGLAT_SET_PCIE_RD_OUTS_OFFSET,
    AVGLAT_SET_PCIE_WR_OUTS_OFFSET,
};

static TD_PERFMON_PC_LIST_ENTRY td_fabric_latency_set_base[] =
{
    {0, "Sram Rd Reqs",   PERF_SELECT_FABRIC_SRAM_EVENT   , PERF_MASK_FABRIC_RD_REQS,         0, NULL },
    {0, "Sram Wr Reqs",   PERF_SELECT_FABRIC_SRAM_EVENT   , PERF_MASK_FABRIC_WR_REQS,         0, NULL },
    {0, "Sram Rd Outs",   PERF_SELECT_FABRIC_SRAM_EVENT   , PERF_MASK_FABRIC_RD_OUTSTANDING,  0, NULL },
    {0, "Sram Wr Outs",   PERF_SELECT_FABRIC_SRAM_EVENT   , PERF_MASK_FABRIC_WR_OUTSTANDING,  0, NULL },
    {0, "Pcie Rd Reqs",   PERF_SELECT_FABRIC_PCIE_EVENT   , PERF_MASK_FABRIC_RD_REQS,         0, NULL },
    {0, "Pcie Wr Reqs",   PERF_SELECT_FABRIC_PCIE_EVENT   , PERF_MASK_FABRIC_WR_REQS,         0, NULL },
    {0, "Pcie Rd Outs",   PERF_SELECT_FABRIC_PCIE_EVENT   , PERF_MASK_FABRIC_RD_OUTSTANDING,  0, NULL },
    {0, "Pcie Wr Outs",   PERF_SELECT_FABRIC_PCIE_EVENT   , PERF_MASK_FABRIC_WR_OUTSTANDING,  0, NULL },
};

enum Cdma_Set_Offsets
{
    CDMA_SET_CDMA0_CYCLES_OFFSET,
    CDMA_SET_CDMA0_DMA_ENQ_OFFSET,
    CDMA_SET_CDMA0_LDDATA_BEAT_OFFSET,
    CDMA_SET_CDMA0_STDATA_BEAT_OFFSET,
    CDMA_SET_CDMA1_CYCLES_OFFSET,
    CDMA_SET_CDMA1_DMA_ENQ_OFFSET,
    CDMA_SET_CDMA1_LDDATA_BEAT_OFFSET,
    CDMA_SET_CDMA1_STDATA_BEAT_OFFSET,
};

//
// All entries in node event list are *REQUIRED* to have the same #nodeID
//
#define NODE_CDMA_SET_LIST(n)                                          \
{                                                                      \
{n , "Cdma0 Cycles"      , PERF_SELECT_NODE_CDMA0_EVENT, PERF_MASK_CDMA_ALWAYS_ONE           , 0, NULL }, \
{n , "Cdma0 Dma Enq"     , PERF_SELECT_NODE_CDMA0_EVENT, PERF_MASK_CDMA_DMA_ENQUED_EVENT     , 0, NULL }, \
{n , "Cdma0 Ld_Data Beat", PERF_SELECT_NODE_CDMA0_EVENT, PERF_MASK_CDMA_LOAD_DATA_BEAT_EVENT , 0, NULL }, \
{n , "Cdma0 St_Data_Beat", PERF_SELECT_NODE_CDMA0_EVENT, PERF_MASK_CDMA_STORE_DATA_BEAT_EVENT, 0, NULL }, \
{n , "Cdma1 Cycles"      , PERF_SELECT_NODE_CDMA1_EVENT, PERF_MASK_CDMA_ALWAYS_ONE           , 0, NULL }, \
{n , "Cdma1 Dma Enq"     , PERF_SELECT_NODE_CDMA1_EVENT, PERF_MASK_CDMA_DMA_ENQUED_EVENT     , 0, NULL }, \
{n , "Cdma1 Ld_Data Beat", PERF_SELECT_NODE_CDMA1_EVENT, PERF_MASK_CDMA_LOAD_DATA_BEAT_EVENT , 0, NULL }, \
{n , "Cdma1 St_Data_Beat", PERF_SELECT_NODE_CDMA1_EVENT, PERF_MASK_CDMA_STORE_DATA_BEAT_EVENT, 0, NULL }, \
}
static TD_PERFMON_PC_LIST_ENTRY td_node00_cdma_set_base[] = NODE_CDMA_SET_LIST(0);  // SFP0
static TD_PERFMON_PC_LIST_ENTRY td_node01_cdma_set_base[] = NODE_CDMA_SET_LIST(1);  // SFP1
static TD_PERFMON_PC_LIST_ENTRY td_node02_cdma_set_base[] = NODE_CDMA_SET_LIST(2);  // SFP2
static TD_PERFMON_PC_LIST_ENTRY td_node03_cdma_set_base[] = NODE_CDMA_SET_LIST(3);  // SFP3
static TD_PERFMON_PC_LIST_ENTRY td_node04_cdma_set_base[] = NODE_CDMA_SET_LIST(4);  // SFP4
static TD_PERFMON_PC_LIST_ENTRY td_node05_cdma_set_base[] = NODE_CDMA_SET_LIST(5);  // SFP5
static TD_PERFMON_PC_LIST_ENTRY td_node06_cdma_set_base[] = NODE_CDMA_SET_LIST(6);  // SFP6

static TD_PERFMON_PC_LIST_ENTRY td_node07_cdma_set_base[] = NODE_CDMA_SET_LIST(7);  // VFP0
static TD_PERFMON_PC_LIST_ENTRY td_node08_cdma_set_base[] = NODE_CDMA_SET_LIST(8);  // VFP1
static TD_PERFMON_PC_LIST_ENTRY td_node09_cdma_set_base[] = NODE_CDMA_SET_LIST(9);  // VFP2
static TD_PERFMON_PC_LIST_ENTRY td_node10_cdma_set_base[] = NODE_CDMA_SET_LIST(10); // VFP3
static TD_PERFMON_PC_LIST_ENTRY td_node11_cdma_set_base[] = NODE_CDMA_SET_LIST(11); // VFP4
#undef NODE_CDMA_SET_LIST

enum Classifier_Set_Offsets
{
    CLASSIFIER_SET_CLSFR0_CYCLES_OFFSET,
    CLASSIFIER_SET_CLSFR0_PIXELCNT_OFFSET,
    CLASSIFIER_SET_CLSFR0_NODECNT_OFFSET,
    CLASSIFIER_SET_CLSFR0_BUSY_OFFSET,
    CLASSIFIER_SET_CLSFR1_CYCLES_OFFSET,
    CLASSIFIER_SET_CLSFR1_PIXELCNT_OFFSET,
    CLASSIFIER_SET_CLSFR1_NODECNT_OFFSET,
    CLASSIFIER_SET_CLSFR1_BUSY_OFFSET,
};

//
// All entries in node event list are *REQUIRED* to have the same #nodeID
//
#define NODE_CLASSIFIER_SET_LIST(n)                                          \
{                                                                   \
{n , "Clsfr0 Cycles"   , PERF_SELECT_NODE_CLASSIFIER0_EVENT, PERF_MASK_CLASSIFIER_ALWAYS_ONE        , 0, NULL }, \
{n , "Clsfr0 Pixel Cnt", PERF_SELECT_NODE_CLASSIFIER0_EVENT, PERF_MASK_CLASSIFIER_PERF_PIXELCNT     , 0, NULL }, \
{n , "Clsfr0 Node Cnt" , PERF_SELECT_NODE_CLASSIFIER0_EVENT, PERF_MASK_CLASSIFIER_PERF_NODECNT      , 0, NULL }, \
{n , "Clsfr0 Busy",      PERF_SELECT_NODE_CLASSIFIER0_EVENT, PERF_MASK_CLASSIFIER_CHAN_BUSY         , 0, NULL }, \
{n , "Clsfr1 Cycles"   , PERF_SELECT_NODE_CLASSIFIER1_EVENT, PERF_MASK_CLASSIFIER_ALWAYS_ONE        , 0, NULL }, \
{n , "Clsfr1 Pixel Cnt", PERF_SELECT_NODE_CLASSIFIER1_EVENT, PERF_MASK_CLASSIFIER_PERF_PIXELCNT     , 0, NULL }, \
{n , "Clsfr1 Node Cnt" , PERF_SELECT_NODE_CLASSIFIER1_EVENT, PERF_MASK_CLASSIFIER_PERF_NODECNT      , 0, NULL }, \
{n , "Clsfr1 Busy",      PERF_SELECT_NODE_CLASSIFIER1_EVENT, PERF_MASK_CLASSIFIER_CHAN_BUSY         , 0, NULL }, \
}
static TD_PERFMON_PC_LIST_ENTRY td_node00_classifier_set_base[] = NODE_CLASSIFIER_SET_LIST(0);  // SFP0
static TD_PERFMON_PC_LIST_ENTRY td_node01_classifier_set_base[] = NODE_CLASSIFIER_SET_LIST(1);  // SFP1
static TD_PERFMON_PC_LIST_ENTRY td_node02_classifier_set_base[] = NODE_CLASSIFIER_SET_LIST(2);  // SFP2
static TD_PERFMON_PC_LIST_ENTRY td_node03_classifier_set_base[] = NODE_CLASSIFIER_SET_LIST(3);  // SFP3
static TD_PERFMON_PC_LIST_ENTRY td_node04_classifier_set_base[] = NODE_CLASSIFIER_SET_LIST(4);  // SFP4
static TD_PERFMON_PC_LIST_ENTRY td_node05_classifier_set_base[] = NODE_CLASSIFIER_SET_LIST(5);  // SFP5
static TD_PERFMON_PC_LIST_ENTRY td_node06_classifier_set_base[] = NODE_CLASSIFIER_SET_LIST(6);  // SFP6
#undef NODE_CLASSIFIER_SET_LIST

//
// ISP base events - each ISP has 4 counters
//
#define ISP_BASE_SET_LIST                       \
{                                               \
    {0, "Isp Cycles",      PERF_SELECT_ISP_ISP_EVENT, PERF_MASK_ISP_ALWAYS_ONE,                        0, NULL}, \
    {0, "Sensor Pixels",   PERF_SELECT_ISP_ISP_EVENT, PERF_MASK_ISP_ISP_SENSOR_INTERFACE_IMAGE_PIXELS, 0, NULL}, \
    {0, "Sib Sof",         PERF_SELECT_ISP_ISP_EVENT, PERF_MASK_ISP_SIB_SOF,                           0, NULL}, \
}
static TD_PERFMON_PC_LIST_ENTRY td_hana0_isp_set_base[] = ISP_BASE_SET_LIST;  // HANA0
static TD_PERFMON_PC_LIST_ENTRY td_kula0_isp_set_base[] = ISP_BASE_SET_LIST;  // KULA0
static TD_PERFMON_PC_LIST_ENTRY td_kula1_isp_set_base[] = ISP_BASE_SET_LIST;  // KULA1
static TD_PERFMON_PC_LIST_ENTRY td_pala0_isp_set_base[] = ISP_BASE_SET_LIST;  // PALA0
static TD_PERFMON_PC_LIST_ENTRY td_pala1_isp_set_base[] = ISP_BASE_SET_LIST;  // PALA1
static TD_PERFMON_PC_LIST_ENTRY td_pala2_isp_set_base[] = ISP_BASE_SET_LIST;  // PALA2
static TD_PERFMON_PC_LIST_ENTRY td_pala3_isp_set_base[] = ISP_BASE_SET_LIST;  // PALA3
#undef ISP_BASE_SET_LIST

//
// ISP base events - each ISP has 4 counters
//
#define ISP_PE_SET_LIST                       \
{                                               \
    {0, "Pe Input Frames", PERF_SELECT_ISP_PE_EVENT, PERF_MASK_PE_FRAME_I_PERF_EVENT,                0, NULL}, \
}
static TD_PERFMON_PC_LIST_ENTRY td_hana0_pe_set_base[] = ISP_PE_SET_LIST;  // HANA0
static TD_PERFMON_PC_LIST_ENTRY td_pala0_pe_set_base[] = ISP_PE_SET_LIST;  // PALA0
static TD_PERFMON_PC_LIST_ENTRY td_pala1_pe_set_base[] = ISP_PE_SET_LIST;  // PALA1
static TD_PERFMON_PC_LIST_ENTRY td_pala2_pe_set_base[] = ISP_PE_SET_LIST;  // PALA2
static TD_PERFMON_PC_LIST_ENTRY td_pala3_pe_set_base[] = ISP_PE_SET_LIST;  // PALA3
#undef ISP_PE_SET_LIST

#endif // REDUCED_MEMORY_FOOTPRINT

//
// Helper functions used to calculate derived metrics from base metrics
// Takes event list and returns UINT64
//

UINT64 fabric_calc_dram_rd_bw (const TD_PERFMON_PC_LIST_ENTRY* const pEventList)
{
    if (pEventList[BANDWIDTH_SET_CYCLES_OFFSET].pSaveData->currSample)
    {
        return (1000000 * pEventList[BANDWIDTH_SET_DRAM_RD_WORDS_OFFSET].pSaveData->currSample / pEventList[BANDWIDTH_SET_CYCLES_OFFSET].pSaveData->currSample);
    }
    else
    {
        return 0ull;
    }
}

UINT64 fabric_calc_dram_wr_bw (const TD_PERFMON_PC_LIST_ENTRY* const pEventList)
{
    if (pEventList[BANDWIDTH_SET_CYCLES_OFFSET].pSaveData->currSample)
    {
        return (1000000 * pEventList[BANDWIDTH_SET_DRAM_WR_WORDS_OFFSET].pSaveData->currSample / pEventList[BANDWIDTH_SET_CYCLES_OFFSET].pSaveData->currSample);
    }
    else
    {
        return 0ull;
    }
}

UINT64 fabric_calc_sram_rd_bw (const TD_PERFMON_PC_LIST_ENTRY* const pEventList)
{
    if (pEventList[BANDWIDTH_SET_CYCLES_OFFSET].pSaveData->currSample)
    {
        return (1000000 * pEventList[BANDWIDTH_SET_SRAM_RD_WORDS_OFFSET].pSaveData->currSample / pEventList[BANDWIDTH_SET_CYCLES_OFFSET].pSaveData->currSample);
    }
    else
    {
        return 0ull;
    }
}

UINT64 fabric_calc_sram_wr_bw (const TD_PERFMON_PC_LIST_ENTRY* const pEventList)
{
    if (pEventList[BANDWIDTH_SET_CYCLES_OFFSET].pSaveData->currSample)
    {   
        return (1000000 * pEventList[BANDWIDTH_SET_SRAM_WR_WORDS_OFFSET].pSaveData->currSample / pEventList[BANDWIDTH_SET_CYCLES_OFFSET].pSaveData->currSample);
    }
    else
    {
        return 0ull;
    }
}

UINT64 fabric_calc_pcie_rd_bw (const TD_PERFMON_PC_LIST_ENTRY* const pEventList)
{
    if (pEventList[BANDWIDTH_SET_CYCLES_OFFSET].pSaveData->currSample)
    {
        return (1000000 * pEventList[BANDWIDTH_SET_PCIE_RD_WORDS_OFFSET].pSaveData->currSample / pEventList[BANDWIDTH_SET_CYCLES_OFFSET].pSaveData->currSample);
    }
    else
    {
        return 0ull;
    }
}

UINT64 fabric_calc_pcie_wr_bw (const TD_PERFMON_PC_LIST_ENTRY* const pEventList)
{
    if (pEventList[BANDWIDTH_SET_CYCLES_OFFSET].pSaveData->currSample)
    {    
        return (1000000 * pEventList[BANDWIDTH_SET_PCIE_WR_WORDS_OFFSET].pSaveData->currSample / pEventList[BANDWIDTH_SET_CYCLES_OFFSET].pSaveData->currSample);
    }
    else
    {
        return 0ull;
    }
}

UINT64 fabric_calc_total_rd_bw (const TD_PERFMON_PC_LIST_ENTRY* const pEventList)
{
    if (pEventList[BANDWIDTH_SET_CYCLES_OFFSET].pSaveData->currSample)
    {
        return (1000000 * (
                    pEventList[BANDWIDTH_SET_DRAM_RD_WORDS_OFFSET].pSaveData->currSample +
                    pEventList[BANDWIDTH_SET_SRAM_RD_WORDS_OFFSET].pSaveData->currSample +
                    pEventList[BANDWIDTH_SET_PCIE_RD_WORDS_OFFSET].pSaveData->currSample
                    ) / pEventList[BANDWIDTH_SET_CYCLES_OFFSET].pSaveData->currSample);
    }
    else
    {
        return 0ull;
    }
}

UINT64 fabric_calc_total_wr_bw (const TD_PERFMON_PC_LIST_ENTRY* const pEventList)
{
    if (pEventList[BANDWIDTH_SET_CYCLES_OFFSET].pSaveData->currSample)
    {    
        return (1000000 * (
                    pEventList[BANDWIDTH_SET_DRAM_WR_WORDS_OFFSET].pSaveData->currSample +
                    pEventList[BANDWIDTH_SET_SRAM_WR_WORDS_OFFSET].pSaveData->currSample +
                    pEventList[BANDWIDTH_SET_PCIE_WR_WORDS_OFFSET].pSaveData->currSample                
                    ) / pEventList[BANDWIDTH_SET_CYCLES_OFFSET].pSaveData->currSample);
    }
    else
    {
        return 0ull;
    }
}

#ifndef REDUCED_MEMORY_FOOTPRINT

UINT64 fabric_calc_pghit_ratio (const TD_PERFMON_PC_LIST_ENTRY* const pEventList)
{
    if (pEventList[MEMMAX_SET_PGHIT_OFFSET].pSaveData->currSample  || 
        pEventList[MEMMAX_SET_PGMISS_OFFSET].pSaveData->currSample ||
        pEventList[MEMMAX_SET_PGCFLT_OFFSET].pSaveData->currSample)
    {
        return (100 * pEventList[MEMMAX_SET_PGHIT_OFFSET].pSaveData->currSample / 
                (pEventList[MEMMAX_SET_PGHIT_OFFSET].pSaveData->currSample +
                 pEventList[MEMMAX_SET_PGMISS_OFFSET].pSaveData->currSample +
                 pEventList[MEMMAX_SET_PGCFLT_OFFSET].pSaveData->currSample
                    ));
    }
    else
    {
        return 0ull;
    }
}

UINT64 fabric_calc_pgmiss_ratio (const TD_PERFMON_PC_LIST_ENTRY* const pEventList)
{
    if (pEventList[MEMMAX_SET_PGHIT_OFFSET].pSaveData->currSample  || 
        pEventList[MEMMAX_SET_PGMISS_OFFSET].pSaveData->currSample ||
        pEventList[MEMMAX_SET_PGCFLT_OFFSET].pSaveData->currSample)
    {
        return (100 * pEventList[MEMMAX_SET_PGMISS_OFFSET].pSaveData->currSample / 
                (pEventList[MEMMAX_SET_PGHIT_OFFSET].pSaveData->currSample +
                 pEventList[MEMMAX_SET_PGMISS_OFFSET].pSaveData->currSample +
                 pEventList[MEMMAX_SET_PGCFLT_OFFSET].pSaveData->currSample
                    ));
    }
    else
    {
        return 0ull;
    }
}

UINT64 fabric_calc_pgcflt_ratio (const TD_PERFMON_PC_LIST_ENTRY* const pEventList)
{
    if (pEventList[MEMMAX_SET_PGHIT_OFFSET].pSaveData->currSample  || 
        pEventList[MEMMAX_SET_PGMISS_OFFSET].pSaveData->currSample ||
        pEventList[MEMMAX_SET_PGCFLT_OFFSET].pSaveData->currSample)
    {
        return (100 * pEventList[MEMMAX_SET_PGCFLT_OFFSET].pSaveData->currSample / 
                (pEventList[MEMMAX_SET_PGHIT_OFFSET].pSaveData->currSample +
                 pEventList[MEMMAX_SET_PGMISS_OFFSET].pSaveData->currSample +
                 pEventList[MEMMAX_SET_PGCFLT_OFFSET].pSaveData->currSample
                    ));
    }
    else
    {
        return 0ull;
    }
}

UINT64 fabric_calc_dram_rd_lat (const TD_PERFMON_PC_LIST_ENTRY* const pEventList)
{
    if (pEventList[MEMMAX_SET_DRAM_RD_REQS_OFFSET].pSaveData->currSample)
    {
        return (FABRIC_CLK_IN_NS * pEventList[MEMMAX_SET_DRAM_RD_OUTS_OFFSET].pSaveData->currSample 
                / pEventList[MEMMAX_SET_DRAM_RD_REQS_OFFSET].pSaveData->currSample);
    }
    else
    {
        return 0ull;
    }
}

UINT64 fabric_calc_dram_wr_lat (const TD_PERFMON_PC_LIST_ENTRY* const pEventList)
{
    if (pEventList[MEMMAX_SET_DRAM_WR_REQS_OFFSET].pSaveData->currSample)
    {
        return (FABRIC_CLK_IN_NS * pEventList[MEMMAX_SET_DRAM_WR_OUTS_OFFSET].pSaveData->currSample 
                / pEventList[MEMMAX_SET_DRAM_WR_REQS_OFFSET].pSaveData->currSample);
    }
    else
    {
        return 0ull;
    }
}

UINT64 fabric_calc_sram_rd_lat (const TD_PERFMON_PC_LIST_ENTRY* const pEventList)
{
    if (pEventList[AVGLAT_SET_SRAM_RD_REQS_OFFSET].pSaveData->currSample)
    {
        return (FABRIC_CLK_IN_NS * pEventList[AVGLAT_SET_SRAM_RD_OUTS_OFFSET].pSaveData->currSample 
                / pEventList[AVGLAT_SET_SRAM_RD_REQS_OFFSET].pSaveData->currSample);
    }
    else
    {
        return 0ull;
    }
}

UINT64 fabric_calc_sram_wr_lat (const TD_PERFMON_PC_LIST_ENTRY* const pEventList)
{
    if (pEventList[AVGLAT_SET_SRAM_WR_REQS_OFFSET].pSaveData->currSample)
    {
        return (FABRIC_CLK_IN_NS * pEventList[AVGLAT_SET_SRAM_WR_OUTS_OFFSET].pSaveData->currSample 
                / pEventList[AVGLAT_SET_SRAM_WR_REQS_OFFSET].pSaveData->currSample);
    }
    else
    {
        return 0ull;
    }
}

UINT64 fabric_calc_pcie_rd_lat (const TD_PERFMON_PC_LIST_ENTRY* const pEventList)
{
    if (pEventList[AVGLAT_SET_PCIE_RD_REQS_OFFSET].pSaveData->currSample)
    {
        return (FABRIC_CLK_IN_NS * pEventList[AVGLAT_SET_PCIE_RD_OUTS_OFFSET].pSaveData->currSample 
                / pEventList[AVGLAT_SET_PCIE_RD_REQS_OFFSET].pSaveData->currSample);
    }
    else
    {
        return 0ull;
    }
}

UINT64 fabric_calc_pcie_wr_lat (const TD_PERFMON_PC_LIST_ENTRY* const pEventList)
{
    if (pEventList[AVGLAT_SET_PCIE_WR_REQS_OFFSET].pSaveData->currSample)
    {
        return (FABRIC_CLK_IN_NS * pEventList[AVGLAT_SET_PCIE_WR_OUTS_OFFSET].pSaveData->currSample 
                / pEventList[AVGLAT_SET_PCIE_WR_REQS_OFFSET].pSaveData->currSample);
    }
    else
    {
        return 0ull;
    }
}

UINT64 node_calc_cdma0_ld_bw (const TD_PERFMON_PC_LIST_ENTRY *const pEventList)
{
    if (pEventList[CDMA_SET_CDMA0_CYCLES_OFFSET].pSaveData->currSample)
    {
        return (1000000 * 4 * pEventList[CDMA_SET_CDMA0_LDDATA_BEAT_OFFSET].pSaveData->currSample / 
                (pEventList[CDMA_SET_CDMA0_CYCLES_OFFSET].pSaveData->currSample * NODE_CLK_IN_NS));
    }
    else
    {
        return 0ull;
    }
}

UINT64 node_calc_cdma0_st_bw (const TD_PERFMON_PC_LIST_ENTRY *const pEventList)
{
    if (pEventList[CDMA_SET_CDMA0_CYCLES_OFFSET].pSaveData->currSample)
    {
        return (1000000 * 4 * pEventList[CDMA_SET_CDMA0_STDATA_BEAT_OFFSET].pSaveData->currSample / 
                (pEventList[CDMA_SET_CDMA0_CYCLES_OFFSET].pSaveData->currSample * NODE_CLK_IN_NS));
    }
    else
    {
        return 0ull;
    }
}

UINT64 node_calc_cdma1_ld_bw (const TD_PERFMON_PC_LIST_ENTRY *const pEventList)
{
    if (pEventList[CDMA_SET_CDMA1_CYCLES_OFFSET].pSaveData->currSample)
    {
        return (1000000 * 4 * pEventList[CDMA_SET_CDMA1_LDDATA_BEAT_OFFSET].pSaveData->currSample / 
                (pEventList[CDMA_SET_CDMA1_CYCLES_OFFSET].pSaveData->currSample * NODE_CLK_IN_NS));
    }
    else
    {
        return 0ull;
    }
}

UINT64 node_calc_cdma1_st_bw (const TD_PERFMON_PC_LIST_ENTRY *const pEventList)
{
    if (pEventList[CDMA_SET_CDMA1_CYCLES_OFFSET].pSaveData->currSample)
    {
        return (1000000 * 4 * pEventList[CDMA_SET_CDMA1_STDATA_BEAT_OFFSET].pSaveData->currSample / 
                (pEventList[CDMA_SET_CDMA1_CYCLES_OFFSET].pSaveData->currSample * NODE_CLK_IN_NS));
    }
    else
    {
        return 0ull;
    }
}

#endif // REDUCED_MEMORY_FOOTPRINT

//
// Lists of derived metrics for each event set
//

#define FABRIC_BANDWIDTH_DERIVED_SET_LIST(s)                     \
{                                                                 \
    {s "Dram Rd Bw (KBps)",  &fabric_calc_dram_rd_bw,  NULL },\
    {s "Dram Wr Bw (KBps)",  &fabric_calc_dram_wr_bw,  NULL },\
    {s "Sram Rd Bw (KBps)",  &fabric_calc_sram_rd_bw,  NULL },\
    {s "Sram Wr Bw (KBps)",  &fabric_calc_sram_wr_bw,  NULL },\
    {s "Pcie Rd Bw (KBps)",  &fabric_calc_pcie_rd_bw,  NULL },\
    {s "Pcie Wr Bw (KBps)",  &fabric_calc_pcie_wr_bw,  NULL },\
    {s "Total Rd Bw (KBps)", &fabric_calc_total_rd_bw, NULL },\
    {s "Total Wr Bw (KBps)", &fabric_calc_total_wr_bw, NULL },\
}

static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_all_bandwidth_set_derived[]  = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Chip ");

#ifndef REDUCED_MEMORY_FOOTPRINT
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_sfp0_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Sfp0 ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_sfp1_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Sfp1 ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_sfp2_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Sfp2 ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_sfp3_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Sfp3 ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_sfp4_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Sfp4 ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_sfp5_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Sfp5 ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_sfp6_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Sfp6 ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_vfp0_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Vfp0 ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_vfp1_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Vfp1 ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_vfp2_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Vfp2 ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_vfp3_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Vfp3 ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_vfp4_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Vfp4 ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_han0_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Han0 ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_kul0_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Kul0 ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_kul1_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Kul1 ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_pal0_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Pal0 ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_pal1_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Pal1 ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_pal2_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Pal2 ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_pal3_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Pal3 ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_pcie_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Pcie ");
static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_gdma_bandwidth_set_derived[] = FABRIC_BANDWIDTH_DERIVED_SET_LIST ("Gdma ");

static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_memmax_set_derived[] =
{
    {"Memmax Pghit  %", &fabric_calc_pghit_ratio,  NULL},
    {"Memmax Pgmiss %", &fabric_calc_pgmiss_ratio, NULL},
    {"Memmax Pgcflt %", &fabric_calc_pgcflt_ratio, NULL},
    {"Dram Avg Rd Lat", &fabric_calc_dram_rd_lat,  NULL},
    {"Dram Avg Wr Lat", &fabric_calc_dram_wr_lat,  NULL},
};

static TD_PERFMON_DERIVED_PC_ENTRY td_fabric_latency_set_derived[] =
{
    {"Sram Avg Rd Lat", &fabric_calc_sram_rd_lat,  NULL},
    {"Sram Avg Wr Lat", &fabric_calc_sram_wr_lat,  NULL},
    {"Pcie Avg Rd Lat", &fabric_calc_pcie_rd_lat,  NULL},
    {"Pcie Avg Wr Lat", &fabric_calc_pcie_wr_lat,  NULL},
};

#define NODE_CDMA_DERIVED_EVENT_LIST                 \
{                                               \
    {"Cdma0 Ld Bw (KBps)", &node_calc_cdma0_ld_bw, NULL}, \
    {"Cdma0 St Bw (KBps)", &node_calc_cdma0_st_bw, NULL}, \
    {"Cdma1 Ld Bw (KBps)", &node_calc_cdma1_ld_bw, NULL}, \
    {"Cdma1 St Bw (KBps)", &node_calc_cdma1_st_bw, NULL}, \
}

static TD_PERFMON_DERIVED_PC_ENTRY td_node00_cdma_set_derived [] = NODE_CDMA_DERIVED_EVENT_LIST; // SFP0
static TD_PERFMON_DERIVED_PC_ENTRY td_node01_cdma_set_derived [] = NODE_CDMA_DERIVED_EVENT_LIST; // SFP1
static TD_PERFMON_DERIVED_PC_ENTRY td_node02_cdma_set_derived [] = NODE_CDMA_DERIVED_EVENT_LIST; // SFP2
static TD_PERFMON_DERIVED_PC_ENTRY td_node03_cdma_set_derived [] = NODE_CDMA_DERIVED_EVENT_LIST; // SFP3
static TD_PERFMON_DERIVED_PC_ENTRY td_node04_cdma_set_derived [] = NODE_CDMA_DERIVED_EVENT_LIST; // SFP4
static TD_PERFMON_DERIVED_PC_ENTRY td_node05_cdma_set_derived [] = NODE_CDMA_DERIVED_EVENT_LIST; // SFP5
static TD_PERFMON_DERIVED_PC_ENTRY td_node06_cdma_set_derived [] = NODE_CDMA_DERIVED_EVENT_LIST; // SFP6
                                                                                                   
static TD_PERFMON_DERIVED_PC_ENTRY td_node07_cdma_set_derived [] = NODE_CDMA_DERIVED_EVENT_LIST; // VFP0
static TD_PERFMON_DERIVED_PC_ENTRY td_node08_cdma_set_derived [] = NODE_CDMA_DERIVED_EVENT_LIST; // VFP1
static TD_PERFMON_DERIVED_PC_ENTRY td_node09_cdma_set_derived [] = NODE_CDMA_DERIVED_EVENT_LIST; // VFP2
static TD_PERFMON_DERIVED_PC_ENTRY td_node10_cdma_set_derived [] = NODE_CDMA_DERIVED_EVENT_LIST; // VFP3
static TD_PERFMON_DERIVED_PC_ENTRY td_node11_cdma_set_derived [] = NODE_CDMA_DERIVED_EVENT_LIST; // VFP4

//
// List of all event sets that will be scheduled together
//

static TD_PERFMON_EVENT_SET td_fabric_events[] =
{
#if (PERFMON_FABRIC_ENABLE_BW_SET > 0)
    {td_fabric_all_bandwidth_set_base ,  td_fabric_all_bandwidth_set_derived, COUNTOF(td_fabric_all_bandwidth_set_base) , COUNTOF(td_fabric_all_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_SFP0_SET > 0)
    {td_fabric_sfp0_bandwidth_set_base,  td_fabric_sfp0_bandwidth_set_derived, COUNTOF(td_fabric_sfp0_bandwidth_set_base) , COUNTOF(td_fabric_sfp0_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_SFP1_SET > 0)
    {td_fabric_sfp1_bandwidth_set_base,  td_fabric_sfp1_bandwidth_set_derived, COUNTOF(td_fabric_sfp1_bandwidth_set_base) , COUNTOF(td_fabric_sfp1_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_SFP2_SET > 0)
    {td_fabric_sfp2_bandwidth_set_base,  td_fabric_sfp2_bandwidth_set_derived, COUNTOF(td_fabric_sfp2_bandwidth_set_base) , COUNTOF(td_fabric_sfp2_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_SFP3_SET > 0)
    {td_fabric_sfp3_bandwidth_set_base,  td_fabric_sfp3_bandwidth_set_derived, COUNTOF(td_fabric_sfp3_bandwidth_set_base) , COUNTOF(td_fabric_sfp3_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_SFP4_SET > 0)
    {td_fabric_sfp4_bandwidth_set_base,  td_fabric_sfp4_bandwidth_set_derived, COUNTOF(td_fabric_sfp4_bandwidth_set_base) , COUNTOF(td_fabric_sfp4_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_SFP5_SET > 0)
    {td_fabric_sfp5_bandwidth_set_base,  td_fabric_sfp5_bandwidth_set_derived, COUNTOF(td_fabric_sfp5_bandwidth_set_base) , COUNTOF(td_fabric_sfp5_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_SFP6_SET > 0)
    {td_fabric_sfp6_bandwidth_set_base,  td_fabric_sfp6_bandwidth_set_derived, COUNTOF(td_fabric_sfp6_bandwidth_set_base) , COUNTOF(td_fabric_sfp6_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_VFP0_SET > 0)
    {td_fabric_vfp0_bandwidth_set_base,  td_fabric_vfp0_bandwidth_set_derived, COUNTOF(td_fabric_vfp0_bandwidth_set_base) , COUNTOF(td_fabric_vfp0_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_VFP1_SET > 0)
    {td_fabric_vfp1_bandwidth_set_base,  td_fabric_vfp1_bandwidth_set_derived, COUNTOF(td_fabric_vfp1_bandwidth_set_base) , COUNTOF(td_fabric_vfp1_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_VFP2_SET > 0)
    {td_fabric_vfp2_bandwidth_set_base,  td_fabric_vfp2_bandwidth_set_derived, COUNTOF(td_fabric_vfp2_bandwidth_set_base) , COUNTOF(td_fabric_vfp2_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_VFP3_SET > 0)
    {td_fabric_vfp3_bandwidth_set_base,  td_fabric_vfp3_bandwidth_set_derived, COUNTOF(td_fabric_vfp3_bandwidth_set_base) , COUNTOF(td_fabric_vfp3_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_VFP4_SET > 0)
    {td_fabric_vfp4_bandwidth_set_base,  td_fabric_vfp4_bandwidth_set_derived, COUNTOF(td_fabric_vfp4_bandwidth_set_base) , COUNTOF(td_fabric_vfp4_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_HAN0_SET > 0)
    {td_fabric_han0_bandwidth_set_base,  td_fabric_han0_bandwidth_set_derived, COUNTOF(td_fabric_han0_bandwidth_set_base) , COUNTOF(td_fabric_han0_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_KUL0_SET > 0)
    {td_fabric_kul0_bandwidth_set_base,  td_fabric_kul0_bandwidth_set_derived, COUNTOF(td_fabric_kul0_bandwidth_set_base) , COUNTOF(td_fabric_kul0_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_KUL1_SET > 0)
    {td_fabric_kul1_bandwidth_set_base,  td_fabric_kul1_bandwidth_set_derived, COUNTOF(td_fabric_kul1_bandwidth_set_base) , COUNTOF(td_fabric_kul1_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_PAL0_SET > 0)
    {td_fabric_pal0_bandwidth_set_base,  td_fabric_pal0_bandwidth_set_derived, COUNTOF(td_fabric_pal0_bandwidth_set_base) , COUNTOF(td_fabric_pal0_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_PAL1_SET > 0)
    {td_fabric_pal1_bandwidth_set_base,  td_fabric_pal1_bandwidth_set_derived, COUNTOF(td_fabric_pal1_bandwidth_set_base) , COUNTOF(td_fabric_pal1_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_PAL2_SET > 0)
    {td_fabric_pal2_bandwidth_set_base,  td_fabric_pal2_bandwidth_set_derived, COUNTOF(td_fabric_pal2_bandwidth_set_base) , COUNTOF(td_fabric_pal2_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_PAL3_SET > 0)
    {td_fabric_pal3_bandwidth_set_base,  td_fabric_pal3_bandwidth_set_derived, COUNTOF(td_fabric_pal3_bandwidth_set_base) , COUNTOF(td_fabric_pal3_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_PCIE_SET > 0)
    {td_fabric_pcie_bandwidth_set_base,  td_fabric_pcie_bandwidth_set_derived, COUNTOF(td_fabric_pcie_bandwidth_set_base) , COUNTOF(td_fabric_pcie_bandwidth_set_derived) },
#endif
#if (!defined PERFMON_FILTER_ENABLED) && (PERFMON_FABRIC_ENABLE_GDMA_SET > 0)
    {td_fabric_gdma_bandwidth_set_base,  td_fabric_gdma_bandwidth_set_derived, COUNTOF(td_fabric_gdma_bandwidth_set_base) , COUNTOF(td_fabric_gdma_bandwidth_set_derived) },
#endif
#if (PERFMON_FABRIC_ENABLE_MEMMAX_SET > 0)
    {td_fabric_memmax_set_base     ,  td_fabric_memmax_set_derived,    COUNTOF(td_fabric_memmax_set_base)    , COUNTOF(td_fabric_memmax_set_derived)    },
#endif
#if (PERFMON_FABRIC_ENABLE_LATENCY_SET > 0)
    {td_fabric_latency_set_base    ,  td_fabric_latency_set_derived,   COUNTOF(td_fabric_latency_set_base)   , COUNTOF(td_fabric_latency_set_derived)   },
#endif
};

#if (PERFMON_NODE_ENABLE_CDMA_SET > 0)
#define CDMA_SET_ENTRY(TD_NODE) {TD_NODE ## _cdma_set_base, TD_NODE ## _cdma_set_derived, COUNTOF(TD_NODE ## _cdma_set_base), COUNTOF(TD_NODE ## _cdma_set_derived)},
#else
#define CDMA_SET_ENTRY(TD_NODE)
#endif

#if (PERFMON_NODE_ENABLE_CLASSIFIER_SET > 0)
#define CLASSIFIER_SET_ENTRY(TD_NODE) {TD_NODE ## _classifier_set_base, NULL, COUNTOF(TD_NODE ## _classifier_set_base), 0},
#else
#define CLASSIFIER_SET_ENTRY(TD_NODE)
#endif

#define SFP_NODE_EVENTSET_LIST(TD_NODE)  \
{                                        \
    CDMA_SET_ENTRY(TD_NODE)              \
    CLASSIFIER_SET_ENTRY(TD_NODE)        \
}
#define VFP_NODE_EVENTSET_LIST(TD_NODE) \
{                                       \
    CDMA_SET_ENTRY(TD_NODE)             \
}                                                           

static TD_PERFMON_EVENT_SET td_node00_events[] = SFP_NODE_EVENTSET_LIST(td_node00);
static TD_PERFMON_EVENT_SET td_node01_events[] = SFP_NODE_EVENTSET_LIST(td_node01);
static TD_PERFMON_EVENT_SET td_node02_events[] = SFP_NODE_EVENTSET_LIST(td_node02);
static TD_PERFMON_EVENT_SET td_node03_events[] = SFP_NODE_EVENTSET_LIST(td_node03);
static TD_PERFMON_EVENT_SET td_node04_events[] = SFP_NODE_EVENTSET_LIST(td_node04);
static TD_PERFMON_EVENT_SET td_node05_events[] = SFP_NODE_EVENTSET_LIST(td_node05);
static TD_PERFMON_EVENT_SET td_node06_events[] = SFP_NODE_EVENTSET_LIST(td_node06);

static TD_PERFMON_EVENT_SET td_node07_events[] = VFP_NODE_EVENTSET_LIST(td_node07);
static TD_PERFMON_EVENT_SET td_node08_events[] = VFP_NODE_EVENTSET_LIST(td_node08);
static TD_PERFMON_EVENT_SET td_node09_events[] = VFP_NODE_EVENTSET_LIST(td_node09);
static TD_PERFMON_EVENT_SET td_node10_events[] = VFP_NODE_EVENTSET_LIST(td_node10);
static TD_PERFMON_EVENT_SET td_node11_events[] = VFP_NODE_EVENTSET_LIST(td_node11);
#undef CDMA_SET_ENTRY
#undef CLASSIFIER_SET_ENTRY
#undef SFP_NODE_EVENTSET_LIST
#undef VFP_NODE_EVENTSET_LIST

#define ISP_EVENTSET_LIST_PE(TD_ISP)                  \
{                                                  \
    {TD_ISP ## _isp_set_base, NULL, COUNTOF(TD_ISP ## _isp_set_base), 0},       \
    {TD_ISP ## _pe_set_base,  NULL, COUNTOF(TD_ISP ## _pe_set_base),  0},       \
}
#define ISP_EVENTSET_LIST(TD_ISP)                  \
{                                                  \
    {TD_ISP ## _isp_set_base, NULL, COUNTOF(TD_ISP ## _isp_set_base), 0},       \
}
static TD_PERFMON_EVENT_SET td_hana0_events[] = ISP_EVENTSET_LIST_PE(td_hana0);
static TD_PERFMON_EVENT_SET td_kula0_events[] = ISP_EVENTSET_LIST(td_kula0);
static TD_PERFMON_EVENT_SET td_kula1_events[] = ISP_EVENTSET_LIST(td_kula1);
static TD_PERFMON_EVENT_SET td_pala0_events[] = ISP_EVENTSET_LIST_PE(td_pala0);
static TD_PERFMON_EVENT_SET td_pala1_events[] = ISP_EVENTSET_LIST_PE(td_pala1);
static TD_PERFMON_EVENT_SET td_pala2_events[] = ISP_EVENTSET_LIST_PE(td_pala2);
static TD_PERFMON_EVENT_SET td_pala3_events[] = ISP_EVENTSET_LIST_PE(td_pala3);
#undef ISP_EVENTSET_LIST_PE
#undef ISP_EVENTSET_LIST

// Counter directory
//
static TD_PERFMON_PC_DIR_ENTRY td_perfmon_dir[] =
{
#if (PERFMON_ENABLE_FABRIC_EVENTS > 0)
    {PERFGRP_FABRIC,    "FABRIC_EVENT", td_fabric_events, COUNTOF(td_fabric_events), 0, 0, true },
#endif
#if (PERFMON_ENABLE_NODE00_EVENTS > 0)
    {PERFGRP_NODE,      "NODE00_EVENT", td_node00_events, COUNTOF(td_node00_events), 0, 0, true },
#endif    
#if (PERFMON_ENABLE_NODE01_EVENTS > 0)
    {PERFGRP_NODE,      "NODE01_EVENT", td_node01_events, COUNTOF(td_node01_events), 0, 0, true },
#endif
#if (PERFMON_ENABLE_NODE02_EVENTS > 0)
    {PERFGRP_NODE,      "NODE02_EVENT", td_node02_events, COUNTOF(td_node02_events), 0, 0, true },
#endif
#if (PERFMON_ENABLE_NODE03_EVENTS > 0)
    {PERFGRP_NODE,      "NODE03_EVENT", td_node03_events, COUNTOF(td_node03_events), 0, 0, true },
#endif
#if (PERFMON_ENABLE_NODE04_EVENTS > 0)
    {PERFGRP_NODE,      "NODE04_EVENT", td_node04_events, COUNTOF(td_node04_events), 0, 0, true },
#endif
#if (PERFMON_ENABLE_NODE05_EVENTS > 0)
    {PERFGRP_NODE,      "NODE05_EVENT", td_node05_events, COUNTOF(td_node05_events), 0, 0, true },
#endif
#if (PERFMON_ENABLE_NODE06_EVENTS > 0)
    {PERFGRP_NODE,      "NODE06_EVENT", td_node06_events, COUNTOF(td_node06_events), 0, 0, true },
#endif
#if (PERFMON_ENABLE_NODE07_EVENTS > 0)
    {PERFGRP_NODE,      "NODE07_EVENT", td_node07_events, COUNTOF(td_node07_events), 0, 0, true },
#endif
#if (PERFMON_ENABLE_NODE08_EVENTS > 0)
    {PERFGRP_NODE,      "NODE08_EVENT", td_node08_events, COUNTOF(td_node08_events), 0, 0, true },
#endif
#if (PERFMON_ENABLE_NODE09_EVENTS > 0)
    {PERFGRP_NODE,      "NODE09_EVENT", td_node09_events, COUNTOF(td_node09_events), 0, 0, true },
#endif
#if (PERFMON_ENABLE_NODE10_EVENTS > 0)
    {PERFGRP_NODE,      "NODE10_EVENT", td_node10_events, COUNTOF(td_node10_events), 0, 0, true },
#endif
#if (PERFMON_ENABLE_NODE11_EVENTS > 0)
    {PERFGRP_NODE,      "NODE11_EVENT", td_node11_events, COUNTOF(td_node11_events), 0, 0, true },
#endif
#if (PERFMON_ENABLE_HANA0_EVENTS > 0)
    {PERFGRP_ISP_HANA0, "HANA0_EVENT",  td_hana0_events,  COUNTOF(td_hana0_events),  0, 0, true },
#endif
#if (PERFMON_ENABLE_KULA0_EVENTS > 0)
    {PERFGRP_ISP_KULA0, "KULA0_EVENT",  td_kula0_events,  COUNTOF(td_kula0_events),  0, 0, true },
#endif
#if (PERFMON_ENABLE_KULA1_EVENTS > 0)
    {PERFGRP_ISP_KULA1, "KULA1_EVENT",  td_kula1_events,  COUNTOF(td_kula1_events),  0, 0, true },
#endif
#if (PERFMON_ENABLE_PALA0_EVENTS > 0)
    {PERFGRP_ISP_PALA0, "PALA0_EVENT",  td_pala0_events,  COUNTOF(td_pala0_events),  0, 0, true },
#endif
#if (PERFMON_ENABLE_PALA1_EVENTS > 0)
    {PERFGRP_ISP_PALA1, "PALA1_EVENT",  td_pala1_events,  COUNTOF(td_pala1_events),  0, 0, true },
#endif
#if (PERFMON_ENABLE_PALA2_EVENTS > 0)
    {PERFGRP_ISP_PALA2, "PALA2_EVENT",  td_pala2_events,  COUNTOF(td_pala2_events),  0, 0, true },
#endif
#if (PERFMON_ENABLE_PALA3_EVENTS > 0)
    {PERFGRP_ISP_PALA3, "PALA3_EVENT",  td_pala3_events,  COUNTOF(td_pala3_events),  0, 0, true },
#endif
};

#else  // REDUCED_MEMORY_FOOTPRINT

static TD_PERFMON_EVENT_SET td_fabric_events[] =
{
#if (PERFMON_FABRIC_ENABLE_BW_SET > 0)
    {td_fabric_all_bandwidth_set_base ,  td_fabric_all_bandwidth_set_derived, COUNTOF(td_fabric_all_bandwidth_set_base) , COUNTOF(td_fabric_all_bandwidth_set_derived) },
#endif
};

static TD_PERFMON_PC_DIR_ENTRY td_perfmon_dir[] =
{
#if (PERFMON_ENABLE_FABRIC_EVENTS > 0)
    {PERFGRP_FABRIC,    "FABRIC_EVENT", td_fabric_events, COUNTOF(td_fabric_events), 0, 0, true },
#endif
};
#endif // REDUCED_MEMORY_FOOTPRINT


void setFabricFilter (UINT32 mask, PMU_TARGET target)
{
    int num_mstrs = BITCOUNT(mask);
    hw_assert (num_mstrs < 16);
    int cur_fltr_idx = 0;
    
    for (int bit_index=0; bit_index < PMU_MSTRID_INVALID; bit_index++)
    {
        if ((mask >> bit_index) & 1)
        {
            hw_assert (PMU_FLTRIDX_0 + cur_fltr_idx < PMU_FLTRIDX_MAX);
            perfmon_fabricSetFilter (target, PMU_FLTRID_FILTER0, PMU_FLTRIDX (PMU_FLTRIDX_0 + cur_fltr_idx), (PMU_MSTRID) bit_index);
            cur_fltr_idx++;
        }
    }     
    perfmon_fabricSetDontPass (target, PMU_FLTRID_FILTER0, false);
}

static inline void clearPerfmonData (TD_PERFMON_DATA *pData)
{
    pData->minSample  = ~0ull;
    pData->maxSample  =  0ull;
    pData->cumValue   =  0ull;
    pData->numSamples =  0ull;
    pData->currSample =  0ull;
}

//
// Assert global-stop for all counter blocks and read out
// current values and store them in a data structure
//
void ProcessCurrentInterval (UINT64 cur_time)
{    

    //UINT64 funcStart = hw_getTime();

    // Apply global stop
    perfmon_applyGlobalStop ();

    // Read out counters and store in saveData

    for (UINT32 dirIdx = 0; dirIdx < COUNTOF(td_perfmon_dir); dirIdx++)
    {
        if (!td_perfmon_dir[dirIdx].enabled)
            continue;

        UINT32 numHwCounters = perfmon_getNumHwCounters (td_perfmon_dir[dirIdx].groupType);
        
        UINT32 cntrNum = 0;

        while (cntrNum < numHwCounters)
        {
            UINT32 nextEventSet           = td_perfmon_dir[dirIdx].readPtr;
            TD_PERFMON_EVENT_SET eventSet = td_perfmon_dir[dirIdx].pEventSets[nextEventSet];

            if (eventSet.baseSize > numHwCounters)
            {
                hw_errmsg ("%s: assert fired -> (eventSet.baseSize(%d) <= numHwCounters(%d)) for dirIdx = %d, nextEventSet = %d\n", __func__, eventSet.baseSize, numHwCounters, dirIdx, nextEventSet);
                hw_assert (eventSet.baseSize <= numHwCounters);
            }

            if ((numHwCounters - cntrNum) < eventSet.baseSize)
            {
                break;
            }

            for (UINT32 eventNum = 0; eventNum < eventSet.baseSize; eventNum++)
            {
                TD_PERFMON_PC_LIST_ENTRY perfEvent = eventSet.pEventList[eventNum];
                TD_PERFMON_DATA *perfData          = perfEvent.pSaveData;
            
                UINT64 cntrVal = perfmon_getPerfCounter (perfEvent.nodeNum, 
                                                         td_perfmon_dir[dirIdx].groupType,
                                                         cntrNum);
                
                BOOL success = perfmon_disablePerfCounter (perfEvent.nodeNum, 
                                                           td_perfmon_dir[dirIdx].groupType,
                                                           cntrNum);
                if (!success)
                {
                    hw_errmsg ("%s: Error while trying to disable counter %d, dirIdx = %d, nextEventSet = %d, event = %s\n", __func__, cntrNum, dirIdx, nextEventSet, perfEvent.eventDesc);
                    continue;
                }
                
                perfData->numSamples++;
                perfData->cumValue += cntrVal;
                perfData->currSample = cntrVal;

                if (cntrVal < perfData->minSample)
                {
                    perfData->minSample = cntrVal;
                }
                if (cntrVal > perfData->maxSample)
                {
                    perfData->maxSample = cntrVal;
                }

                cntrNum++;
            }

            if (eventSet.pDerivedList)
            {
                for (UINT32 eventNum = 0; eventNum < eventSet.derivedSize; eventNum++)
                {
                    TD_PERFMON_DATA *perfData = eventSet.pDerivedList[eventNum].pSaveData;
                    UINT64 (* const calc_function) (const TD_PERFMON_PC_LIST_ENTRY* const) = eventSet.pDerivedList[eventNum].pCalcFunction;
                    UINT64 derivedVal = calc_function (eventSet.pEventList);

                    perfData->numSamples++;
                    perfData->cumValue += derivedVal;
                    perfData->currSample = derivedVal;
                    
                    if (derivedVal < perfData->minSample)
                    {
                        perfData->minSample = derivedVal;
                    }
                    if (derivedVal > perfData->maxSample)
                    {
                        perfData->maxSample = derivedVal;
                    }                                        
                }
            }

            td_perfmon_dir[dirIdx].readPtr = (td_perfmon_dir[dirIdx].readPtr + 1) % (td_perfmon_dir[dirIdx].listSize);
        }
    }

    //UINT64 funcEnd = hw_getTime();
    //hw_status ("Exiting %s: took %lld ns\n", __func__, funcEnd - funcStart);
}

//
//  Report statistics for the current epoch and clear out data structure
//  Data could possibly contain multiple samples of each event
//
void DumpCurrentEpoch (UINT64 cur_time)
{

    //UINT64 funcStart = hw_getTime();

    hw_printf((HW_PRINT_TYPE) PERFMON_PRINT_LEVEL, "td_perfmon :: <<==============================|| BEGIN PERFMON ||==============================>>\n");   

    hw_printf((HW_PRINT_TYPE) PERFMON_PRINT_LEVEL, "td_perfmon :: %-14s | %-20s | %-3s | %-10s | %-10s | %-10s\n", "PERF_BLOCK", "EVENT_NAME", "CNT", "MIN_VAL", "MAX_VAL", "AVG_VAL");

    hw_printf((HW_PRINT_TYPE) PERFMON_PRINT_LEVEL, "td_perfmon :: -----------------------------------------------------------------------------------\n");

    for (UINT32 dirIdx = 0; dirIdx < COUNTOF(td_perfmon_dir); dirIdx++)
    {

        if (!td_perfmon_dir[dirIdx].enabled)
            continue;

        for (UINT32 listIdx = 0; listIdx < td_perfmon_dir[dirIdx].listSize; listIdx++)
        {
            TD_PERFMON_EVENT_SET eventSet = td_perfmon_dir[dirIdx].pEventSets[listIdx];

            for (UINT32 setIdx = 0; setIdx < eventSet.baseSize; setIdx++)
            {

                TD_PERFMON_PC_LIST_ENTRY perfEvent = eventSet.pEventList[setIdx];
                TD_PERFMON_DATA         *perfData  = perfEvent.pSaveData;
                
                if (perfData->numSamples > 0 && perfData->maxSample != 0)
                {
                    hw_printf((HW_PRINT_TYPE) PERFMON_PRINT_LEVEL, "td_perfmon :: %-14s | %-25s | %-3d | %-10lld | %-10lld | %-10lld\n", td_perfmon_dir[dirIdx].prefixStr, perfEvent.eventDesc, perfData->numSamples, perfData->minSample, perfData->maxSample, perfData->cumValue/perfData->numSamples);
                }
                
                clearPerfmonData (perfData);
            }

            if (eventSet.pDerivedList)
            {
                for (UINT32 setIdx = 0; setIdx < eventSet.derivedSize; setIdx++)
                {
                    TD_PERFMON_DERIVED_PC_ENTRY perfEvent = eventSet.pDerivedList[setIdx];
                    TD_PERFMON_DATA            *perfData  = perfEvent.pSaveData;
                    
                    if (perfData->numSamples > 0 && perfData->maxSample != 0)
                    {
                        hw_printf((HW_PRINT_TYPE) PERFMON_PRINT_LEVEL, "td_perfmon :: %-14s | %-25s | %-3d | %-10lld | %-10lld | %-10lld\n", td_perfmon_dir[dirIdx].prefixStr, perfEvent.eventDesc, perfData->numSamples, perfData->minSample, perfData->maxSample, perfData->cumValue/perfData->numSamples);
                    }
                    
                    clearPerfmonData (perfData);
                }
            }
        }

        hw_printf((HW_PRINT_TYPE) PERFMON_PRINT_LEVEL, "td_perfmon :: -----------------------------------------------------------------------------------\n");

    }

    hw_printf((HW_PRINT_TYPE) PERFMON_PRINT_LEVEL, "td_perfmon :: <<==============================||  END PERFMON  ||==============================>>\n");

    //UINT64 funcEnd = hw_getTime();
    //hw_status ("Exiting %s: took %lld ns\n", __func__, funcEnd - funcStart);
}

//
// Schedule events for the next monitoring interval and re-enable
// all counter blocks
// 
void PrepareNextInterval ()
{    

    //UINT64 funcStart = hw_getTime();

    // First configure all perf counters

    for (UINT32 dirIdx = 0; dirIdx < COUNTOF(td_perfmon_dir); dirIdx++)
    {
        
        if (!td_perfmon_dir[dirIdx].enabled)
            continue;
        
        UINT32 numHwCounters = perfmon_getNumHwCounters (td_perfmon_dir[dirIdx].groupType);

        UINT32 cntrNum = 0;
        while (cntrNum < numHwCounters)
        {            
            UINT32 nextEventSet           = td_perfmon_dir[dirIdx].writePtr;
            TD_PERFMON_EVENT_SET eventSet = td_perfmon_dir[dirIdx].pEventSets[nextEventSet];
            BOOL success;

            if (eventSet.baseSize > numHwCounters)
            {
                hw_errmsg ("%s: assert fired -> (eventSet.baseSize(%d) <= numHwCounters(%d)) for dirIdx = %d, nextEventSet = %d\n", __func__, eventSet.baseSize, numHwCounters, dirIdx, nextEventSet);
                hw_assert (eventSet.baseSize <= numHwCounters);
            }

            if ((numHwCounters - cntrNum) < eventSet.baseSize)
            {
                break;
            }

            for (UINT32 eventNum = 0; eventNum < eventSet.baseSize; eventNum++)
            {
                TD_PERFMON_PC_LIST_ENTRY perfEvent = eventSet.pEventList[eventNum];

                success = perfmon_configWithFeatures (perfEvent.nodeNum, 
                                                      td_perfmon_dir[dirIdx].groupType,
                                                      cntrNum,
                                                      perfEvent.eventSel,
                                                      perfEvent.eventMask,
                                                      FEATURE_GLOBAL_STOP | FEATURE_FABRIC_RST_ON_NEG);

                // If filter is specified, process the mask and set the filter indices
#if (PERFMON_ENABLE_FABRIC_EVENTS > 0) && (defined PERFMON_FABRIC_ENABLE_DRAM_FILTER)
                setFabricFilter (PERFMON_FABRIC_ENABLE_DRAM_FILTER, PMU_TARGET_DRAM);
#endif
                
#if (PERFMON_ENABLE_FABRIC_EVENTS > 0) && (defined PERFMON_FABRIC_ENABLE_SRAM_FILTER)
                setFabricFilter (PERFMON_FABRIC_ENABLE_SRAM_FILTER, PMU_TARGET_SRAM);
#endif
                
#if (PERFMON_ENABLE_FABRIC_EVENTS > 0) && (defined PERFMON_FABRIC_ENABLE_PCIE_FILTER)
                setFabricFilter (PERFMON_FABRIC_ENABLE_PCIE_FILTER, PMU_TARGET_PCIE);
#endif

                if (perfEvent.filterMask)
                {
                    setFabricFilter (perfEvent.filterMask, PMU_TARGET_DRAM);
                    setFabricFilter (perfEvent.filterMask, PMU_TARGET_SRAM);
                    setFabricFilter (perfEvent.filterMask, PMU_TARGET_PCIE);
                };

                success &= perfmon_enablePerfCounter (perfEvent.nodeNum,
                                                      td_perfmon_dir[dirIdx].groupType,
                                                      cntrNum);
                
                if (! success)
                {
                    hw_errmsg ("%s: Error while configuring perf counter %d, dirIdx = %d, eventSet = %d, event = %s\n", __func__, dirIdx, nextEventSet, perfEvent.eventDesc);
                }

                cntrNum++;
            }

            td_perfmon_dir[dirIdx].writePtr = (td_perfmon_dir[dirIdx].writePtr + 1) % (td_perfmon_dir[dirIdx].listSize);

//            hw_status ("%s: For %s:%s, scheduling on cntr#%d :: updating writePtr from %d to %d\n", __func__, td_perfmon_dir[dirIdx].prefixStr, perfEvent.eventDesc, cntrNum, nextEventPtr, td_perfmon_dir[dirIdx].writePtr);
        }
    }

    // Then enable them by releasing global stop
    perfmon_releaseGlobalStop();
    
    //UINT64 funcEnd = hw_getTime();
    //hw_status ("Exiting %s: took %lld ns\n", __func__, funcEnd - funcStart);
}

//
// Initialize data structures
//
extern void td_perfmon_init ()
{
    // Make sure that print interval is greater than COLLECT interval and is evenly divisible
    static_assert (PERFMON_REPORT_INTERVAL >= PERFMON_COLLECT_INTERVAL);
    static_assert (PERFMON_REPORT_INTERVAL % PERFMON_COLLECT_INTERVAL == 0);  

    for (UINT32 idx = 0; idx < COUNTOF (td_perfmon_dir); idx++)
    {
        if ((td_perfmon_dir[idx].groupType == PERFGRP_NODE) && td_perfmon_dir[idx].pEventSets && td_perfmon_dir[idx].pEventSets[0].pEventList)
        {
            // Check just the first event list entry

            if (! (hw_activeCoreList[(2 * td_perfmon_dir[idx].pEventSets[0].pEventList[0].nodeNum)] ||
                   hw_activeCoreList[(2 * td_perfmon_dir[idx].pEventSets[0].pEventList[0].nodeNum) + 1]))
            {                
                // If both cores in node are inactive, do not enable dir entry
                td_perfmon_dir[idx].enabled = false;
            }
        }

        if (td_perfmon_dir[idx].enabled)
        {            
            for (UINT32 setIdx = 0; setIdx < td_perfmon_dir[idx].listSize; setIdx++)
            {
                for (UINT32 evnt = 0; evnt < td_perfmon_dir[idx].pEventSets[setIdx].baseSize; evnt++)
                {
                    td_perfmon_dir[idx].pEventSets[setIdx].pEventList[evnt].pSaveData = (TD_PERFMON_DATA *) hw_alloc (sizeof(TD_PERFMON_DATA));
                    clearPerfmonData (td_perfmon_dir[idx].pEventSets[setIdx].pEventList[evnt].pSaveData);
                }

                for (UINT32 evnt = 0; evnt < td_perfmon_dir[idx].pEventSets[setIdx].derivedSize; evnt++)
                {
                    td_perfmon_dir[idx].pEventSets[setIdx].pDerivedList[evnt].pSaveData = (TD_PERFMON_DATA *) hw_alloc (sizeof(TD_PERFMON_DATA));
                    clearPerfmonData (td_perfmon_dir[idx].pEventSets[setIdx].pDerivedList[evnt].pSaveData);
                }
            }
        }
    }

}

//
// Main update function that gets called from the 
// driver run loop
// 

extern void td_perfmon_update (UINT64 cur_time)
{    

    UINT64 funcStart = hw_getTime();

    // If counters are enabled - read the current ones
    if (interval_cnt > 0)
    {
        ProcessCurrentInterval (cur_time);
    }

    // If PERFMON_REPORT_INTERVAL has elapsed, dump currently programmed counters
    if (prev_time)
    {
        if ((cur_time - prev_time) > PERFMON_REPORT_INTERVAL)
        {
            DumpCurrentEpoch (cur_time);
            prev_time = cur_time;
        }
    }
    else
    {
        prev_time = cur_time;
    }

    // Setup counters for next interval
    PrepareNextInterval();    

    interval_cnt++;

    UINT64 funcEnd = hw_getTime();
    //hw_printf ("%s (iter%d): took %lld ns (start=%lld, end=%lld), (cur_time = %lld)\n", __func__, interval_cnt, funcEnd - funcStart, funcStart, funcEnd, cur_time);
}
#endif

