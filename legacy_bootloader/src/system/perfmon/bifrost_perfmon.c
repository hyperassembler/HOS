/*-----------------------------------------------------
  |      bifrost_perfmon.c
  |
  |    Source file for the performance monitor library
  |
  |------------------------------------------------------
  |
  |    Copyright (C) 2013  Microsoft Corporation
  |    All Rights Reserved
  |    Confidential and Proprietary
  |
  |------------------------------------------------------
*/

#include "bifrost.h"
#include <bifrost_lock.h>
#include "hup_chip.h"

// Flag to control DEBUG prints
// Uncomment if you want hw_perfmon_debug messages
//#define BIFROST_PERFMON_DEBUG

//
// Globals
//

#ifdef REDUCED_MEMORY_FOOTPRINT
// Performance monitoring is disabled in REDUCED_MEMORY_FOOTPRINT
// mode because of memory limitations
static perfmon_config perf_cur_state [0][0][0];
#else
static perfmon_config perf_cur_state [HW_SUPERNODE_CNT][NUM_PERFGRP][MAX_PERF_COUNTERS];
#endif

static BOOL nodeInitialized [HW_SUPERNODE_CNT] = {false};
static BOOL clsramInitialized = FALSE;

static BOOL* getNodeInit(UINT32 node, PERF_GROUP group)
{
    BOOL* ret = NULL;
    
    if (group <= PERFGRP_NODE)
    {
        ret = &nodeInitialized[node];
    }
    else
    {
        switch(group)
        {
        case PERFGRP_CLSRAM:
            ret = &clsramInitialized;
            break;
        default:
            break;
        }
    }
    
    return ret;
}

static UINT32 isp_block_base_addr [] = {
    HUP_CHIP_MIP0_ADDRESS,
    HUP_CHIP_MIP1_ADDRESS,
    HUP_CHIP_MIP2_ADDRESS,
    HUP_CHIP_MIP3_ADDRESS,
    HUP_CHIP_MIP4_ADDRESS,
    HUP_CHIP_MIP5_ADDRESS,
    HUP_CHIP_MIP6_ADDRESS,
    HUP_CHIP_TIP_ADDRESS
};

static UINT32 tip_subblock_mctrl_addr [] = {
    HUP_ISP_TIP_DPP_BASE_OFFSET + HUP_ISP_TIP_DPP_BASE_PERF_MCTRL_OFFSET,
    HUP_ISP_TIP_TFP_BASE_OFFSET + HUP_ISP_TIP_TFP_BASE_PERF_MCTRL_OFFSET,
    HUP_ISP_TIP_MDMA_BASE_OFFSET + HUP_ISP_TIP_MDMA_BASE_PERF_MCTRL_OFFSET,
    HUP_ISP_TIP_TIP_PRV_BASE_OFFSET + HUP_ISP_TIP_TIP_PRV_BASE_PERF_MCTRL_OFFSET
};

static UINT32 mip_subblock_mctrl_addr [] = {
    HUP_ISP_MIP_DPP_BASE_OFFSET + HUP_ISP_MIP_DPP_BASE_PERF_MCTRL_OFFSET,
    HUP_ISP_MIP_DDPC_BASE_OFFSET + HUP_ISP_MIP_DDPC_BASE_PERF_MCTRL_OFFSET,
    HUP_ISP_MIP_LSC_BASE_OFFSET + HUP_ISP_MIP_LSC_BASE_PERF_MCTRL_OFFSET,
    HUP_ISP_MIP_GAM_BASE_OFFSET + HUP_ISP_MIP_GAM_BASE_PERF_MCTRL_OFFSET,
    HUP_ISP_MIP_PE_BASE_OFFSET + HUP_ISP_MIP_PE_BASE_PERF_MCTRL_OFFSET,
    HUP_ISP_MIP_STATS_BASE_OFFSET + HUP_ISP_MIP_STATS_BASE_PERF_MCTRL_OFFSET,
    HUP_ISP_MIP_MDMA_BASE_OFFSET + HUP_ISP_MIP_MDMA_BASE_PERF_MCTRL_OFFSET,
    HUP_ISP_MIP_MIP_PRV_BASE_OFFSET + HUP_ISP_MIP_MIP_PRV_BASE_PERF_MCTRL_OFFSET
};

// Make sure this is in the same order as PERFGRP enum
static UINT32 fm_block_base_addr [] = {
    HUP_CHIP_FM_MC0_MC_00_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC0_MC_01_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC0_MC_02_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC0_MC_03_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC0_MC_04_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC0_MC_05_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC0_MC_06_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC0_MC_07_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC0_MC_08_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC0_MC_09_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC0_MC_10_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC0_MC_11_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC1_MC_00_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC1_MC_01_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC1_MC_02_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC1_MC_03_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC1_MC_04_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC1_MC_05_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC1_MC_06_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC1_MC_07_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC1_MC_08_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC1_MC_09_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC1_MC_10_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_MC1_MC_11_REGS_BYTE_ADDRESS,
    HUP_CHIP_FM_BASRAM0_BYTE_ADDRESS,
    HUP_CHIP_FM_BASRAM1_BYTE_ADDRESS,
    HUP_CHIP_FM_BASRAM2_BYTE_ADDRESS,
    HUP_CHIP_FM_PCIE_BYTE_ADDRESS,
};

//
// Lock for atomic perfmon usage
//
hw_spin_lock_t hw_perfmonLock = HW_LOCK_INITIALIZER;

//
// Wrapper around hw_reg_read32
//
static inline UINT32 dbg_read32 (const char *parent, UINT32 base, UINT32 offset)
{
#ifdef BIFROST_PERFMON_DEBUG
    hw_perfmon_debug ("%-30s: READ32  addr = %08x\n", parent, base + offset);
#endif
    UINT32 regval = hw_reg_read32 (base, offset);
    hw_serialize();
#ifdef BIFROST_PERFMON_DEBUG
    hw_perfmon_debug ("%-30s:         return_data = %08x\n", parent, regval);
#endif
    return regval;
}

//
// Wrapper around hw_reg_write32
//
static inline VOID   dbg_write32 (const char *parent, UINT32 base, UINT32 offset, UINT32 value)
{
#ifdef BIFROST_PERFMON_DEBUG
    hw_perfmon_debug ("%-30s: WRITE32 addr = %08x  write_data = %08x\n", parent, base + offset, value);
#endif
    hw_reg_write32 (base, offset, value);
    hw_serialize();
#ifdef BIFROST_PERFMON_DEBUG
    UINT32 regval = hw_reg_read32 (base, offset);
    hw_perfmon_debug ("%-30s:          return_data = %x\n", parent, regval);
#endif
}

//
// Return base address for given node (local or global)
//

static inline UINT32 get_node_base (UINT32 node, PERF_GROUP group)
{
    UINT32 node_base = 0;
    
    switch(group)
    {
    case PERFGRP_LX5_CORE0:
    case PERFGRP_LX5_CORE1:
    case PERFGRP_NODE:
        if (node == hw_getSuperNodeNum())
        {
            // Use local address
            node_base = HW_ADDR_NODE_LOCAL_BASE;
        }
        else
        {
            // Use global address
            node_base = HW_SYS_NODE_BASE + HW_SYS_NODE_SIZE * node;
        }
        break;
    case PERFGRP_CLSRAM:
        node_base = HUP_CHIP_CLSRAM_ADDRESS;
        break;
    default:
        break;
    }
    
    return node_base;
}

//
// Return perfmon base address for given node and core
//

static inline UINT32 get_lx5_perfmon_base (UINT32 node, PERF_GROUP group)
{
    UINT32 perfmon_base;

    if (group == PERFGRP_LX5_CORE0)
    {
        perfmon_base = get_node_base (node, group) + LX5_CORE0_REG_BASE;
    }
    else
    {
        perfmon_base = get_node_base (node, group) + LX5_CORE1_REG_BASE;
    }
    return perfmon_base;
}

//
// Return the supernode sub-block base address for given node and event select
//

static inline UINT32 get_node_block_base (UINT32 node, UINT32 event_select, PERF_GROUP group)
{
    UINT32 node_block_base;

    switch (event_select)
    {
        case PERF_SELECT_NODE_CDMA_EVENT:
            node_block_base = get_node_base (node, group) + HUP_CN_SFP_CD0_OFFSET;
            break;
        case PERF_SELECT_NODE_CLASSIFIER0_EVENT:
            node_block_base = get_node_base (node, group) + HUP_CN_SFP_CL_OFFSET;
            break;
        case PERF_SELECT_NODE_CLASSIFIER1_EVENT:
            node_block_base = get_node_base (node, group) + HUP_CN_SFP_CL_OFFSET + HUP_CL_CL_REGS_GROUP_BYTE_SIZE;
            break;
        case PERF_SELECT_NODE_XBAR_EVENT:
            node_block_base = get_node_base (node, group) + HUP_CN_SFP_XB_OFFSET;
            if(group == PERFGRP_CLSRAM)
            {
                node_block_base += HUP_CLSRAM_BASRAM_OFFSET;
            }
            break;
        default:
            hw_errmsg ("%s: Invalid event_select: %d\n", __func__, event_select);
            return (UINT32) -1;
    }

    return node_block_base;
}

//
// Update PERF_EVENT_MUX ;; FIXME: should be a MACRO?
//
static void update_pcnt_mux(const char *caller, UINT32 unit_base_addr, UINT32 perf_event_mux_offset,
                            UINT32 counter_num, UINT32 event_num)
{
    UINT32 perf_event_mux_data = dbg_read32 (caller, unit_base_addr, perf_event_mux_offset);

    UINT32 field_shift = (3-counter_num)*8;
    perf_event_mux_data   &= ~(0xff      << field_shift);
    perf_event_mux_data   |=  (event_num << field_shift);
    dbg_write32 (caller, unit_base_addr, perf_event_mux_offset, perf_event_mux_data);
}

//
// Utility functions for setting specified bit ;; FIXME
//
static void set_one_bit(const char *caller, UINT32 unit_base_addr, UINT32 reg_offset,
                        UINT32 bit_num)
{
    UINT32 reg_data     = dbg_read32 (caller, unit_base_addr, reg_offset);
    reg_data     |= 1 << bit_num;
    dbg_write32 (caller, unit_base_addr, reg_offset, reg_data);
}

//
// Utility functions for setting 2 specified bits ;; FIXME
//
static void set_two_bit(const char *caller, UINT32 unit_base_addr, UINT32 reg_offset,
                        UINT32 bit_num1, UINT32 bit_num2)
{
    UINT32 reg_data     = dbg_read32 (caller, unit_base_addr, reg_offset);
    reg_data     |= (1 << bit_num1) | (1<<bit_num2);
    dbg_write32 (caller, unit_base_addr, reg_offset, reg_data);
}

//
// Do common sanity checks for the methods below
//

static BOOL sanity_check_args (const char *parent, UINT32 node, PERF_GROUP group, UINT32 counter_num)
{
    // Sanity check args: node
    if (node >= HW_SUPERNODE_CNT)
    {
        hw_errmsg ("%s: Node ID %d is greater than HW_SUPERNODE_CNT (%d)\n", parent, node, HW_SUPERNODE_CNT);
        return false;
    }
    
    BOOL* nodeInitFlag = getNodeInit(node, group);
    
    // Turn on clocks if not yet done
    if (nodeInitFlag != NULL && !(*nodeInitFlag))
    {
        *nodeInitFlag = TRUE;
        
        UINT32 unit_base_addr        = get_node_base (node, group);
        
        //
        // De-assert Reset and enable performance counters
        //
        UINT32 cn_ctrl_reg = hw_reg_read32 (unit_base_addr, HUP_CN_MISC_CN_CTRL_OFFSET);
        
        // Enable Debug/Trace Unit & Perfmon clock
        cn_ctrl_reg |= HUP_CN_MISC_NCTRL_CN_DTB_EN_FIELD_MASK | HUP_CN_MISC_NCTRL_CN_PERF_EN_FIELD_MASK;
        
        // De-assert core0 and core1 debug logic reset
        cn_ctrl_reg &= ~(HUP_CN_MISC_NCTRL_CN_P0_DRESET_FIELD_MASK | HUP_CN_MISC_NCTRL_CN_P1_DRESET_FIELD_MASK);
        
        // De-assert core0 and core1 APB reset
        cn_ctrl_reg &= ~(HUP_CN_MISC_NCTRL_CN_P0_APB_PRESET_FIELD_MASK | HUP_CN_MISC_NCTRL_CN_P1_APB_PRESET_FIELD_MASK);
        
        hw_reg_write32 (unit_base_addr, HUP_CN_MISC_CN_CTRL_OFFSET, cn_ctrl_reg);
    }
    
    // Sanity check args: counter #
    if (counter_num > perfmon_getNumHwCounters (group))
    {
        hw_errmsg ("%s: Counter Num %d > Max hardware counters (%d) for this group (%d)\n", parent, counter_num, perfmon_getNumHwCounters (group), group);
        return false;
    }

    switch (group)
    {
        case PERFGRP_LX5_CORE0:
        case PERFGRP_LX5_CORE1:
        {
            // Sanity check args: coreId
            if (! hw_activeCoreList[node*2 + (int) (group-PERFGRP_LX5_CORE0)])
            {
                hw_errmsg ("%s: Target LX5 core %d not active\n", parent, node*2 + (int) (group - PERFGRP_LX5_CORE0));
                return false;
            }
            break;
        }
        case PERFGRP_NODE:
        case PERFGRP_INTC:
        case PERFGRP_JBL:
        case PERFGRP_FM_MC0_Q0:
        case PERFGRP_FM_MC0_Q1:
        case PERFGRP_FM_MC0_Q2:
        case PERFGRP_FM_MC0_Q3:
        case PERFGRP_FM_MC0_Q4:
        case PERFGRP_FM_MC0_Q5:
        case PERFGRP_FM_MC0_Q6:
        case PERFGRP_FM_MC0_Q7:
        case PERFGRP_FM_MC0_Q8:
        case PERFGRP_FM_MC0_Q9:
        case PERFGRP_FM_MC0_Q10:
        case PERFGRP_FM_MC0_Q11:
        case PERFGRP_FM_MC1_Q0:
        case PERFGRP_FM_MC1_Q1:
        case PERFGRP_FM_MC1_Q2:
        case PERFGRP_FM_MC1_Q3:
        case PERFGRP_FM_MC1_Q4:
        case PERFGRP_FM_MC1_Q5:
        case PERFGRP_FM_MC1_Q6:
        case PERFGRP_FM_MC1_Q7:
        case PERFGRP_FM_MC1_Q8:
        case PERFGRP_FM_MC1_Q9:
        case PERFGRP_FM_MC1_Q10:
        case PERFGRP_FM_MC1_Q11:
        case PERFGRP_FM_BASRAM0:
        case PERFGRP_FM_BASRAM1:
        case PERFGRP_FM_BASRAM2:
        case PERFGRP_FM_PCIE:
        case PERFGRP_CLSRAM:
        case PERFGRP_MIP0:
        case PERFGRP_MIP1:
        case PERFGRP_MIP2:
        case PERFGRP_MIP3:
        case PERFGRP_MIP4:
        case PERFGRP_MIP5:
        case PERFGRP_MIP6:
        case PERFGRP_TIP:
        case PERFGRP_DNNSRAM:
        case PERFGRP_DNN_250:
        case PERFGRP_DNN_500:
        case PERFGRP_MCU0:
        case PERFGRP_MCU1:
        case PERFGRP_IMP0:
        case PERFGRP_IMP1:
        case PERFGRP_LSR0_SLOW:
        case PERFGRP_LSR1_SLOW:
        case PERFGRP_LSR0_FAST:
        case PERFGRP_LSR1_FAST:
        {
            break;
        }
        /*{
            hw_errmsg ("%s: PERFGRP %d not implemented yet\n", parent, group);
            return false;
        }*/
        default:
            hw_errmsg ("%s: Invalid group type %d passed as an argument\n", parent, group);
            return false;
    }

    return true;
}

//
// Configure performance counter with specified event select
// and unit masks; Also clears counter register
// TODO: Handle global snapshot/stop mode
//
HW_STATUSCODE   perfmon_configWithFeatures (UINT32       node        ,
                                            PERF_GROUP   group       ,
                                            UINT32       counter_num ,
                                            UINT32       event_select,
                                            UINT32       event_mask  ,
                                            UINT32       feature_mask)
{

    if (group > PERFGRP_NODE)
    {
        // All global counters should use the same node value when updating state
        node = 0;
    }

    if (! sanity_check_args (__func__, node, group, counter_num))
    {
        return PERFMON_STATUS_SANITY_FAIL;
    }    

    HW_CORE currentOwner = (HW_CORE) hw_getCoreNum();    

    if (perf_cur_state [node][(int) group][counter_num].in_use && perf_cur_state[node][(int) group][counter_num].owner != currentOwner)
    {
        hw_errmsg ("%s: Core %d requesting Counter (node=%d, group=%d, cntr#=%d) which is in use by another core %d\n", __func__, currentOwner, node, group, counter_num, perf_cur_state[node][(int) group][counter_num].owner);
        return PERFMON_STATUS_COUNTER_LOCKED;
    }

    hw_printf(HW_PERFMON_DEBUG, "\n%s : Passed sanity check.\n", __func__);
    switch (group)
    {
        case PERFGRP_LX5_CORE0:
        case PERFGRP_LX5_CORE1:
        {
            UINT32 perfmon_base;
            UINT32 ctrl_reg_offset;
            UINT32 cntr_reg_offset;
            UINT32 ctrl_reg_data;

            perfmon_base = get_lx5_perfmon_base (node, group);
            ctrl_reg_offset = PERFMON_LX5_PMCTRL0_OFFSET + (counter_num * 4);
            cntr_reg_offset = PERFMON_LX5_PM0_OFFSET     + (counter_num * 4);

            // Setup ctrl_reg_data
            ctrl_reg_data = SET_LX5_PERF_CTRL_REG (event_select, event_mask);

            // Reset cntr register
            dbg_write32 (__func__, perfmon_base, cntr_reg_offset, 0);

            // Write ctrl register
            dbg_write32 (__func__, perfmon_base, ctrl_reg_offset, ctrl_reg_data);

            if (feature_mask != FEATURE_NONE) 
            {
                hw_errmsg ("%s: No advanced features supported yet for LX5 core counters\n", __func__);
                hw_errmsg ("%s: Please use FEATURE_NONE\n", __func__);
                return PERFMON_STATUS_FEATURE_UNSUPPORTED;
            }

            break;
        }
        case PERFGRP_CLSRAM:
            switch (event_select)
            {
                case PERF_SELECT_NODE_CLASSIFIER0_EVENT:
                case PERF_SELECT_NODE_CLASSIFIER1_EVENT:
                case PERF_SELECT_NODE_XBAR_EVENT:
                    break;
                default:
                {
                    hw_errmsg ("%s: Event select %d is not valid for PERFGRP_CLSRAM\n", __func__, event_select);
                    hw_errmsg ("%s: Use one of PERF_SELECT_NODE_* defines in perf_events.h\n", __func__);
                    return PERFMON_STATUS_EVENTSEL_INVALID;
                }
            }
            // Fall-through
        case PERFGRP_NODE:
        {
            switch (event_select)
            {
                case PERF_SELECT_NODE_CLASSIFIER0_EVENT:
                case PERF_SELECT_NODE_CLASSIFIER1_EVENT:
                    // The classifier has a couple of special enables for a couple of event.
                    // FIXME: We probably should shut these off at some point.
                    if (event_mask == PERF_MASK_CLASSIFIER_PERF_PIXELCNT) {
                        // Enable the event : perfpix_en of mode register
                        // hw_debug("Enabling perfpix_en\n");
                        UINT32 unit_base_addr        = get_node_block_base (node, event_select, group);
                        set_one_bit(__func__, unit_base_addr, HUP_CL_CL_REGS_GROUP_MODE_OFFSET, HUP_CL_MODE_PERFPIX_EN_LSB);
                    }
                    else if (event_mask == PERF_MASK_CLASSIFIER_PERF_NODECNT) {
                        // Enable the event : perfnode_en of mode register
                        // hw_debug("Enabling perfnode_en\n");
                        UINT32 unit_base_addr        = get_node_block_base (node, event_select, group);
                        set_one_bit(__func__, unit_base_addr, HUP_CL_CL_REGS_GROUP_MODE_OFFSET, HUP_CL_MODE_PERFNODE_EN_LSB);
                    }

                    // Fall thru...
                case PERF_SELECT_NODE_CDMA_EVENT:
                case PERF_SELECT_NODE_XBAR_EVENT:
                {
                    UINT32 unit_base_addr;
                    UINT32 perf_event_mux_offset;
                    UINT32 perf_event_mux_data;
                    UINT32 perf_cntr_ovrride;
                    UINT32 gbl_snpshot_en;
                    UINT32 gbl_stop_en;

                    // Clear previous configuration
                    if (perf_cur_state [node][(int) group][counter_num].valid &&
                        (perf_cur_state [node][(int) group][counter_num].event_select != event_select))
                    {
                        unit_base_addr        = get_node_block_base (node, perf_cur_state [node][(int) group][counter_num].event_select, group);

                        perf_event_mux_offset = (counter_num < 4) ? HUP_CD_PERF_MCTRL0_OFFSET : HUP_CD_PERF_MCTRL1_OFFSET;

                        perf_event_mux_data   = dbg_read32 (__func__, unit_base_addr, perf_event_mux_offset);

                        perf_event_mux_data   = SET_NODE_EVENT_MUX_CTRL (perf_event_mux_data, counter_num, 0);

                        dbg_write32 (__func__, unit_base_addr, perf_event_mux_offset, perf_event_mux_data);
                    }

                    // Set event mask in unit level mux control
                    unit_base_addr        = get_node_block_base (node, event_select, group);

                    perf_event_mux_offset = (counter_num < 4) ? HUP_CD_PERF_MCTRL0_OFFSET : HUP_CD_PERF_MCTRL1_OFFSET;

                    perf_event_mux_data   = dbg_read32 (__func__, unit_base_addr, perf_event_mux_offset);

                    perf_event_mux_data   = SET_NODE_EVENT_MUX_CTRL (perf_event_mux_data, counter_num, event_mask);

                    dbg_write32 (__func__, unit_base_addr, perf_event_mux_offset, perf_event_mux_data);
                    
                    // Apply global snapshot & global stop settings based on feature_mask
                    unit_base_addr        = get_node_base (node, group);

                    gbl_snpshot_en = (feature_mask & FEATURE_GLOBAL_SNAPSHOT) ? 0 : 1;

                    gbl_stop_en = (feature_mask & FEATURE_GLOBAL_STOP) ? 0 : 1;

                    perf_cntr_ovrride     = dbg_read32 (__func__, unit_base_addr, HUP_CN_MISC_PERF_PGCTRL_OFFSET);
                    perf_cntr_ovrride     = SET_NODE_SNAPSHOT_CTRL (perf_cntr_ovrride, counter_num, gbl_snpshot_en);

                    perf_cntr_ovrride     = SET_NODE_GLBLSTOP_CTRL (perf_cntr_ovrride, counter_num, gbl_stop_en);

                    dbg_write32 (__func__, unit_base_addr, HUP_CN_MISC_PERF_PGCTRL_OFFSET, perf_cntr_ovrride);

                    // Clear counter
                    dbg_write32 (__func__, unit_base_addr, HUP_CN_MISC_PERF_PSNPC_OFFSET, CLR_NODE_PERF_CNTR (counter_num));

                    break;
                }
                default:
                {
                    hw_errmsg ("%s: Event select %d is not valid for PERFGRP_NODE\n", __func__, event_select);
                    hw_errmsg ("%s: Use one of PERF_SELECT_NODE_* defines in perf_events.h\n", __func__);
                    return PERFMON_STATUS_EVENTSEL_INVALID;
                }
            }
            break;
        }
        case PERFGRP_INTC:
        {
            UINT32 perf_cntr_ovrride;

            // FIXME: Track event assignments.

            // Set event mask in unit level mux control
            hw_printf(HW_PERFMON_DEBUG, "\n%s : Updating mux.\n", __func__);
            hw_write32((UINT32 *)(HUP_CHIP_INTC_ADDRESS + HUP_INTC_MUX_CONTROL_GROUP_OFFSET + (counter_num * 0x4)), event_mask);

            hw_printf(HW_PERFMON_DEBUG, "\n%s : Updating PGCTRL .\n", __func__);
            // Apply global snapshot & global stop settings based on feature_mask            
            perf_cntr_ovrride     = dbg_read32 (__func__, HUP_CHIP_INTC_ADDRESS, HUP_INTC_PERF_PGCTRL_OFFSET);
            perf_cntr_ovrride     = SET_INTC_SNAPSHOT_CTRL (perf_cntr_ovrride, counter_num, (feature_mask & FEATURE_GLOBAL_SNAPSHOT) ? 0 : 1);
            perf_cntr_ovrride     = SET_INTC_GLBLSTOP_CTRL (perf_cntr_ovrride, counter_num, (feature_mask & FEATURE_GLOBAL_STOP) ? 0 : 1);            
            dbg_write32 (__func__, HUP_CHIP_INTC_ADDRESS, HUP_INTC_PERF_PGCTRL_OFFSET, perf_cntr_ovrride);

            // Clear counter
            hw_printf(HW_PERFMON_DEBUG, "\n%s : Clearing counter\n", __func__);
            set_one_bit(__func__, HUP_CHIP_INTC_ADDRESS, HUP_INTC_PERF_PSNPC_OFFSET,
                        16+counter_num);

            hw_printf(HW_PERFMON_DEBUG, "\n%s : Done\n", __func__);
            break;
        }
        case PERFGRP_MIP0:
        case PERFGRP_MIP1:
        case PERFGRP_MIP2:
        case PERFGRP_MIP3:
        case PERFGRP_MIP4:
        case PERFGRP_MIP5:
        case PERFGRP_MIP6:
        case PERFGRP_TIP:
        {
            UINT32 unit_base_addr;
            UINT32 mctrl_offset;
            UINT32 perf_cntr_ovrride;
            UINT32 glob_stop_en;
            UINT32 glob_snapshot_en;

            unit_base_addr = isp_block_base_addr[group-PERFGRP_MIP0];

            if (group == PERFGRP_TIP)
            {
                if (event_select > PERF_SELECT_TIP_PRV_EVENT)
                {
                        hw_errmsg ("%s: Invalid event select %d provided for TIP group\n", __func__, event_select);
                        hw_errmsg ("%s: Use one of PERF_SELECT_TIP_*  macro definitions in bifrost_perfmon.h", __func__);
                        return PERFMON_STATUS_EVENTSEL_INVALID;
                }
                mctrl_offset = tip_subblock_mctrl_addr [event_select];
            }
            else
            {
                if (event_select > PERF_SELECT_MIP_PRV_EVENT)
                {
                        hw_errmsg ("%s: Invalid event select %d provided for MIP group\n", __func__, event_select);
                        hw_errmsg ("%s: Use one of PERF_SELECT_MIP_*  macro definitions in bifrost_perfmon.h", __func__);
                        return PERFMON_STATUS_EVENTSEL_INVALID;
                }
                mctrl_offset = mip_subblock_mctrl_addr [event_select];
            }
            // Enable perf clock
            set_one_bit (__func__, unit_base_addr, HUP_ISP_MIP_MIP_PRV_BASE_MIP_PRV_CTRL_OFFSET, HUP_ISP_MIP_MIP_PRV_BASE_MIP_PRV_CTRL_PERF_EN_LSB);
            
            // Update event mask in unit level mux control
            update_pcnt_mux (__func__, unit_base_addr, mctrl_offset, counter_num, event_mask);
            
            // Apply global snapshot & global stop settings
            glob_stop_en = feature_mask & FEATURE_GLOBAL_STOP ? 0 : 1;
            glob_snapshot_en = feature_mask & FEATURE_GLOBAL_SNAPSHOT ? 0 : 1;
            perf_cntr_ovrride = dbg_read32 (__func__, unit_base_addr, HUP_ISP_MIP_MIP_PRV_BASE_PERF_PGCTRL_OFFSET);
            perf_cntr_ovrride = SET_ISP_SNAPSHOT_CTRL (perf_cntr_ovrride, counter_num, glob_snapshot_en);
            perf_cntr_ovrride = SET_ISP_GLBLSTOP_CTRL (perf_cntr_ovrride, counter_num, glob_stop_en);
            dbg_write32 (__func__, unit_base_addr, HUP_ISP_MIP_MIP_PRV_BASE_PERF_PGCTRL_OFFSET, perf_cntr_ovrride);
            
            // Clear counter
            set_one_bit (__func__, unit_base_addr, HUP_ISP_MIP_MIP_PRV_BASE_PERF_PSNPC_OFFSET, HUP_ISP_MIP_MIP_PRV_BASE_PERF_PSNPC_PERF_CLR0_LSB+counter_num);
            break;
        }
        case PERFGRP_JBL:
        {
            UINT32 mctrl_offset;
            UINT32 perf_cntr_ovrride;
            UINT32 glob_stop_en;
            UINT32 glob_snapshot_en;
            UINT32 perfSel;

            // Make sure chicken bits are not disabling perf clk
            UINT32 chicken = dbg_read32(__func__, HUP_CHIP_JBL_ADDRESS, HUP_JBL_CHICKEN_SW_OFFSET);
            if (HUP_JBL_CHICKEN_SW_DEBUG_TIES_GET(chicken) & 1)
            {
                hw_errmsg ("%s: JBL chicken bits disabling perf clock\n");
                return PERFMON_STATUS_PERFCLK_DISABLED;
            }
            
            // Update event mask in unit level mux control
            perfSel = dbg_read32 (__func__, HUP_CHIP_JBL_ADDRESS, HUP_JBL_PERF_SELECT0_OFFSET);
            dbg_write32 (__func__, HUP_CHIP_JBL_ADDRESS, HUP_JBL_PERF_SELECT0_OFFSET, SET_JBL_PERF_SEL(perfSel, counter_num, event_mask));
            
            // Apply global snapshot & global stop settings
            glob_stop_en = feature_mask & FEATURE_GLOBAL_STOP ? 0 : 1;
            glob_snapshot_en = feature_mask & FEATURE_GLOBAL_SNAPSHOT ? 0 : 1;
            perf_cntr_ovrride = dbg_read32 (__func__, HUP_CHIP_JBL_ADDRESS, HUP_JBL_PERF_PGCTRL_OFFSET);
            // FIX_ME perf_cntr_ovrride = SET_ISP_SNAPSHOT_CTRL (perf_cntr_ovrride, counter_num, glob_snapshot_en);
            // FIX_ME perf_cntr_ovrride = SET_ISP_GLBLSTOP_CTRL (perf_cntr_ovrride, counter_num, glob_stop_en);
            dbg_write32 (__func__, HUP_CHIP_JBL_ADDRESS, HUP_JBL_PERF_PGCTRL_OFFSET, perf_cntr_ovrride);
            
            // Clear counter
            set_one_bit (__func__, HUP_CHIP_JBL_ADDRESS, HUP_JBL_PERF_PSNPC_OFFSET, HUP_JBL_PERF_PSNPC_PERF_CLR0_LSB+counter_num);
            break;
        }
        case PERFGRP_FM_MC0_Q0:
        case PERFGRP_FM_MC0_Q1:
        case PERFGRP_FM_MC0_Q2:
        case PERFGRP_FM_MC0_Q3:
        case PERFGRP_FM_MC0_Q4:
        case PERFGRP_FM_MC0_Q5:
        case PERFGRP_FM_MC0_Q6:
        case PERFGRP_FM_MC0_Q7:
        case PERFGRP_FM_MC0_Q8:
        case PERFGRP_FM_MC0_Q9:
        case PERFGRP_FM_MC0_Q10:
        case PERFGRP_FM_MC0_Q11:
        case PERFGRP_FM_MC1_Q0:
        case PERFGRP_FM_MC1_Q1:
        case PERFGRP_FM_MC1_Q2:
        case PERFGRP_FM_MC1_Q3:
        case PERFGRP_FM_MC1_Q4:
        case PERFGRP_FM_MC1_Q5:
        case PERFGRP_FM_MC1_Q6:
        case PERFGRP_FM_MC1_Q7:
        case PERFGRP_FM_MC1_Q8:
        case PERFGRP_FM_MC1_Q9:
        case PERFGRP_FM_MC1_Q10:
        case PERFGRP_FM_MC1_Q11:
        case PERFGRP_FM_BASRAM0:
        case PERFGRP_FM_BASRAM1:
        case PERFGRP_FM_BASRAM2:
        case PERFGRP_FM_PCIE:
        {
            // Add FM implementation here
            UINT32 fmbase_addr = fm_block_base_addr [group-PERFGRP_FM_MC0_Q0];
            UINT32 perfSel;
            UINT32 perf_cntr_ovrride;
            UINT32 glob_stop_en;
            UINT32 glob_snapshot_en;
            
            // Update event mask in unit level mux control
            perfSel = dbg_read32 (__func__, fmbase_addr, (counter_num < 4) ? HUP_FM_AXI_MUX_CTRL_3_0_OFFSET : HUP_FM_AXI_MUX_CTRL_7_4_OFFSET);
            dbg_write32 (__func__, fmbase_addr, (counter_num < 4) ?  HUP_FM_AXI_MUX_CTRL_3_0_OFFSET : HUP_FM_AXI_MUX_CTRL_7_4_OFFSET, SET_FM_PERF_SEL(perfSel, counter_num, event_select));
            
            // Apply global snapshot & global stop settings
            glob_stop_en = feature_mask & FEATURE_GLOBAL_STOP ? 0 : 1;
            glob_snapshot_en = feature_mask & FEATURE_GLOBAL_SNAPSHOT ? 0 : 1;
            perf_cntr_ovrride = dbg_read32 (__func__, fmbase_addr, HUP_FM_AXI_PERF_PGCTRL_OFFSET);
            perf_cntr_ovrride = SET_FM_SNAPSHOT_CTRL (perf_cntr_ovrride, counter_num, glob_snapshot_en);
            perf_cntr_ovrride = SET_FM_GLBLSTOP_CTRL (perf_cntr_ovrride, counter_num, glob_stop_en);
            dbg_write32 (__func__, fmbase_addr, HUP_FM_AXI_PERF_PGCTRL_OFFSET, perf_cntr_ovrride);
            
            // Clear counter
            set_one_bit (__func__, fmbase_addr, HUP_FM_AXI_PERF_PSNPC_OFFSET, HUP_FM_AXI_PERF_PSNPC_PERF_CLR0_LSB+counter_num);

            break;
        }
        case PERFGRP_DNNSRAM:
        case PERFGRP_DNN_250:
        case PERFGRP_DNN_500:
        {
            UINT32 perf_sel_reg;
            UINT32 mctrl0_addr;
            UINT32 mctrl1_addr;
            UINT32 pgctrl_addr;
            UINT32 psnpc_addr;
            UINT32 perf_cntr_ovrride;
            UINT32 glob_stop_en;
            UINT32 glob_snapshot_en;

            if (group == PERFGRP_DNN_250)
            {
                // Program event selects
                perf_sel_reg = dbg_read32 (__func__, HUP_CHIP_DNNSRAM_DNN_DEBUG_PERFMON_250_SEL_ADDRESS, 0);
                perf_sel_reg = SET_DNN_PERF_SEL(perf_sel_reg, counter_num, event_select);
                dbg_write32 (__func__,HUP_CHIP_DNNSRAM_DNN_DEBUG_PERFMON_250_SEL_ADDRESS, 0, perf_sel_reg);
                mctrl0_addr = HUP_CHIP_DNNSRAM_DNN_REGS_TP_PERF_250_MCTRL0_ADDRESS;
                mctrl1_addr = HUP_CHIP_DNNSRAM_DNN_REGS_TP_PERF_250_MCTRL1_ADDRESS;                
                pgctrl_addr = HUP_CHIP_DNNSRAM_DNN_PERF_250_PERF_PGCTRL_ADDRESS;
                psnpc_addr = HUP_CHIP_DNNSRAM_DNN_PERF_250_PERF_PSNPC_ADDRESS;
            }            
            else if (group == PERFGRP_DNN_500)
            {
                // Program event selects
                perf_sel_reg = dbg_read32 (__func__, HUP_CHIP_DNNSRAM_DNN_DEBUG_PERFMON_500_SEL_ADDRESS, 0);
                perf_sel_reg = SET_DNN_PERF_SEL(perf_sel_reg, counter_num, event_select);
                dbg_write32 (__func__, HUP_CHIP_DNNSRAM_DNN_DEBUG_PERFMON_500_SEL_ADDRESS, 0, perf_sel_reg);
                mctrl0_addr = HUP_CHIP_DNNSRAM_DNN_REGS_TP_PERF_500_MCTRL0_ADDRESS;
                mctrl1_addr = HUP_CHIP_DNNSRAM_DNN_REGS_TP_PERF_500_MCTRL1_ADDRESS;
                pgctrl_addr = HUP_CHIP_DNNSRAM_DNN_PERF_500_PERF_PGCTRL_ADDRESS;
                psnpc_addr = HUP_CHIP_DNNSRAM_DNN_PERF_500_PERF_PSNPC_ADDRESS;                
            }
            else
            {
                mctrl0_addr = HUP_CHIP_DNNSRAM_BASRAM_XB_PERF_MCTRL0_ADDRESS;
                mctrl1_addr = HUP_CHIP_DNNSRAM_BASRAM_XB_PERF_MCTRL1_ADDRESS;
                pgctrl_addr = HUP_CHIP_DNNSRAM_PERF_PGCTRL_ADDRESS;
                psnpc_addr = HUP_CHIP_DNNSRAM_PERF_PSNPC_ADDRESS;
                // Enable perf clock
                set_one_bit(__func__, HUP_CHIP_DNNSRAM_BASRAM_BASRAM_CLK_ADDRESS, 0, BASRAM_REGS_BASRAM_CLK_PERF_CLKG_LSB);
            }

            if (counter_num < 4)
            {
                update_pcnt_mux(__func__, mctrl0_addr, 0, counter_num, event_mask);                
            }
            else
            {
                update_pcnt_mux(__func__, mctrl1_addr, 0, counter_num-4, event_mask);
            }

            // Enable perf clock
            set_one_bit (__func__, HUP_CHIP_DNNSRAM_DNNSRAM_CTRL_ADDRESS, 0, DNNSRAM_CTRL_DNNSRAM_PERF_EN_LSB);
            // Enable debug clock
            set_one_bit (__func__, HUP_CHIP_DNNSRAM_DNN_DEBUG_DEBUG_CONTROL_ADDRESS, 0, HUP_DNN_DEBUG_DEBUG_CONTROL_DBG_EN_LSB);
            // Override clock gating
            set_one_bit (__func__, HUP_CHIP_DNNSRAM_DNNSRAM_CCTRL_ADDRESS, 0, DNNSRAM_CCTRL_DNN_CLKG_OVR_LSB);
            // Apply global snapshot & global stop settings
            glob_stop_en = feature_mask & FEATURE_GLOBAL_STOP ? 0 : 1;
            glob_snapshot_en = feature_mask & FEATURE_GLOBAL_SNAPSHOT ? 0 : 1;
            perf_cntr_ovrride = dbg_read32 (__func__, pgctrl_addr, 0);
            perf_cntr_ovrride = SET_DNN_SNAPSHOT_CTRL (perf_cntr_ovrride, counter_num, glob_snapshot_en);
            perf_cntr_ovrride = SET_DNN_GLBLSTOP_CTRL (perf_cntr_ovrride, counter_num, glob_stop_en);
            dbg_write32 (__func__, pgctrl_addr, 0, perf_cntr_ovrride);
            
            // Clear counter
            set_one_bit (__func__, psnpc_addr, 0, HUP_DNNSRAM_PERF_PSNPC_PERF_CLR0_LSB+counter_num);

            break;
        }
        case PERFGRP_MCU0:
        case PERFGRP_MCU1:
        {
            UINT32 mcuoffset = (group-PERFGRP_MCU0)*(HUP_CHIP_MCU1_ADDRESS-HUP_CHIP_MCU0_ADDRESS);
            UINT32 perf_cntr_ovrride;
            UINT32 glob_stop_en;
            UINT32 glob_snapshot_en;

            if (perf_cur_state[node][(int) group][counter_num].valid && (perf_cur_state[node][(int) group][counter_num].event_select != event_select))
            {
                // Clear mux ctrl used in previous config
                switch (perf_cur_state[node][(int) group][counter_num].event_select)
                {
                    case PERF_SELECT_MCU_MCP_EVENT:
                        if (counter_num < 4)
                        {
                            update_pcnt_mux(__func__, HUP_CHIP_MCU0_MCP_REGS_PERFMUX0_ADDRESS, mcuoffset, counter_num, 0);
                        }
                        else
                        {
                            update_pcnt_mux(__func__, HUP_CHIP_MCU0_MCP_REGS_PERFMUX1_ADDRESS, mcuoffset, counter_num-4, 0);
                        }
                        break;
                    case PERF_SELECT_MCU_SCH_EVENT:
                        dbg_write32 (__func__, HUP_CHIP_MCU0_SCH_REGS_SCH_PERF_EVENT0_ADDRESS + (counter_num*4), mcuoffset, 0);
                        break;
                    case PERF_SELECT_MCU_PMG_EVENT:
                        dbg_write32 (__func__, HUP_CHIP_MCU0_PMG_REGS_PERF_MUX_EVENT0_ADDRESS + (counter_num*4), mcuoffset, 0);
                        break;
                    case PERF_SELECT_MCU_DDC_EVENT:
                        if (counter_num < 4)
                        {
                            update_pcnt_mux(__func__, HUP_CHIP_MCU0_DDC_REGS_PERFMUX_0_ADDRESS, mcuoffset, counter_num, 0);
                        }
                        else
                        {
                            update_pcnt_mux(__func__, HUP_CHIP_MCU0_DDC_REGS_PERFMUX_1_ADDRESS, mcuoffset, counter_num-4, 0);
                        }
                        break;
                    default:
                        hw_errmsg ("%s: Event select %d not valid for MCU\n", __func__, event_select);
                        return PERFMON_STATUS_EVENTSEL_INVALID;
                }
            }

            // Set mux ctrl for selected sub-block
            switch (event_select)
            {
                case PERF_SELECT_MCU_MCP_EVENT:
                    if (counter_num < 4)
                    {
                        update_pcnt_mux(__func__, HUP_CHIP_MCU0_MCP_REGS_PERFMUX0_ADDRESS, mcuoffset, counter_num, event_mask);
                    }
                    else
                    {
                        update_pcnt_mux(__func__, HUP_CHIP_MCU0_MCP_REGS_PERFMUX1_ADDRESS, mcuoffset, counter_num-4, event_mask);
                    }
                    break;
                case PERF_SELECT_MCU_SCH_EVENT:
                    dbg_write32 (__func__, HUP_CHIP_MCU0_SCH_REGS_SCH_PERF_EVENT0_ADDRESS + (counter_num*4), mcuoffset, event_mask);
                    break;
                case PERF_SELECT_MCU_PMG_EVENT:
                    dbg_write32 (__func__, HUP_CHIP_MCU0_PMG_REGS_PERF_MUX_EVENT0_ADDRESS + (counter_num*4), mcuoffset, event_mask);
                    break;
                case PERF_SELECT_MCU_DDC_EVENT:
                    if (counter_num < 4)
                    {
                        update_pcnt_mux(__func__, HUP_CHIP_MCU0_DDC_REGS_PERFMUX_0_ADDRESS, mcuoffset, counter_num, event_mask);
                    }
                    else
                    {
                        update_pcnt_mux(__func__, HUP_CHIP_MCU0_DDC_REGS_PERFMUX_1_ADDRESS, mcuoffset, counter_num-4, event_mask);
                    }
                    break;
                default:
                    hw_errmsg ("%s: Event select %d not valid for MCU\n", __func__, event_select);
                    return PERFMON_STATUS_EVENTSEL_INVALID;
            }

            // Enable perf clock
            set_one_bit (__func__, HUP_CHIP_MCU0_PMB_REGS_CONTROL_ADDRESS, mcuoffset, HUP_MCU_PMB_CONTROL_PERF_EN_LSB);

            // Apply global snapshot & global stop settings
            glob_stop_en = feature_mask & FEATURE_GLOBAL_STOP ? 0 : 1;
            glob_snapshot_en = feature_mask & FEATURE_GLOBAL_SNAPSHOT ? 0 : 1;
            perf_cntr_ovrride = dbg_read32 (__func__, HUP_CHIP_MCU0_PMB_REGS_PERF_PGCTRL_ADDRESS, mcuoffset);
            perf_cntr_ovrride = SET_DNN_SNAPSHOT_CTRL (perf_cntr_ovrride, counter_num, glob_snapshot_en);
            perf_cntr_ovrride = SET_DNN_GLBLSTOP_CTRL (perf_cntr_ovrride, counter_num, glob_stop_en);
            dbg_write32 (__func__, HUP_CHIP_MCU0_PMB_REGS_PERF_PGCTRL_ADDRESS, mcuoffset, perf_cntr_ovrride);
            
            // Clear counter
            set_one_bit (__func__, HUP_CHIP_MCU0_PMB_REGS_PERF_PSNPC_ADDRESS, mcuoffset, HUP_MCU_PMB_PERF_PSNPC_PERF_CLR0_LSB+counter_num);
            
            break;
        }
        case PERFGRP_IMP0:
        case PERFGRP_IMP1:
        {
            UINT32 perf_cntr_ovrride;
            UINT32 glob_stop_en;
            UINT32 glob_snapshot_en;
            UINT32 impPerfOffset = (group == PERFGRP_IMP0) ? 0 : HUP_LSR_IMP_SIZE;
            UINT32 topEventMask = event_mask;
            
            // Set mux ctrl for selected sub-block
            switch (event_select)
            {
                case PERF_SELECT_IMP_MDMA_EVENT:
                    // Enable debug clock
                    UINT32 perf_ctrl;
                    perf_ctrl = dbg_read32 (__func__, HUP_CHIP_IMP0_SDMA_PERF_CTRL_ADDRESS, impPerfOffset);
                    dbg_write32 (__func__, HUP_CHIP_IMP0_SDMA_PERF_CTRL_ADDRESS, impPerfOffset, HUP_ISP_TIP_MDMA_BASE_PERF_CTRL_PERF_DBG_EN_MODIFY(perf_ctrl,1));

                    // Update mux ctrl in MDMA
                    update_pcnt_mux(__func__, HUP_CHIP_IMP0_SDMA_PERF_MCTRL_ADDRESS, impPerfOffset, counter_num, event_mask);
                    if(counter_num == 0)
                    {
                        topEventMask = PERF_MASK_IMP_TOP_SDMA_CNTR0;
                    }
                    else if(counter_num == 1)
                    {
                        topEventMask = PERF_MASK_IMP_TOP_SDMA_CNTR1;
                    }
                    else if(counter_num == 2)
                    {
                        topEventMask = PERF_MASK_IMP_TOP_SDMA_CNTR2;
                    }
                    else if(counter_num == 3)
                    {
                        topEventMask = PERF_MASK_IMP_TOP_SDMA_CNTR3;
                    }
                    // fall through
                case PERF_SELECT_IMP_TOP_EVENT:
                    update_pcnt_mux(__func__, HUP_CHIP_IMP0_MCTRL_IMP_ADDRESS, impPerfOffset, counter_num, topEventMask);
                    break;
            }
            
            // Enable perf clock
            set_one_bit (__func__, HUP_CHIP_IMP0_PRV_PRV_EN_ADDRESS, impPerfOffset, HUP_LSR_IMP_IMP_PRV_PRV_EN_PERF_LSB);
            
            // Apply global snapshot & global stop settings
            glob_stop_en = feature_mask & FEATURE_GLOBAL_STOP ? 0 : 1;
            glob_snapshot_en = feature_mask & FEATURE_GLOBAL_SNAPSHOT ? 0 : 1;
            perf_cntr_ovrride = dbg_read32 (__func__, HUP_CHIP_IMP0_PRV_PERF_PGCTRL_ADDRESS, impPerfOffset);
            perf_cntr_ovrride = SET_IMP_SNAPSHOT_CTRL (perf_cntr_ovrride, counter_num, glob_snapshot_en);
            perf_cntr_ovrride = SET_IMP_GLBLSTOP_CTRL (perf_cntr_ovrride, counter_num, glob_stop_en);
            dbg_write32 (__func__, HUP_CHIP_IMP0_PRV_PERF_PGCTRL_ADDRESS, impPerfOffset, perf_cntr_ovrride);
            
            // Clear counter
            set_one_bit (__func__, HUP_CHIP_IMP0_PRV_PERF_PSNPC_ADDRESS, impPerfOffset, HUP_LSR_IMP_IMP_PRV_PERF_PSNPC_PERF_CLR0_LSB + counter_num);
            
            break;
        }
        case PERFGRP_LSR0_SLOW:
        case PERFGRP_LSR1_SLOW:
        {
            UINT32 perf_cntr_ovrride;
            UINT32 glob_stop_en;
            UINT32 glob_snapshot_en;
            UINT32 func_en;
            UINT32 lsrPerfOffset = (group - PERFGRP_LSR0_SLOW) * (HUP_CHIP_LSR1_LSR_SLOW_ADDRESS - HUP_CHIP_LSR0_LSR_SLOW_ADDRESS);
            
            update_pcnt_mux(__func__, HUP_CHIP_LSR0_LSR_SLOW_SLOW_PERFM0_ADDRESS, lsrPerfOffset, counter_num, event_mask);

            // Enable perf clock
            func_en = dbg_read32 (__func__, HUP_CHIP_LSR0_LSR_FAST_LSR_FUNCEN_ADDRESS, lsrPerfOffset);
            func_en = HUP_LSR_FAST_LSR_FUNCEN_PERF_EN_MODIFY(func_en,1);
            dbg_write32 (__func__, HUP_CHIP_LSR0_LSR_FAST_LSR_FUNCEN_ADDRESS, lsrPerfOffset, func_en);
            
            // Apply global snapshot & global stop settings
            glob_stop_en = feature_mask & FEATURE_GLOBAL_STOP ? 0 : 1;
            glob_snapshot_en = feature_mask & FEATURE_GLOBAL_SNAPSHOT ? 0 : 1;
            perf_cntr_ovrride = dbg_read32 (__func__, HUP_CHIP_LSR0_LSR_SLOW_SLOW_PRV_PERF_PGCTRL_ADDRESS, lsrPerfOffset);
            perf_cntr_ovrride = SET_LSR_SLOW_SNAPSHOT_CTRL (perf_cntr_ovrride, counter_num, glob_snapshot_en);
            perf_cntr_ovrride = SET_LSR_SLOW_GLBLSTOP_CTRL (perf_cntr_ovrride, counter_num, glob_stop_en);
            dbg_write32 (__func__, HUP_CHIP_LSR0_LSR_SLOW_SLOW_PRV_PERF_PGCTRL_ADDRESS, lsrPerfOffset, perf_cntr_ovrride);
            
            // Clear counter
            set_one_bit (__func__, HUP_CHIP_LSR0_LSR_SLOW_SLOW_PRV_PERF_PSNPC_ADDRESS, lsrPerfOffset, HUP_LSR_SLOW_SLOW_PRV_PERF_PSNPC_PERF_CLR0_LSB + counter_num);
            
            break;
        }
        case PERFGRP_LSR0_FAST:
        case PERFGRP_LSR1_FAST:
        {
            UINT32 perf_cntr_ovrride;
            UINT32 glob_stop_en;
            UINT32 glob_snapshot_en;
            UINT32 lsrPerfOffset = (group - PERFGRP_LSR0_FAST) * (HUP_CHIP_LSR1_LSR_FAST_ADDRESS - HUP_CHIP_LSR0_LSR_FAST_ADDRESS);            
            UINT32 blockOffset;
            UINT32 func_en;

            // Make sure this matches the order of event select defines
            static const UINT32 lsr_mux_ctrl_addr[] = 
            {
                HUP_CHIP_LSR0_LSR_FAST_L3C_L3_PERFM0_ADDRESS,
                HUP_CHIP_LSR0_LSR_FAST_L3A_L3_PERFM0_ADDRESS,
                HUP_CHIP_LSR0_LSR_OP_MCTRL_LSR_OP0_ADDRESS,
                HUP_CHIP_LSR0_LSR_FAST_TC_PERF_MUX0_ADDRESS,
                HUP_CHIP_LSR0_LSR_PP_MCTRL_PP0_ADDRESS,
                HUP_CHIP_LSR0_LSR_PP_MCTRL_PP0_ADDRESS + HUP_CHIP_LSR0_LSR_PP_ARRAY_ELEMENT_SIZE,
                HUP_CHIP_LSR0_LSR_PP_MCTRL_PP0_ADDRESS + 2*HUP_CHIP_LSR0_LSR_PP_ARRAY_ELEMENT_SIZE,
                HUP_CHIP_LSR0_LSR_CG_MISC_PERF_MUX0_ADDRESS,
                HUP_CHIP_LSR0_LSR_CG_MISC_PERF_MUX0_ADDRESS + HUP_CHIP_LSR0_LSR_CG_ARRAY_ELEMENT_SIZE,
                HUP_CHIP_LSR0_LSR_CG_MISC_PERF_MUX0_ADDRESS + 2*HUP_CHIP_LSR0_LSR_CG_ARRAY_ELEMENT_SIZE,
                HUP_CHIP_LSR0_LSR_CG_SLB0_PSC_PS_PERFM0_ADDRESS,
                HUP_CHIP_LSR0_LSR_CG_SLB0_PSC_PS_PERFM0_ADDRESS + HUP_CHIP_LSR0_LSR_CG_ARRAY_ELEMENT_SIZE,
                HUP_CHIP_LSR0_LSR_CG_SLB0_PSC_PS_PERFM0_ADDRESS + 2*HUP_CHIP_LSR0_LSR_CG_ARRAY_ELEMENT_SIZE,
                HUP_CHIP_LSR0_LSR_CG_SLB1_PSC_PS_PERFM0_ADDRESS,
                HUP_CHIP_LSR0_LSR_CG_SLB1_PSC_PS_PERFM0_ADDRESS + HUP_CHIP_LSR0_LSR_CG_ARRAY_ELEMENT_SIZE,
                HUP_CHIP_LSR0_LSR_CG_SLB1_PSC_PS_PERFM0_ADDRESS,
                HUP_CHIP_LSR0_LSR_CG_SLB1_PSA_PS_PERFM0_ADDRESS,
                HUP_CHIP_LSR0_LSR_CG_SLB1_PSA_PS_PERFM0_ADDRESS + HUP_CHIP_LSR0_LSR_CG_ARRAY_ELEMENT_SIZE,
                HUP_CHIP_LSR0_LSR_CG_SLB1_PSA_PS_PERFM0_ADDRESS + 2*HUP_CHIP_LSR0_LSR_CG_ARRAY_ELEMENT_SIZE
            };

            static_assert ((sizeof(lsr_mux_ctrl_addr)/sizeof(UINT32)) == (PERF_SELECT_LSR_FAST_CG2_SLB1_PSA_EVENT+1));
            
            if (perf_cur_state[node][(int) group][counter_num].valid && (perf_cur_state[node][(int) group][counter_num].event_select != event_select))
            {
                // Clear mux ctrl used in previous config
                switch (perf_cur_state[node][(int) group][counter_num].event_select)
                {
                    case PERF_SELECT_LSR_FAST_L3C_EVENT         :
                    case PERF_SELECT_LSR_FAST_L3A_EVENT         :
                    case PERF_SELECT_LSR_FAST_OP_EVENT          :
                    case PERF_SELECT_LSR_FAST_TC_EVENT          :
                    case PERF_SELECT_LSR_FAST_PP0_EVENT         :
                    case PERF_SELECT_LSR_FAST_PP1_EVENT         :
                    case PERF_SELECT_LSR_FAST_PP2_EVENT         :
                    case PERF_SELECT_LSR_FAST_CG0_TOP_EVENT     :
                    case PERF_SELECT_LSR_FAST_CG1_TOP_EVENT     :
                    case PERF_SELECT_LSR_FAST_CG2_TOP_EVENT     :
                    case PERF_SELECT_LSR_FAST_CG0_SLB0_PSC_EVENT:
                    case PERF_SELECT_LSR_FAST_CG1_SLB0_PSC_EVENT:
                    case PERF_SELECT_LSR_FAST_CG2_SLB0_PSC_EVENT:
                    case PERF_SELECT_LSR_FAST_CG0_SLB1_PSC_EVENT:
                    case PERF_SELECT_LSR_FAST_CG1_SLB1_PSC_EVENT:
                    case PERF_SELECT_LSR_FAST_CG2_SLB1_PSC_EVENT:
                    case PERF_SELECT_LSR_FAST_CG0_SLB1_PSA_EVENT:
                    case PERF_SELECT_LSR_FAST_CG1_SLB1_PSA_EVENT:
                    case PERF_SELECT_LSR_FAST_CG2_SLB1_PSA_EVENT:                    
                    {
                        if (counter_num < 4)
                        {
                            update_pcnt_mux(__func__, lsr_mux_ctrl_addr[perf_cur_state[node][(int) group][counter_num].event_select], lsrPerfOffset, counter_num, 0);
                        }
                        else
                        {
                            update_pcnt_mux(__func__, lsr_mux_ctrl_addr[perf_cur_state[node][(int) group][counter_num].event_select] + 4, lsrPerfOffset, counter_num-4, 0);
                        }
                        break;
                    }
                    default:
                    {
                        hw_errmsg ("%s: Event select %d not valid for LSR Fast\n", __func__, event_select);
                        return PERFMON_STATUS_EVENTSEL_INVALID;
                    }
                }
            }
            
            // Set mux ctrl for selected sub-block
            switch (event_select)
            {
                case PERF_SELECT_LSR_FAST_L3C_EVENT         :
                case PERF_SELECT_LSR_FAST_L3A_EVENT         :
                case PERF_SELECT_LSR_FAST_OP_EVENT          :
                case PERF_SELECT_LSR_FAST_TC_EVENT          :
                case PERF_SELECT_LSR_FAST_PP0_EVENT         :
                case PERF_SELECT_LSR_FAST_PP1_EVENT         :
                case PERF_SELECT_LSR_FAST_PP2_EVENT         :
                case PERF_SELECT_LSR_FAST_CG0_TOP_EVENT     :
                case PERF_SELECT_LSR_FAST_CG1_TOP_EVENT     :
                case PERF_SELECT_LSR_FAST_CG2_TOP_EVENT     :
                case PERF_SELECT_LSR_FAST_CG0_SLB0_PSC_EVENT:
                case PERF_SELECT_LSR_FAST_CG1_SLB0_PSC_EVENT:
                case PERF_SELECT_LSR_FAST_CG2_SLB0_PSC_EVENT:
                case PERF_SELECT_LSR_FAST_CG0_SLB1_PSC_EVENT:
                case PERF_SELECT_LSR_FAST_CG1_SLB1_PSC_EVENT:
                case PERF_SELECT_LSR_FAST_CG2_SLB1_PSC_EVENT:
                case PERF_SELECT_LSR_FAST_CG0_SLB1_PSA_EVENT:
                case PERF_SELECT_LSR_FAST_CG1_SLB1_PSA_EVENT:
                case PERF_SELECT_LSR_FAST_CG2_SLB1_PSA_EVENT:
                {
                    if (counter_num < 4)
                    {
                        update_pcnt_mux(__func__, lsr_mux_ctrl_addr[event_select], lsrPerfOffset, counter_num, event_mask);
                    }
                    else
                    {
                        update_pcnt_mux(__func__, lsr_mux_ctrl_addr[event_select] + 4, lsrPerfOffset, counter_num-4, event_mask);
                    }
                    break;
                }
                default:
                    hw_errmsg ("%s: Event select %d not valid for LSR Fast\n", __func__, event_select);
                    return PERFMON_STATUS_EVENTSEL_INVALID;
            }

            // Enable perf clock
            func_en = dbg_read32 (__func__, HUP_CHIP_LSR0_LSR_FAST_LSR_FUNCEN_ADDRESS, lsrPerfOffset);
            func_en = HUP_LSR_FAST_LSR_FUNCEN_PERF_EN_MODIFY(func_en,1);
            dbg_write32 (__func__, HUP_CHIP_LSR0_LSR_FAST_LSR_FUNCEN_ADDRESS, lsrPerfOffset, func_en);

            // Additional perf enables for sub-blocks
            switch (event_select)
            {
                case PERF_SELECT_LSR_FAST_OP_EVENT          :
                {
                    func_en = dbg_read32 (__func__, HUP_CHIP_LSR0_LSR_OP_FUNCEN_ADDRESS, lsrPerfOffset);
                    func_en = HUP_LSR_OP_FUNCEN_PERF_EN_MODIFY(func_en,1);
                    dbg_write32 (__func__, HUP_CHIP_LSR0_LSR_OP_FUNCEN_ADDRESS, lsrPerfOffset, func_en);
                    break;
                }
                case PERF_SELECT_LSR_FAST_PP0_EVENT         :
                case PERF_SELECT_LSR_FAST_PP1_EVENT         :
                case PERF_SELECT_LSR_FAST_PP2_EVENT         :
                {
                    blockOffset = (event_select - PERF_SELECT_LSR_FAST_PP0_EVENT) * HUP_CHIP_LSR0_LSR_PP_ARRAY_ELEMENT_SIZE;
                    func_en = dbg_read32 (__func__, HUP_CHIP_LSR0_LSR_PP_PP_PPFUNCEN_ADDRESS, lsrPerfOffset + blockOffset);
                    func_en = HUP_LSR_PP_PP_PPFUNCEN_PERF_EN_MODIFY(func_en,1);
                    dbg_write32 (__func__, HUP_CHIP_LSR0_LSR_PP_PP_PPFUNCEN_ADDRESS, lsrPerfOffset + blockOffset, func_en);
                    break;
                }
                case PERF_SELECT_LSR_FAST_CG0_TOP_EVENT     :
                case PERF_SELECT_LSR_FAST_CG1_TOP_EVENT     :
                case PERF_SELECT_LSR_FAST_CG2_TOP_EVENT     :
                case PERF_SELECT_LSR_FAST_CG0_SLB0_PSC_EVENT:
                case PERF_SELECT_LSR_FAST_CG1_SLB0_PSC_EVENT:
                case PERF_SELECT_LSR_FAST_CG2_SLB0_PSC_EVENT:
                case PERF_SELECT_LSR_FAST_CG0_SLB1_PSC_EVENT:
                case PERF_SELECT_LSR_FAST_CG1_SLB1_PSC_EVENT:
                case PERF_SELECT_LSR_FAST_CG2_SLB1_PSC_EVENT:
                case PERF_SELECT_LSR_FAST_CG0_SLB1_PSA_EVENT:
                case PERF_SELECT_LSR_FAST_CG1_SLB1_PSA_EVENT:
                case PERF_SELECT_LSR_FAST_CG2_SLB1_PSA_EVENT:
                {
                    blockOffset = ((event_select -  PERF_SELECT_LSR_FAST_CG0_TOP_EVENT)%3) * HUP_CHIP_LSR0_LSR_CG_ARRAY_ELEMENT_SIZE;
                    func_en = dbg_read32 (__func__, HUP_CHIP_LSR0_LSR_CG_MISC_FUNCEN_ADDRESS, lsrPerfOffset + blockOffset);
                    func_en = HUP_LSR_CG_MISC_FUNCEN_PERF_EN_MODIFY(func_en,1);
                    dbg_write32 (__func__, HUP_CHIP_LSR0_LSR_CG_MISC_FUNCEN_ADDRESS, lsrPerfOffset + blockOffset, func_en);
                    break;
                }
                default:
                {
                    // Do nothing
                    break;
                }
            }

            // Apply global snapshot & global stop settings
            glob_stop_en = feature_mask & FEATURE_GLOBAL_STOP ? 0 : 1;
            glob_snapshot_en = feature_mask & FEATURE_GLOBAL_SNAPSHOT ? 0 : 1;
            perf_cntr_ovrride = dbg_read32 (__func__, HUP_CHIP_LSR0_LSR_FAST_FAST_PRV_PERF_PGCTRL_ADDRESS, lsrPerfOffset);
            perf_cntr_ovrride = SET_LSR_SLOW_SNAPSHOT_CTRL (perf_cntr_ovrride, counter_num, glob_snapshot_en);
            perf_cntr_ovrride = SET_LSR_SLOW_GLBLSTOP_CTRL (perf_cntr_ovrride, counter_num, glob_stop_en);
            dbg_write32 (__func__, HUP_CHIP_LSR0_LSR_FAST_FAST_PRV_PERF_PGCTRL_ADDRESS, lsrPerfOffset, perf_cntr_ovrride);
            
            // Clear counter
            set_one_bit (__func__, HUP_CHIP_LSR0_LSR_FAST_FAST_PRV_PERF_PSNPC_ADDRESS, lsrPerfOffset, HUP_LSR_FAST_FAST_PRV_PERF_PSNPC_PERF_CLR0_LSB + counter_num);

            break;
        }
        case NUM_PERFGRP:
        default:
        {
            hw_errmsg ("%s: PERF_GROUP %d not valid\n", __func__, group);
            return PERFMON_STATUS_PERFGRP_INVALID;
        }
    }

    perf_cur_state [node][(int) group][counter_num] = (perfmon_config) { true, true, event_select, event_mask, feature_mask, currentOwner };
    hw_cacheFlushAddr (&perf_cur_state [node][(int) group][counter_num]);

    return PERFMON_STATUS_SUCCESS;
}

//
// Set the contents of the counter register to specified value
//
HW_STATUSCODE   perfmon_setPerfCounter     (UINT32     node        ,
                                            PERF_GROUP group       ,
                                            UINT32     counter_num ,
                                            UINT64     value)
{

    if (group > PERFGRP_NODE)
    {
        // All global counters should use the same node value when updating state
        node = 0;
    }

    if (! sanity_check_args (__func__, node, group, counter_num))
    {
        return PERFMON_STATUS_SANITY_FAIL;
    }

    HW_CORE currentOwner = (HW_CORE) hw_getCoreNum();    

    if (perf_cur_state [node][(int) group][counter_num].in_use && perf_cur_state[node][(int) group][counter_num].owner != currentOwner)
    {
        hw_errmsg ("%s: Core %d requesting Counter (node=%d, group=%d, cntr#=%d) which is in use by another core %d\n", __func__, currentOwner, node, group, counter_num, perf_cur_state[node][(int) group][counter_num].owner);
        return PERFMON_STATUS_COUNTER_LOCKED;
    }

    switch (group)
    {
        case PERFGRP_LX5_CORE0:
        case PERFGRP_LX5_CORE1:
        {
            UINT32 perfmon_base;
            UINT32 cntr_reg_offset;

            perfmon_base = get_lx5_perfmon_base (node, group);

            cntr_reg_offset = PERFMON_LX5_PM0_OFFSET + (counter_num * 4);

            // Write value to counter register
            dbg_write32 (__func__, perfmon_base, cntr_reg_offset, (UINT32) value);

            break;
        }
        case PERFGRP_NODE:
        {
            dbg_write32 (__func__, get_node_base (node, group), HUP_CN_MISC_PERF_PCNTR0_OFFSET + (counter_num * 8), (UINT32) value);

            break;
        }

        case PERFGRP_INTC:
        {
            dbg_write32(__func__, HUP_CHIP_INTC_ADDRESS, HUP_INTC_PERF_PCNTR0_OFFSET + (counter_num * 8), (UINT32) value);
            break;
        }
        case PERFGRP_CLSRAM:
        {
            dbg_write32 (__func__, HUP_CHIP_CLSRAM_ADDRESS, HUP_CLSRAM_MISC_PERF_PCNTR0_OFFSET + (counter_num * 8), (UINT32) value);

            break;
        }
        case PERFGRP_MIP0:
        case PERFGRP_MIP1:
        case PERFGRP_MIP2:
        case PERFGRP_MIP3:
        case PERFGRP_MIP4:
        case PERFGRP_MIP5:
        case PERFGRP_MIP6:
        case PERFGRP_TIP:
        {
            UINT32 isp_base_addr = isp_block_base_addr[group-PERFGRP_MIP0];

            dbg_write32 (__func__, isp_base_addr, HUP_ISP_MIP_MIP_PRV_BASE_PERF_PCNTR0_OFFSET + (counter_num * 8), (UINT32) value);
        }
        case PERFGRP_JBL:
        {
            dbg_write32 (__func__, HUP_CHIP_JBL_ADDRESS, HUP_JBL_PERF_PCNTR0_OFFSET + (counter_num * 8), (UINT32) value);            
            break;
        }
        case PERFGRP_FM_MC0_Q0:
        case PERFGRP_FM_MC0_Q1:
        case PERFGRP_FM_MC0_Q2:
        case PERFGRP_FM_MC0_Q3:
        case PERFGRP_FM_MC0_Q4:
        case PERFGRP_FM_MC0_Q5:
        case PERFGRP_FM_MC0_Q6:
        case PERFGRP_FM_MC0_Q7:
        case PERFGRP_FM_MC0_Q8:
        case PERFGRP_FM_MC0_Q9:
        case PERFGRP_FM_MC0_Q10:
        case PERFGRP_FM_MC0_Q11:
        case PERFGRP_FM_MC1_Q0:
        case PERFGRP_FM_MC1_Q1:
        case PERFGRP_FM_MC1_Q2:
        case PERFGRP_FM_MC1_Q3:
        case PERFGRP_FM_MC1_Q4:
        case PERFGRP_FM_MC1_Q5:
        case PERFGRP_FM_MC1_Q6:
        case PERFGRP_FM_MC1_Q7:
        case PERFGRP_FM_MC1_Q8:
        case PERFGRP_FM_MC1_Q9:
        case PERFGRP_FM_MC1_Q10:
        case PERFGRP_FM_MC1_Q11:
        case PERFGRP_FM_BASRAM0:
        case PERFGRP_FM_BASRAM1:
        case PERFGRP_FM_BASRAM2:
        case PERFGRP_FM_PCIE:
        {
            // Add FM implementation here
            UINT32 fmbase_addr = fm_block_base_addr [group-PERFGRP_FM_MC0_Q0];
            dbg_write32 (__func__, fmbase_addr, HUP_FM_AXI_PERF_PCNTR0_OFFSET + (counter_num * 8), (UINT32) value);
            break;
        }
        case PERFGRP_DNNSRAM:
        case PERFGRP_DNN_250:
        case PERFGRP_DNN_500:
        {
            UINT32 pcntr0_addr;
            if (group == PERFGRP_DNN_250)
            {
                pcntr0_addr = HUP_CHIP_DNNSRAM_DNN_PERF_250_PERF_PCNTR0_ADDRESS;
            }
            else if (group == PERFGRP_DNN_500)
            {
                pcntr0_addr = HUP_CHIP_DNNSRAM_DNN_PERF_500_PERF_PCNTR0_ADDRESS;
            }
            else
            {
                pcntr0_addr = HUP_CHIP_DNNSRAM_PERF_PCNTR0_ADDRESS;
            }
            dbg_write32 (__func__, pcntr0_addr, (counter_num * 8), (UINT32) value);
            break;
        }
        case PERFGRP_MCU0:
        case PERFGRP_MCU1:
        {
            UINT32 mcuoffset = (group-PERFGRP_MCU0)*(HUP_CHIP_MCU1_ADDRESS-HUP_CHIP_MCU0_ADDRESS);
            dbg_write32 (__func__, HUP_CHIP_MCU0_PMB_REGS_PERF_PCNTR0_ADDRESS + (counter_num*8), mcuoffset, (UINT32) value);
            break;
        }
        case PERFGRP_IMP0:
        case PERFGRP_IMP1:
        {
            UINT32 impPerfOffset = (group == PERFGRP_IMP0) ? 0 : HUP_LSR_IMP_SIZE;
            dbg_write32 (__func__, HUP_CHIP_IMP0_PRV_PERF_PCNTR0_ADDRESS + (counter_num*8), impPerfOffset, (UINT32) value);
            break;
        }
        case PERFGRP_LSR0_SLOW:
        case PERFGRP_LSR1_SLOW:
        {
            UINT32 lsrPerfOffset = (group - PERFGRP_LSR0_SLOW) * (HUP_CHIP_LSR1_LSR_SLOW_ADDRESS - HUP_CHIP_LSR0_LSR_SLOW_ADDRESS);
            dbg_write32 (__func__, HUP_CHIP_LSR0_LSR_SLOW_SLOW_PRV_PERF_PCNTR0_ADDRESS + (counter_num*8), lsrPerfOffset, (UINT32) value);
            break;
        }
        case PERFGRP_LSR0_FAST:
        case PERFGRP_LSR1_FAST:
        {
            UINT32 lsrPerfOffset = (group - PERFGRP_LSR0_FAST) * (HUP_CHIP_LSR1_LSR_FAST_ADDRESS - HUP_CHIP_LSR0_LSR_FAST_ADDRESS);
            dbg_write32 (__func__, HUP_CHIP_LSR0_LSR_FAST_FAST_PRV_PERF_PCNTR0_ADDRESS + (counter_num*8), lsrPerfOffset, (UINT32) value);
            break;
        }
        case NUM_PERFGRP:
        default:
        {
            hw_errmsg ("%s: PERF_GROUP %d not valid\n", __func__, group);
            return PERFMON_STATUS_PERFGRP_INVALID;
        }
    }
    return PERFMON_STATUS_SUCCESS;
}

//
// Return the contents of the specified register
// Does NOT stop the counter
// TODO: Overflow detect, Interrupt on overflow
//
UINT64 perfmon_getPerfCounter     (UINT32     node        ,
                                   PERF_GROUP group       ,
                                   UINT32     counter_num )
{

    if (group > PERFGRP_NODE)
    {
        // All global counters should use the same node value when updating state
        node = 0;
    }

    if (! sanity_check_args (__func__, node, group, counter_num))
    {
        return -1;
    }

    HW_CORE currentOwner = (HW_CORE) hw_getCoreNum();    

    if (perf_cur_state [node][(int) group][counter_num].in_use && perf_cur_state[node][(int) group][counter_num].owner != currentOwner)
    {
        hw_errmsg ("%s: Core %d requesting Counter (node=%d, group=%d, cntr#=%d) which is in use by another core %d\n", __func__, currentOwner, node, group, counter_num, perf_cur_state[node][(int) group][counter_num].owner);
        return -1;
    }

    switch (group)
    {
        case PERFGRP_LX5_CORE0:
        case PERFGRP_LX5_CORE1:
        {
            UINT32 perfmon_base;
            UINT32 cntr_reg_offset;
            UINT64 cntr_reg_data;

            perfmon_base = get_lx5_perfmon_base (node, group);

            cntr_reg_offset = PERFMON_LX5_PM0_OFFSET + (counter_num * 4);

            // Read cntr register
            cntr_reg_data = (UINT64) dbg_read32 (__func__, perfmon_base, cntr_reg_offset);

            return cntr_reg_data;
        }
        case PERFGRP_NODE:
        {
            UINT64 cntr_reg_data;

            if (perf_cur_state [node][(int) group][counter_num].feature_mask & FEATURE_GLOBAL_SNAPSHOT)
            {
                cntr_reg_data = (UINT64) dbg_read32 (__func__, get_node_base (node, group), HUP_CN_MISC_PERF_PSNPS0_OFFSET + (counter_num * 8));
            }
            else
            {
                cntr_reg_data = (UINT64) dbg_read32 (__func__, get_node_base (node, group), HUP_CN_MISC_PERF_PCNTR0_OFFSET + (counter_num * 8));
            }

            return cntr_reg_data;
        }

        case PERFGRP_INTC:
        {
            UINT64 cntr_reg_data;

            if (perf_cur_state [node][(int) group][counter_num].feature_mask & FEATURE_GLOBAL_SNAPSHOT)
            {
                cntr_reg_data = dbg_read32(__func__, HUP_CHIP_INTC_ADDRESS, HUP_INTC_PERF_PSNPS0_OFFSET + (counter_num * 8));
            }
            else
            {
                cntr_reg_data = dbg_read32(__func__, HUP_CHIP_INTC_ADDRESS, HUP_INTC_PERF_PCNTR0_OFFSET + (counter_num * 8));
            }
            return cntr_reg_data;
        }
        case PERFGRP_CLSRAM:
        {
            UINT64 cntr_reg_data;

            if (perf_cur_state [node][(int) group][counter_num].feature_mask & FEATURE_GLOBAL_SNAPSHOT)
            {
                cntr_reg_data = (UINT64) dbg_read32 (__func__, HUP_CHIP_CLSRAM_ADDRESS, HUP_CLSRAM_MISC_PERF_PSNPS0_OFFSET + (counter_num * 8));
            }
            else
            {
                cntr_reg_data = (UINT64) dbg_read32 (__func__, HUP_CHIP_CLSRAM_ADDRESS, HUP_CLSRAM_MISC_PERF_PCNTR0_OFFSET + (counter_num * 8));
            }

            return cntr_reg_data;
        }

        case PERFGRP_MIP0:
        case PERFGRP_MIP1:
        case PERFGRP_MIP2:
        case PERFGRP_MIP3:
        case PERFGRP_MIP4:
        case PERFGRP_MIP5:
        case PERFGRP_MIP6:
        case PERFGRP_TIP:
        {
            UINT32 isp_base_addr = isp_block_base_addr[group-PERFGRP_MIP0];
            UINT64 cntr_reg_data;

            if (perf_cur_state [node][(int) group][counter_num].feature_mask & FEATURE_GLOBAL_SNAPSHOT)
            {
                cntr_reg_data = (UINT64) dbg_read32(__func__, isp_base_addr, HUP_ISP_MIP_MIP_PRV_BASE_PERF_PSNPS0_OFFSET + (counter_num * 8));
            }
            else
            {
                cntr_reg_data = (UINT64) dbg_read32(__func__, isp_base_addr, HUP_ISP_MIP_MIP_PRV_BASE_PERF_PCNTR0_OFFSET + (counter_num * 8));
            }
            return cntr_reg_data;            
        }
        case PERFGRP_JBL:
        {
            UINT64 cntr_reg_data;

            if (perf_cur_state [node][(int) group][counter_num].feature_mask & FEATURE_GLOBAL_SNAPSHOT)
            {
                cntr_reg_data = dbg_read32(__func__, HUP_CHIP_JBL_ADDRESS, HUP_JBL_PERF_PSNPS0_OFFSET + (counter_num * 8));
            }
            else
            {
                cntr_reg_data = dbg_read32(__func__, HUP_CHIP_JBL_ADDRESS, HUP_JBL_PERF_PCNTR0_OFFSET + (counter_num * 8));
            }
            return cntr_reg_data;            
        }
        case PERFGRP_FM_MC0_Q0:
        case PERFGRP_FM_MC0_Q1:
        case PERFGRP_FM_MC0_Q2:
        case PERFGRP_FM_MC0_Q3:
        case PERFGRP_FM_MC0_Q4:
        case PERFGRP_FM_MC0_Q5:
        case PERFGRP_FM_MC0_Q6:
        case PERFGRP_FM_MC0_Q7:
        case PERFGRP_FM_MC0_Q8:
        case PERFGRP_FM_MC0_Q9:
        case PERFGRP_FM_MC0_Q10:
        case PERFGRP_FM_MC0_Q11:
        case PERFGRP_FM_MC1_Q0:
        case PERFGRP_FM_MC1_Q1:
        case PERFGRP_FM_MC1_Q2:
        case PERFGRP_FM_MC1_Q3:
        case PERFGRP_FM_MC1_Q4:
        case PERFGRP_FM_MC1_Q5:
        case PERFGRP_FM_MC1_Q6:
        case PERFGRP_FM_MC1_Q7:
        case PERFGRP_FM_MC1_Q8:
        case PERFGRP_FM_MC1_Q9:
        case PERFGRP_FM_MC1_Q10:
        case PERFGRP_FM_MC1_Q11:
        case PERFGRP_FM_BASRAM0:
        case PERFGRP_FM_BASRAM1:
        case PERFGRP_FM_BASRAM2:
        case PERFGRP_FM_PCIE:
        {
            // Add FM implementation here
            UINT32 fmbase_addr = fm_block_base_addr [group-PERFGRP_FM_MC0_Q0];
            UINT64 cntr_reg_data;

            if (perf_cur_state [node][(int) group][counter_num].feature_mask & FEATURE_GLOBAL_SNAPSHOT)
            {
                cntr_reg_data = dbg_read32(__func__, fmbase_addr, HUP_FM_AXI_PERF_PSNPS0_OFFSET + (counter_num * 8));
            }
            else
            {
                cntr_reg_data = dbg_read32(__func__, fmbase_addr, HUP_FM_AXI_PERF_PCNTR0_OFFSET + (counter_num * 8));
            }
            return cntr_reg_data;
        }
        case PERFGRP_DNNSRAM:
        case PERFGRP_DNN_250:
        case PERFGRP_DNN_500:
        {
            UINT64 cntr_reg_data;
            UINT32 pcntr_addr;
            if (group == PERFGRP_DNN_250)
            {
                if (perf_cur_state [node][(int) group][counter_num].feature_mask & FEATURE_GLOBAL_SNAPSHOT)                
                {
                    pcntr_addr = HUP_CHIP_DNNSRAM_DNN_PERF_250_PERF_PSNPS0_ADDRESS;
                }
                else
                {
                    pcntr_addr = HUP_CHIP_DNNSRAM_DNN_PERF_250_PERF_PCNTR0_ADDRESS;
                }
            }
            else if (group == PERFGRP_DNN_500)
            {
                if (perf_cur_state [node][(int) group][counter_num].feature_mask & FEATURE_GLOBAL_SNAPSHOT)                
                {
                    pcntr_addr = HUP_CHIP_DNNSRAM_DNN_PERF_500_PERF_PSNPS0_ADDRESS;
                }
                else
                {
                    pcntr_addr = HUP_CHIP_DNNSRAM_DNN_PERF_500_PERF_PCNTR0_ADDRESS;
                }
            }
            else
            {
                if (perf_cur_state [node][(int) group][counter_num].feature_mask & FEATURE_GLOBAL_SNAPSHOT)                
                {
                    pcntr_addr = HUP_CHIP_DNNSRAM_PERF_PSNPS0_ADDRESS;
                }
                else
                {
                    pcntr_addr = HUP_CHIP_DNNSRAM_PERF_PCNTR0_ADDRESS;
                }
            }
            cntr_reg_data = dbg_read32(__func__, pcntr_addr, (counter_num * 8));
            return cntr_reg_data;
        }
        case PERFGRP_MCU0:
        case PERFGRP_MCU1:
        {
            UINT32 mcuoffset = (group-PERFGRP_MCU0)*(HUP_CHIP_MCU1_ADDRESS-HUP_CHIP_MCU0_ADDRESS);
            UINT64 cntr_reg_data;
            if (perf_cur_state [node][(int) group][counter_num].feature_mask & FEATURE_GLOBAL_SNAPSHOT)
            {
                cntr_reg_data = dbg_read32 (__func__, HUP_CHIP_MCU0_PMB_REGS_PERF_PSNPS0_ADDRESS + (counter_num*8), mcuoffset);
            }
            else
            {
                cntr_reg_data = dbg_read32 (__func__, HUP_CHIP_MCU0_PMB_REGS_PERF_PCNTR0_ADDRESS + (counter_num*8), mcuoffset);
            }
            return cntr_reg_data;
        }
        case PERFGRP_IMP0:
        case PERFGRP_IMP1:
        {
            UINT32 impPerfOffset = (group == PERFGRP_IMP0) ? 0 : HUP_LSR_IMP_SIZE;
            UINT64 cntr_reg_data = 0;
            if (perf_cur_state [node][(int) group][counter_num].feature_mask & FEATURE_GLOBAL_SNAPSHOT)
            {
                cntr_reg_data = dbg_read32 (__func__, HUP_CHIP_IMP0_PRV_PERF_PSNPS0_ADDRESS + (counter_num*8), impPerfOffset);
            }
            else
            {
                cntr_reg_data = dbg_read32 (__func__, HUP_CHIP_IMP0_PRV_PERF_PCNTR0_ADDRESS + (counter_num*8), impPerfOffset);
            }
            return cntr_reg_data;
        }
        case PERFGRP_LSR0_SLOW:
        case PERFGRP_LSR1_SLOW:
        {
            UINT32 lsrPerfOffset = (group - PERFGRP_LSR0_SLOW) * (HUP_CHIP_LSR1_LSR_SLOW_ADDRESS - HUP_CHIP_LSR0_LSR_SLOW_ADDRESS);
            UINT64 cntr_reg_data = 0;
            
            if (perf_cur_state [node][(int) group][counter_num].feature_mask & FEATURE_GLOBAL_SNAPSHOT)
            {
                cntr_reg_data = dbg_read32 (__func__, HUP_CHIP_LSR0_LSR_SLOW_SLOW_PRV_PERF_PSNPS0_ADDRESS + (counter_num*8), lsrPerfOffset);
            }
            else
            {
                cntr_reg_data = dbg_read32 (__func__, HUP_CHIP_LSR0_LSR_SLOW_SLOW_PRV_PERF_PCNTR0_ADDRESS + (counter_num*8), lsrPerfOffset);
            }
            return cntr_reg_data;
            
            break;
        }
        case PERFGRP_LSR0_FAST:
        case PERFGRP_LSR1_FAST:
        {
            UINT32 lsrPerfOffset = (group - PERFGRP_LSR0_FAST) * (HUP_CHIP_LSR1_LSR_FAST_ADDRESS - HUP_CHIP_LSR0_LSR_FAST_ADDRESS);
            UINT64 cntr_reg_data = 0;
            
            if (perf_cur_state [node][(int) group][counter_num].feature_mask & FEATURE_GLOBAL_SNAPSHOT)
            {
                cntr_reg_data = dbg_read32 (__func__, HUP_CHIP_LSR0_LSR_FAST_FAST_PRV_PERF_PSNPS0_ADDRESS + (counter_num*8), lsrPerfOffset);
            }
            else
            {
                cntr_reg_data = dbg_read32 (__func__, HUP_CHIP_LSR0_LSR_FAST_FAST_PRV_PERF_PCNTR0_ADDRESS + (counter_num*8), lsrPerfOffset);
            }
            return cntr_reg_data;
            
            break;
        }
        case NUM_PERFGRP:
        default:
        {
            hw_errmsg ("%s: PERF_GROUP %d not valid\n", __func__, group);
            return 0xdeadbeef;
        }
    }
}

//
// Enable specified counter and kick-off counting
//
HW_STATUSCODE   perfmon_enablePerfCounter  (UINT32     node        ,
                                            PERF_GROUP group       ,
                                            UINT32     counter_num )
{

    if (group > PERFGRP_NODE)
    {
        // All global counters should use the same node value when updating state
        node = 0;
    }

    if (! sanity_check_args (__func__, node, group, counter_num))
    {
        return PERFMON_STATUS_SANITY_FAIL;
    }

    HW_CORE currentOwner = (HW_CORE) hw_getCoreNum();    

    if (perf_cur_state [node][(int) group][counter_num].in_use && perf_cur_state[node][(int) group][counter_num].owner != currentOwner)
    {
        hw_errmsg ("%s: Core %d requesting Counter (node=%d, group=%d, cntr#=%d) which is in use by another core %d\n", __func__, currentOwner, node, group, counter_num, perf_cur_state[node][(int) group][counter_num].owner);
        return PERFMON_STATUS_COUNTER_LOCKED;
    }

    switch (group)
    {
        case PERFGRP_LX5_CORE0:
        case PERFGRP_LX5_CORE1:
        {
            UINT32 perfmon_base;
            UINT32 pmg_reg_offset;

            perfmon_base = get_lx5_perfmon_base (node, group);

            pmg_reg_offset = PERFMON_LX5_PMG_OFFSET;

            dbg_write32 (__func__, perfmon_base, pmg_reg_offset, 1);

            break;
        }
        case PERFGRP_NODE:
        case PERFGRP_CLSRAM:
        {
            UINT32 perf_ctrl_data;

            // Shared register, should be accessed atomically

            ke_spin_lock(&hw_perfmonLock);

            perf_ctrl_data = dbg_read32 (__func__, get_node_base (node, group), HUP_CN_MISC_PERF_PCTRL_OFFSET);

            dbg_write32 (__func__, get_node_base (node, group), HUP_CN_MISC_PERF_PCTRL_OFFSET, SET_NODE_PERF_PCTRL_EN (perf_ctrl_data, counter_num, 1));

            ke_spin_unlock(&hw_perfmonLock);

            break;
        }
        case PERFGRP_INTC:
        {
            UINT32 perf_ctrl_data;

            perf_ctrl_data = dbg_read32 (__func__, HUP_CHIP_INTC_ADDRESS,
                                         HUP_INTC_PERF_PCTRL_OFFSET);
            perf_ctrl_data |= 1<<counter_num;
            dbg_write32 (__func__, HUP_CHIP_INTC_ADDRESS, HUP_INTC_PERF_PCTRL_OFFSET,
                         perf_ctrl_data);

            break;
        }
        case PERFGRP_MIP0:
        case PERFGRP_MIP1:
        case PERFGRP_MIP2:
        case PERFGRP_MIP3:
        case PERFGRP_MIP4:
        case PERFGRP_MIP5:
        case PERFGRP_MIP6:
        case PERFGRP_TIP:
        {
            UINT32 perf_ctrl_data;
            UINT32 isp_base_addr = isp_block_base_addr[group-PERFGRP_MIP0];

            perf_ctrl_data = dbg_read32 (__func__, isp_base_addr, HUP_ISP_MIP_MIP_PRV_BASE_PERF_PCTRL_OFFSET);
            perf_ctrl_data |= (1<<counter_num);
            dbg_write32 (__func__, isp_base_addr, HUP_ISP_MIP_MIP_PRV_BASE_PERF_PCTRL_OFFSET, perf_ctrl_data);
            break;
        }
        case PERFGRP_JBL:
        {
            UINT32 perf_ctrl_data; 
            
            perf_ctrl_data = dbg_read32 (__func__, HUP_CHIP_JBL_ADDRESS, HUP_JBL_PERF_PCTRL_OFFSET);
            perf_ctrl_data |= (1<<counter_num);
            dbg_write32 (__func__, HUP_CHIP_JBL_ADDRESS, HUP_JBL_PERF_PCTRL_OFFSET, perf_ctrl_data);
            break;
        }
        case PERFGRP_FM_MC0_Q0:
        case PERFGRP_FM_MC0_Q1:
        case PERFGRP_FM_MC0_Q2:
        case PERFGRP_FM_MC0_Q3:
        case PERFGRP_FM_MC0_Q4:
        case PERFGRP_FM_MC0_Q5:
        case PERFGRP_FM_MC0_Q6:
        case PERFGRP_FM_MC0_Q7:
        case PERFGRP_FM_MC0_Q8:
        case PERFGRP_FM_MC0_Q9:
        case PERFGRP_FM_MC0_Q10:
        case PERFGRP_FM_MC0_Q11:
        case PERFGRP_FM_MC1_Q0:
        case PERFGRP_FM_MC1_Q1:
        case PERFGRP_FM_MC1_Q2:
        case PERFGRP_FM_MC1_Q3:
        case PERFGRP_FM_MC1_Q4:
        case PERFGRP_FM_MC1_Q5:
        case PERFGRP_FM_MC1_Q6:
        case PERFGRP_FM_MC1_Q7:
        case PERFGRP_FM_MC1_Q8:
        case PERFGRP_FM_MC1_Q9:
        case PERFGRP_FM_MC1_Q10:
        case PERFGRP_FM_MC1_Q11:
        case PERFGRP_FM_BASRAM0:
        case PERFGRP_FM_BASRAM1:
        case PERFGRP_FM_BASRAM2:
        case PERFGRP_FM_PCIE:
        {
            // Add FM implementation here
            UINT32 fmbase_addr = fm_block_base_addr [group-PERFGRP_FM_MC0_Q0];
            UINT32 perf_ctrl_data; 
            
            perf_ctrl_data = dbg_read32 (__func__, fmbase_addr, HUP_FM_AXI_PERF_PCTRL_OFFSET);
            perf_ctrl_data |= (1<<counter_num);
            dbg_write32 (__func__, fmbase_addr, HUP_FM_AXI_PERF_PCTRL_OFFSET, perf_ctrl_data);
            break;
        }
        case PERFGRP_DNNSRAM:
        case PERFGRP_DNN_250:
        case PERFGRP_DNN_500:
        {
            UINT32 pctrl_addr;
            UINT32 perf_ctrl_data;
            if (group == PERFGRP_DNN_250)
            {
                pctrl_addr = HUP_CHIP_DNNSRAM_DNN_PERF_250_PERF_PCTRL_ADDRESS;
            }
            else if (group == PERFGRP_DNN_500)
            {
                pctrl_addr = HUP_CHIP_DNNSRAM_DNN_PERF_500_PERF_PCTRL_ADDRESS;
            }
            else
            {
                pctrl_addr = HUP_CHIP_DNNSRAM_PERF_PCTRL_ADDRESS;
            }
            perf_ctrl_data = dbg_read32 (__func__, pctrl_addr, 0);
            perf_ctrl_data |= (1<<counter_num);
            dbg_write32 (__func__, pctrl_addr, 0, perf_ctrl_data);
            break;
        }
        case PERFGRP_MCU0:
        case PERFGRP_MCU1:
        {
            UINT32 mcuoffset = (group-PERFGRP_MCU0)*(HUP_CHIP_MCU1_ADDRESS-HUP_CHIP_MCU0_ADDRESS);
            UINT32 perf_ctrl_data;
            perf_ctrl_data = dbg_read32 (__func__, HUP_CHIP_MCU0_PMB_REGS_PERF_PCTRL_ADDRESS, mcuoffset);
            perf_ctrl_data |= (1<<counter_num);
            dbg_write32 (__func__, HUP_CHIP_MCU0_PMB_REGS_PERF_PCTRL_ADDRESS, mcuoffset, perf_ctrl_data);
            break;
        }
        case PERFGRP_IMP0:
        case PERFGRP_IMP1:
        {
            UINT32 impPerfOffset = (group == PERFGRP_IMP0) ? 0 : HUP_LSR_IMP_SIZE;
            UINT32 perf_ctrl_data;
            perf_ctrl_data = dbg_read32 (__func__, HUP_CHIP_IMP0_PRV_PERF_PCTRL_ADDRESS, impPerfOffset);
            perf_ctrl_data |= (1<<counter_num);
            dbg_write32 (__func__, HUP_CHIP_IMP0_PRV_PERF_PCTRL_ADDRESS, impPerfOffset, perf_ctrl_data);
            break;
        }
        case PERFGRP_LSR0_SLOW:
        case PERFGRP_LSR1_SLOW:
        {
            UINT32 lsrPerfOffset = (group - PERFGRP_LSR0_SLOW) * (HUP_CHIP_LSR1_LSR_SLOW_ADDRESS - HUP_CHIP_LSR0_LSR_SLOW_ADDRESS);
            UINT32 perf_ctrl_data;
            perf_ctrl_data = dbg_read32 (__func__, HUP_CHIP_LSR0_LSR_SLOW_SLOW_PRV_PERF_PCTRL_ADDRESS, lsrPerfOffset);
            perf_ctrl_data |= (1<<counter_num);
            dbg_write32 (__func__, HUP_CHIP_LSR0_LSR_SLOW_SLOW_PRV_PERF_PCTRL_ADDRESS, lsrPerfOffset, perf_ctrl_data);
            break;
        }
        case PERFGRP_LSR0_FAST:
        case PERFGRP_LSR1_FAST:
        {
            UINT32 lsrPerfOffset = (group - PERFGRP_LSR0_FAST) * (HUP_CHIP_LSR1_LSR_FAST_ADDRESS - HUP_CHIP_LSR0_LSR_FAST_ADDRESS);
            UINT32 perf_ctrl_data;
            perf_ctrl_data = dbg_read32 (__func__, HUP_CHIP_LSR0_LSR_FAST_FAST_PRV_PERF_PCTRL_ADDRESS, lsrPerfOffset);
            perf_ctrl_data |= (1<<counter_num);
            dbg_write32 (__func__, HUP_CHIP_LSR0_LSR_FAST_FAST_PRV_PERF_PCTRL_ADDRESS, lsrPerfOffset, perf_ctrl_data);
            break;
        }
        case NUM_PERFGRP:
        default:
        {
            hw_errmsg ("%s: PERF_GROUP %d not valid\n", __func__, group);
            return PERFMON_STATUS_PERFGRP_INVALID;
        }
    }
   
    return PERFMON_STATUS_SUCCESS;
}

//
// Stop specified counter and disable counting
//
HW_STATUSCODE   perfmon_disablePerfCounter (UINT32     node        ,
                                            PERF_GROUP group       ,
                                            UINT32     counter_num )
{

    if (group > PERFGRP_NODE)
    {
        // All global counters should use the same node value when updating state
        node = 0;
    }

    if (! sanity_check_args (__func__, node, group, counter_num))
    {
        return PERFMON_STATUS_SANITY_FAIL;
    }

    HW_CORE currentOwner = (HW_CORE) hw_getCoreNum();    

    if (perf_cur_state [node][(int) group][counter_num].in_use && perf_cur_state[node][(int) group][counter_num].owner != currentOwner)
    {
        hw_errmsg ("%s: Core %d requesting Counter (node=%d, group=%d, cntr#=%d) which is in use by another core %d\n", __func__, currentOwner, node, group, counter_num, perf_cur_state[node][(int) group][counter_num].owner);
        return PERFMON_STATUS_COUNTER_LOCKED;
    }

    switch (group)
    {
        case PERFGRP_LX5_CORE0:
        case PERFGRP_LX5_CORE1:
        {
            UINT32 perfmon_base;
            UINT32 pmg_reg_offset;
            UINT32 pmg_reg_data;

            perfmon_base = get_lx5_perfmon_base (node, group);

            pmg_reg_offset = PERFMON_LX5_PMG_OFFSET;

            // Write 0 to stop ALL counters (counter_num is irrelevant)
            pmg_reg_data = dbg_read32 (__func__, perfmon_base, pmg_reg_offset);

            if (pmg_reg_data & 0x1 == 1)
            {
                dbg_write32 (__func__, perfmon_base, pmg_reg_offset, 0);
            }

            break;
        }
        case PERFGRP_NODE:
        case PERFGRP_CLSRAM:
        {
            UINT32 perf_ctrl_data;

            perf_ctrl_data = dbg_read32 (__func__, get_node_base (node, group), HUP_CN_MISC_PERF_PCTRL_OFFSET);

            dbg_write32 (__func__, get_node_base (node, group), HUP_CN_MISC_PERF_PCTRL_OFFSET, SET_NODE_PERF_PCTRL_EN (perf_ctrl_data, counter_num, 0));

            break;
        }
        case PERFGRP_INTC:
        {
            UINT32 perf_ctrl_data;

            perf_ctrl_data = dbg_read32 (__func__, HUP_CHIP_INTC_ADDRESS,
                                         HUP_INTC_PERF_PCTRL_OFFSET);
            perf_ctrl_data &= ~(1<<counter_num);
            dbg_write32 (__func__, HUP_CHIP_INTC_ADDRESS, HUP_INTC_PERF_PCTRL_OFFSET,
                         perf_ctrl_data);

            break;
        }
        case PERFGRP_MIP0:
        case PERFGRP_MIP1:
        case PERFGRP_MIP2:
        case PERFGRP_MIP3:
        case PERFGRP_MIP4:
        case PERFGRP_MIP5:
        case PERFGRP_MIP6:
        case PERFGRP_TIP:
        {
            UINT32 perf_ctrl_data;
            UINT32 isp_base_addr = isp_block_base_addr[group-PERFGRP_MIP0];

            perf_ctrl_data = dbg_read32 (__func__, isp_base_addr, HUP_ISP_MIP_MIP_PRV_BASE_PERF_PCTRL_OFFSET);
            perf_ctrl_data &= ~(1<<counter_num);
            dbg_write32 (__func__, isp_base_addr, HUP_ISP_MIP_MIP_PRV_BASE_PERF_PCTRL_OFFSET, perf_ctrl_data);
            
            break;
        }
        case PERFGRP_JBL:
        {
            UINT32 perf_ctrl_data; 
            
            perf_ctrl_data = dbg_read32 (__func__, HUP_CHIP_JBL_ADDRESS, HUP_JBL_PERF_PCTRL_OFFSET);
            perf_ctrl_data &= ~(1<<counter_num);
            dbg_write32 (__func__, HUP_CHIP_JBL_ADDRESS, HUP_JBL_PERF_PCTRL_OFFSET, perf_ctrl_data);
            break;
        }
        case PERFGRP_FM_MC0_Q0:
        case PERFGRP_FM_MC0_Q1:
        case PERFGRP_FM_MC0_Q2:
        case PERFGRP_FM_MC0_Q3:
        case PERFGRP_FM_MC0_Q4:
        case PERFGRP_FM_MC0_Q5:
        case PERFGRP_FM_MC0_Q6:
        case PERFGRP_FM_MC0_Q7:
        case PERFGRP_FM_MC0_Q8:
        case PERFGRP_FM_MC0_Q9:
        case PERFGRP_FM_MC0_Q10:
        case PERFGRP_FM_MC0_Q11:
        case PERFGRP_FM_MC1_Q0:
        case PERFGRP_FM_MC1_Q1:
        case PERFGRP_FM_MC1_Q2:
        case PERFGRP_FM_MC1_Q3:
        case PERFGRP_FM_MC1_Q4:
        case PERFGRP_FM_MC1_Q5:
        case PERFGRP_FM_MC1_Q6:
        case PERFGRP_FM_MC1_Q7:
        case PERFGRP_FM_MC1_Q8:
        case PERFGRP_FM_MC1_Q9:
        case PERFGRP_FM_MC1_Q10:
        case PERFGRP_FM_MC1_Q11:
        case PERFGRP_FM_BASRAM0:
        case PERFGRP_FM_BASRAM1:
        case PERFGRP_FM_BASRAM2:
        case PERFGRP_FM_PCIE:
        {
            // Add FM implementation here
            UINT32 fmbase_addr = fm_block_base_addr [group-PERFGRP_FM_MC0_Q0];
            UINT32 perf_ctrl_data; 
            
            perf_ctrl_data = dbg_read32 (__func__, fmbase_addr, HUP_FM_AXI_PERF_PCTRL_OFFSET);
            perf_ctrl_data &= ~(1<<counter_num);
            dbg_write32 (__func__, fmbase_addr, HUP_FM_AXI_PERF_PCTRL_OFFSET, perf_ctrl_data);
            break;
        }
        case PERFGRP_DNNSRAM:
        case PERFGRP_DNN_250:
        case PERFGRP_DNN_500:
        {
            UINT32 pctrl_addr;
            UINT32 perf_ctrl_data;
            if (group == PERFGRP_DNN_250)
            {
                pctrl_addr = HUP_CHIP_DNNSRAM_DNN_PERF_250_PERF_PCTRL_ADDRESS;
            }
            else if (group == PERFGRP_DNN_500)
            {
                pctrl_addr = HUP_CHIP_DNNSRAM_DNN_PERF_500_PERF_PCTRL_ADDRESS;
            }
            else
            {
                pctrl_addr = HUP_CHIP_DNNSRAM_PERF_PCTRL_ADDRESS;
            }
            perf_ctrl_data = dbg_read32 (__func__, pctrl_addr, 0);
            perf_ctrl_data &= ~(1<<counter_num);
            dbg_write32 (__func__, pctrl_addr, 0, perf_ctrl_data);
            break;
        }
        case PERFGRP_MCU0:
        case PERFGRP_MCU1:
        {
            UINT32 mcuoffset = (group-PERFGRP_MCU0)*(HUP_CHIP_MCU1_ADDRESS-HUP_CHIP_MCU0_ADDRESS);
            UINT32 perf_ctrl_data;
            perf_ctrl_data = dbg_read32 (__func__, HUP_CHIP_MCU0_PMB_REGS_PERF_PCTRL_ADDRESS, mcuoffset);
            perf_ctrl_data &= ~(1<<counter_num);
            dbg_write32 (__func__, HUP_CHIP_MCU0_PMB_REGS_PERF_PCTRL_ADDRESS, mcuoffset, perf_ctrl_data);
            break;
        }
        case PERFGRP_IMP0:
        case PERFGRP_IMP1:
        {
            UINT32 impPerfOffset = (group == PERFGRP_IMP0) ? 0 : HUP_LSR_IMP_SIZE;
            UINT32 perf_ctrl_data;
            perf_ctrl_data = dbg_read32 (__func__, HUP_CHIP_IMP0_PRV_PERF_PCTRL_ADDRESS, impPerfOffset);
            perf_ctrl_data &= ~(1<<counter_num);
            dbg_write32 (__func__, HUP_CHIP_IMP0_PRV_PERF_PCTRL_ADDRESS, impPerfOffset, perf_ctrl_data);
            break;
        }
        case PERFGRP_LSR0_SLOW:
        case PERFGRP_LSR1_SLOW:
        {
            UINT32 lsrPerfOffset = (group - PERFGRP_LSR0_SLOW) * (HUP_CHIP_LSR1_LSR_SLOW_ADDRESS - HUP_CHIP_LSR0_LSR_SLOW_ADDRESS);
            UINT32 perf_ctrl_data;
            perf_ctrl_data = dbg_read32 (__func__, HUP_CHIP_LSR0_LSR_SLOW_SLOW_PRV_PERF_PCTRL_ADDRESS, lsrPerfOffset);
            perf_ctrl_data &= ~(1<<counter_num);
            dbg_write32 (__func__, HUP_CHIP_LSR0_LSR_SLOW_SLOW_PRV_PERF_PCTRL_ADDRESS, lsrPerfOffset, perf_ctrl_data);
            break;
        }
        case PERFGRP_LSR0_FAST:
        case PERFGRP_LSR1_FAST:
        {
            UINT32 lsrPerfOffset = (group - PERFGRP_LSR0_FAST) * (HUP_CHIP_LSR1_LSR_FAST_ADDRESS - HUP_CHIP_LSR0_LSR_FAST_ADDRESS);
            UINT32 perf_ctrl_data;
            perf_ctrl_data = dbg_read32 (__func__, HUP_CHIP_LSR0_LSR_FAST_FAST_PRV_PERF_PCTRL_ADDRESS, lsrPerfOffset);
            perf_ctrl_data &= ~(1<<counter_num);
            dbg_write32 (__func__, HUP_CHIP_LSR0_LSR_FAST_FAST_PRV_PERF_PCTRL_ADDRESS, lsrPerfOffset, perf_ctrl_data);
            break;
        }
        case NUM_PERFGRP:
        default:
        {
            hw_errmsg ("%s: PERF_GROUP %d not valid\n", __func__, group);
            return PERFMON_STATUS_PERFGRP_INVALID;
        }
    }
    
    perf_cur_state [node][(int) group][counter_num].in_use = false;
    hw_cacheFlushAddr (&perf_cur_state [node][(int) group][counter_num]);

    return PERFMON_STATUS_SUCCESS;
}

//
// Return the number of physical counters for specified device type
//
UINT32 perfmon_getNumHwCounters   (PERF_GROUP group       )
{
    switch (group)
    {
        case PERFGRP_LX5_CORE0:
        case PERFGRP_LX5_CORE1:
        case PERFGRP_NODE:
        case PERFGRP_MCU0:
        case PERFGRP_MCU1:
        case PERFGRP_LSR0_FAST:
        case PERFGRP_LSR1_FAST:
        case PERFGRP_DNNSRAM:
        case PERFGRP_DNN_250:
        case PERFGRP_DNN_500:
        case PERFGRP_CLSRAM:
        case PERFGRP_FM_MC0_Q0:
        case PERFGRP_FM_MC0_Q1:
        case PERFGRP_FM_MC0_Q2:
        case PERFGRP_FM_MC0_Q3:
        case PERFGRP_FM_MC0_Q4:
        case PERFGRP_FM_MC0_Q5:
        case PERFGRP_FM_MC0_Q6:
        case PERFGRP_FM_MC0_Q7:
        case PERFGRP_FM_MC0_Q8:
        case PERFGRP_FM_MC0_Q9:
        case PERFGRP_FM_MC0_Q10:
        case PERFGRP_FM_MC0_Q11:
        case PERFGRP_FM_MC1_Q0:
        case PERFGRP_FM_MC1_Q1:
        case PERFGRP_FM_MC1_Q2:
        case PERFGRP_FM_MC1_Q3:
        case PERFGRP_FM_MC1_Q4:
        case PERFGRP_FM_MC1_Q5:
        case PERFGRP_FM_MC1_Q6:
        case PERFGRP_FM_MC1_Q7:
        case PERFGRP_FM_MC1_Q8:
        case PERFGRP_FM_MC1_Q9:
        case PERFGRP_FM_MC1_Q10:
        case PERFGRP_FM_MC1_Q11:
        case PERFGRP_FM_BASRAM0:
        case PERFGRP_FM_BASRAM1:
        case PERFGRP_FM_BASRAM2:
        case PERFGRP_FM_PCIE:
            return 8;
        case PERFGRP_INTC:
        case PERFGRP_JBL:
        case PERFGRP_IMP0:
        case PERFGRP_IMP1:
        case PERFGRP_MIP0:
        case PERFGRP_MIP1:
        case PERFGRP_MIP2:
        case PERFGRP_MIP3:
        case PERFGRP_MIP4:
        case PERFGRP_MIP5:
        case PERFGRP_MIP6:
        case PERFGRP_TIP:
        case PERFGRP_LSR0_SLOW:
        case PERFGRP_LSR1_SLOW:
            return 4;

        default:
            hw_errmsg ("%s: Invalid group type %d passed as an argument\n", __func__, group);
            return (UINT32) -1;
    }
}

//
// Release global stop by writing to the POR register
// IMPORTANT: A test writer HAS to call this in order to enable counters
// 
EXTERN_C VOID   perfmon_releaseGlobalStop ()
{
    UINT32 glblPerfIntc1 = dbg_read32 (__func__, HUP_CHIP_POR_ADDRESS, HUP_POR_GLOBALPERFORMANCEINTC1_OFFSET); 
    dbg_write32 (__func__, HUP_CHIP_POR_ADDRESS, HUP_POR_GLOBALPERFORMANCEINTC1_OFFSET, glblPerfIntc1 & ~HUP_POR_GLOBALPERFORMANCEINTC1_GLB_PERF_STOP_FIELD_MASK);
}

//
// Trigger global stop by writing to the POR register
// 
EXTERN_C VOID   perfmon_applyGlobalStop ()
{
    UINT32 glblPerfIntc1 = dbg_read32 (__func__, HUP_CHIP_POR_ADDRESS, HUP_POR_GLOBALPERFORMANCEINTC1_OFFSET); 
    dbg_write32 (__func__, HUP_CHIP_POR_ADDRESS, HUP_POR_GLOBALPERFORMANCEINTC1_OFFSET, glblPerfIntc1 | HUP_POR_GLOBALPERFORMANCEINTC1_GLB_PERF_STOP_FIELD_MASK);
}

//
// Trigger global snapshot by writing to the POR register
// 
EXTERN_C VOID   perfmon_applyGlobalSnapshot ()
{
    UINT32 glblPerfIntc2 = dbg_read32 (__func__, HUP_CHIP_POR_ADDRESS, HUP_POR_GLOBALPERFORMANCEINTC2_OFFSET); 
    dbg_write32 (__func__, HUP_CHIP_POR_ADDRESS, HUP_POR_GLOBALPERFORMANCEINTC2_OFFSET, glblPerfIntc2 | HUP_POR_GLOBALPERFORMANCEINTC2_GLB_PERF_SNAP_FIELD_MASK);
}
