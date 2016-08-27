/*-----------------------------------------------------
 |
 |    td_tempmon.c
 |    ============
 |    Contains JTM temperature monitoring module
 |    Reports on-chip temperature
 | 
 |------------------------------------------------------
 |
 |    Copyright (C) 2014  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

#include "bifrost.h"
#include "tsb_regs.h"
#include "jtm_reg_def.h"

static FLOAT32 gSlope = 0.334;
static FLOAT32 gOffset = -25.3;

VOID startJTM (int inst_id)
{
    UINT32 jtm_base =  LAHAINA_JTM_BASE + inst_id * 0x100;
    // Read CTRL REG and turn off JTM_EN bits
    UINT32 jtm_ctrl_reg = hw_reg_read32 (jtm_base, JTM_JTM_CTRL_0_OFS);
    UINT64 cur_time;
    UINT64 start_time = hw_getTime();

    hw_reg_write32 (jtm_base, JTM_JTM_CTRL_0_OFS, jtm_ctrl_reg & ~JTM_JTM_CTRL_0_JTM_EN_MASK);

    while ((JTM_JTM_CTRL_0_JTM_EN_MASK & (jtm_ctrl_reg = hw_reg_read32 (jtm_base, JTM_JTM_CTRL_0_OFS))))
    {
        cur_time = hw_getTime();
        // Wait 1 second, and then timeout
        if (cur_time - start_time >= 1000000000)
        {
            hw_errmsg ("%s: Error: Timeout in writing JTM_CTRL_%d register\n", __func__, inst_id);
            return;
        }
    }

    hw_status ("%s: JTM disabled: JTM_CTRL_REG = 0x%08X\n", __func__, jtm_ctrl_reg);

    // Re-enable JTM_EN 
    jtm_ctrl_reg |= JTM_JTM_CTRL_0_JTM_EN_MASK;

    hw_reg_write32 (jtm_base, JTM_JTM_CTRL_0_OFS, jtm_ctrl_reg);

    start_time = cur_time;

    while (! (JTM_JTM_CTRL_0_JTM_EN_MASK & (jtm_ctrl_reg = hw_reg_read32 (jtm_base, JTM_JTM_CTRL_0_OFS))))
    {
        cur_time = hw_getTime();
        // Wait 1 second, and then timeout
        if (cur_time - start_time >= 1000000000)
        {
            hw_errmsg ("%s: Error: Timeout in writing JTM_CTRL_%d register\n", __func__, inst_id);
            return;
        }
    }

    hw_status ("%s: JTM re-enabled: JTM_CTRL_REG = 0x%08X\n", __func__, jtm_ctrl_reg);    
}

VOID reportJTM (int inst_id)
{
    UINT32 jtm_base =  LAHAINA_JTM_BASE + inst_id * 0x100;
 
    // Read CTRL REG and make sure JTM_EN is set
    UINT32 jtm_ctrl_reg = hw_reg_read32 (jtm_base, JTM_JTM_CTRL_0_OFS);
    if (! (jtm_ctrl_reg & JTM_JTM_CTRL_0_JTM_EN_MASK))
    {
        hw_errmsg ("%s: Error: JTM_EN bit of JTM_CTRL reg not set: 0x%08X\n", __func__, jtm_ctrl_reg);
        return;
    }

    // If enabled, read out from TEMP register
    UINT32 jtm_temp_reg = hw_reg_read32 (jtm_base, JTM_JTM_TEMP_0_OFS);
    
    if (jtm_temp_reg & JTM_JTM_TEMP_0_TOK_MASK)
    {
        UINT32 jtm_raw_temp = (jtm_temp_reg & JTM_JTM_TEMP_0_TEMP_MASK) >> JTM_JTM_TEMP_0_TEMP_SHIFT;
        FLOAT32 temp_in_c = (jtm_raw_temp * gSlope) + gOffset;

        hw_critical ("JTM[%d] = %0.2f [raw:%d] | ", inst_id, temp_in_c, jtm_raw_temp);
    }
    else 
    {
        hw_errmsg ("%s: Error: JTM_OK bit of JTM_TEMP_%d reg not set: 0x%08X\n", __func__, inst_id, jtm_temp_reg);
        return;
    }
}

VOID td_tempmon_init()
{
    INT32 slope = 0;
    INT32 offset = 0;
    hw_status ("%s: TEMP MON enabled, initializing\n", __func__);

    for (int i=0; i < JTM_NUM_INST; i++)
    {
        startJTM (i);
    }

    slope = hw_get_jtm_slope();
    if(slope != 0)
    {
        gSlope = (FLOAT32)slope / (FLOAT32)1000;
    }
    else
    {
        gSlope = 0.334;
    }

    offset = hw_get_jtm_offset();
    if(offset != 0)
    {
        gOffset = (FLOAT32)offset / (FLOAT32)1000;
    }
    else
    {
        gOffset = -25.3;
    }
}

VOID td_tempmon_report_temps (UINT64 cur_time)
{
    hw_status ("Time %lld: %s:        TEMP MON report\n", cur_time, __func__);
    hw_status ("Time %lld: %s: =============================\n",cur_time,  __func__);
    hw_critical ("Time %lld: %s: ", cur_time, __func__);

    for (int i=0; i < JTM_NUM_INST; i++)
    {
        reportJTM(i);
    }
    hw_critical ("\n");
}
