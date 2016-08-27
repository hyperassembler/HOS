/*-------------------------------------------------------
 |
 |    system_init.c
 |
 |    system specific initialization
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
#include <bifrost_lock.h>
#include "pcie.h"
#include "init_helper_common.h"
#include "hup_chip.h"
#include "por.h"

#define INTC_BASE_ADDR               0x04000000
#define INTC_P0_DATA0                0x0
#define INTC_SCRATCH_0_DATA          0x5000
#define INTC_SCRATCH_1_DATA          0x5010
#define INTC_SCRATCH_1_CLEAR         0x5018
#define INTC_SCRATCH_2_DATA          0x5020
#define INTC_SCRATCH_3_DATA          0x5030
#define INTC_SCRATCH_4_DATA          0x5040
#define INTC_SCRATCH_5_DATA          0x5050
#define INTC_SCRATCH_6_DATA          0x5060
#define INTC_SCRATCH_7_DATA          0x5070

#define BLOCK_TEST_MASK              0x80000000

extern void sys_init_pcie();
EXTERN_C void bss_clear_ASM(UINT32 table_start);
extern void sys_tsStall();

static const UINTPTR dramHeapInfo[1][3] __attribute__ ((aligned(4))) = {
    {
        1,
        HW_HEAP_DRAM_BASE,
        HW_HEAP_DRAM_BASE + HW_HEAP_DRAM_AVAIL_SIZE
    }
};

extern UINTPTR hw_ddr_sram_bak_info[];
extern unsigned char hw_config_params_arr[];

extern volatile UINT32 hw_bssInitFinished;
extern volatile UINT32 hw_memInitFinished;
extern volatile UINT8 hw_tsStallFlag[HW_PROC_CNT];
extern hw_spin_lock_t hw_perfmonLock;

static UINT32 gJTMInterval = 0;
static INT32 gJTMSlope = 0;
static INT32 gJTMOffset = 0;

static volatile BOOL gDramHeapInitFlagChecked = FALSE;
static volatile BOOL gNeedDramHeapInit = FALSE;

static volatile BOOL gSiliconFlagChecked = FALSE;
static volatile BOOL gIsSilicon = FALSE;

static BOOT_DIAG* gBootDiagPtr = (BOOT_DIAG*)CACHED_BOOT_DIAG_BASE_ADDR;
static BOOL32 gBootOutOfStandby = FALSE;

static INIT_OPTION* gInitOptionPtr = (INIT_OPTION*)INIT_OPTION_BASE_ADDR;

BOOL isVpaConfig()
{
    return (hw_pTestConfigs->config[0] == 'v' && hw_pTestConfigs->config[1] == 'p' && hw_pTestConfigs->config[2] == 'a');
}

BOOL isSensorConfig()
{
#if 0
    UINT32 len = hw_strnlen((const char*)hw_pTestConfigs->config, 28);
    if((len > 6) && hw_strcmp(&hw_pTestConfigs->config[len-6], "sensor") == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
#else
    return TRUE;
#endif
}

void print_ddr_freq_info()
{
    UINT32 freq = 0;
    //freq = hw_get_ddr_frequency();
    if(freq != 0)
    {
        hw_critical("DDR frequency: %uMHz \n", freq);
    }
    else
    {
        hw_critical("DDR frequency: N/A \n", freq);
    }
}

void init_testcfg()
{
#if (defined __XTENSA__)
    UINT32 suite_rand_seed = gInitOptionPtr->bf_init_opt.suite_rand_seed;
    UINT32 verbosity = gInitOptionPtr->bf_init_opt.verbosity;
    UINT32 runtime = gInitOptionPtr->bf_init_opt.runtime;
    if(suite_rand_seed != 0)
    {
        hw_pTestConfigs->suite_rand_seed = suite_rand_seed;
    }
    if(verbosity != 0)
    {
        hw_pTestConfigs->verbosity = verbosity;
    }
    if(runtime != 0)
    {
        hw_pTestConfigs->runtime = (UINT64)runtime * 1000000000ull;
    }

    gJTMInterval = gInitOptionPtr->bf_init_opt.jtm_interval;
    gJTMSlope = gInitOptionPtr->bf_init_opt.jtm_slope;
    gJTMOffset = gInitOptionPtr->bf_init_opt.jtm_offset;

    gBootOutOfStandby = gInitOptionPtr->dramc_init_opt.std.NO_DRAMC_RESET;
#endif
}

void bfabric_init()
{
    UINT32 rdval;
    const UINT32 hat_req_cnt = 4;
    const UINT32 het_req_cnt = 6;
    const UINT32 et_req_cnt  = 4;
    const UINT32 imp_req_cnt = 4;
    const UINT32 lsr_req_cnt = 6;

    rdval = hw_reg_read32 (HUP_CHIP_BFABMINI0_FAB_MCQ0_INIT_FAB_MCQ0_INIT_NBPKTMAX0_ADDRESS, 0);
    hw_reg_write32 (HUP_CHIP_BFABMINI0_FAB_MCQ0_INIT_FAB_MCQ0_INIT_NBPKTMAX0_ADDRESS, 0, HUP_FABRICMINI_FAB_MCQ0_INIT_FAB_MCQ0_INIT_NBPKTMAX0_NBPKTMAX0_MODIFY(rdval,hat_req_cnt));
    
    rdval = hw_reg_read32 (HUP_CHIP_BFABMINI1_FAB_MCQ0_INIT_FAB_MCQ0_INIT_NBPKTMAX0_ADDRESS, 0);
    hw_reg_write32 (HUP_CHIP_BFABMINI1_FAB_MCQ0_INIT_FAB_MCQ0_INIT_NBPKTMAX0_ADDRESS, 0, HUP_FABRICMINI_FAB_MCQ0_INIT_FAB_MCQ0_INIT_NBPKTMAX0_NBPKTMAX0_MODIFY(rdval,hat_req_cnt));
    
    rdval = hw_reg_read32 (HUP_CHIP_BFABMINI0_FAB_MCQ1_INIT_FAB_MCQ1_INIT_NBPKTMAX0_ADDRESS, 0);
    hw_reg_write32 (HUP_CHIP_BFABMINI0_FAB_MCQ1_INIT_FAB_MCQ1_INIT_NBPKTMAX0_ADDRESS, 0, HUP_FABRICMINI_FAB_MCQ1_INIT_FAB_MCQ1_INIT_NBPKTMAX0_NBPKTMAX0_MODIFY(rdval,het_req_cnt));
    
    rdval = hw_reg_read32 (HUP_CHIP_BFABMINI1_FAB_MCQ1_INIT_FAB_MCQ1_INIT_NBPKTMAX0_ADDRESS, 0);
    hw_reg_write32 (HUP_CHIP_BFABMINI1_FAB_MCQ1_INIT_FAB_MCQ1_INIT_NBPKTMAX0_ADDRESS, 0, HUP_FABRICMINI_FAB_MCQ1_INIT_FAB_MCQ1_INIT_NBPKTMAX0_NBPKTMAX0_MODIFY(rdval,het_req_cnt));
    
    rdval = hw_reg_read32 (HUP_CHIP_BFABMINI0_FAB_MCQ2_INIT_FAB_MCQ2_INIT_NBPKTMAX0_ADDRESS, 0);
    hw_reg_write32 (HUP_CHIP_BFABMINI0_FAB_MCQ2_INIT_FAB_MCQ2_INIT_NBPKTMAX0_ADDRESS, 0, HUP_FABRICMINI_FAB_MCQ2_INIT_FAB_MCQ2_INIT_NBPKTMAX0_NBPKTMAX0_MODIFY(rdval,et_req_cnt));
    
    rdval = hw_reg_read32 (HUP_CHIP_BFABMINI1_FAB_MCQ2_INIT_FAB_MCQ2_INIT_NBPKTMAX0_ADDRESS, 0);
    hw_reg_write32 (HUP_CHIP_BFABMINI1_FAB_MCQ2_INIT_FAB_MCQ2_INIT_NBPKTMAX0_ADDRESS, 0, HUP_FABRICMINI_FAB_MCQ2_INIT_FAB_MCQ2_INIT_NBPKTMAX0_NBPKTMAX0_MODIFY(rdval,et_req_cnt));
    
    rdval = hw_reg_read32 (HUP_CHIP_BFABMINI0_FAB_MCQ3_INIT_FAB_MCQ3_INIT_NBPKTMAX0_ADDRESS, 0);
    hw_reg_write32 (HUP_CHIP_BFABMINI0_FAB_MCQ3_INIT_FAB_MCQ3_INIT_NBPKTMAX0_ADDRESS, 0, HUP_FABRICMINI_FAB_MCQ3_INIT_FAB_MCQ3_INIT_NBPKTMAX0_NBPKTMAX0_MODIFY(rdval,imp_req_cnt));
    
    rdval = hw_reg_read32 (HUP_CHIP_BFABMINI1_FAB_MCQ3_INIT_FAB_MCQ3_INIT_NBPKTMAX0_ADDRESS, 0);
    hw_reg_write32 (HUP_CHIP_BFABMINI1_FAB_MCQ3_INIT_FAB_MCQ3_INIT_NBPKTMAX0_ADDRESS, 0, HUP_FABRICMINI_FAB_MCQ3_INIT_FAB_MCQ3_INIT_NBPKTMAX0_NBPKTMAX0_MODIFY(rdval,imp_req_cnt));
    
    rdval = hw_reg_read32 (HUP_CHIP_BFABMINI0_FAB_MCQ4_INIT_FAB_MCQ4_INIT_NBPKTMAX0_ADDRESS, 0);
    hw_reg_write32 (HUP_CHIP_BFABMINI0_FAB_MCQ4_INIT_FAB_MCQ4_INIT_NBPKTMAX0_ADDRESS, 0, HUP_FABRICMINI_FAB_MCQ4_INIT_FAB_MCQ4_INIT_NBPKTMAX0_NBPKTMAX0_MODIFY(rdval,lsr_req_cnt));
    
    rdval = hw_reg_read32 (HUP_CHIP_BFABMINI1_FAB_MCQ4_INIT_FAB_MCQ4_INIT_NBPKTMAX0_ADDRESS, 0);
    hw_reg_write32 (HUP_CHIP_BFABMINI1_FAB_MCQ4_INIT_FAB_MCQ4_INIT_NBPKTMAX0_ADDRESS, 0, HUP_FABRICMINI_FAB_MCQ4_INIT_FAB_MCQ4_INIT_NBPKTMAX0_NBPKTMAX0_MODIFY(rdval,lsr_req_cnt));
    
    rdval = hw_reg_read32 (HUP_CHIP_BFABMINI0_FAB_MCQ5_INIT_FAB_MCQ5_INIT_NBPKTMAX0_ADDRESS, 0);
    hw_reg_write32 (HUP_CHIP_BFABMINI0_FAB_MCQ5_INIT_FAB_MCQ5_INIT_NBPKTMAX0_ADDRESS, 0, HUP_FABRICMINI_FAB_MCQ5_INIT_FAB_MCQ5_INIT_NBPKTMAX0_NBPKTMAX0_MODIFY(rdval,lsr_req_cnt));
    
    rdval = hw_reg_read32 (HUP_CHIP_BFABMINI1_FAB_MCQ5_INIT_FAB_MCQ5_INIT_NBPKTMAX0_ADDRESS, 0);
    hw_reg_write32 (HUP_CHIP_BFABMINI1_FAB_MCQ5_INIT_FAB_MCQ5_INIT_NBPKTMAX0_ADDRESS, 0, HUP_FABRICMINI_FAB_MCQ5_INIT_FAB_MCQ5_INIT_NBPKTMAX0_NBPKTMAX0_MODIFY(rdval,lsr_req_cnt));
}

// TODO: PCIE and POR
//void enable_debug_trace_clock()
//{
//#if (defined __XTENSA__)
//    hw_reg_write32(0, POR_ClockEnable2, 0x1000);
//#endif    
//}
//
//void disable_pcie_transaction()
//{
//#if (defined __XTENSA__)    
//    hw_reg_write32(0, SSX1_IA_SSX0_I0_IP_RT1_TA_PCIEC_T0_PM_REQ_INFO_PERMISSION_0_BYTE_ADDRESS, 0);
//#endif
//}

void cleanup_intc_registers()
{
#if (defined __XTENSA__)
    hw_reg_write32(INTC_BASE_ADDR, INTC_P0_DATA0, 0);
    hw_reg_write32(INTC_BASE_ADDR, INTC_SCRATCH_0_DATA, 0);
    hw_reg_write32(INTC_BASE_ADDR, INTC_SCRATCH_1_CLEAR, 0x7FFFFFFF);
    hw_reg_write32(INTC_BASE_ADDR, INTC_SCRATCH_2_DATA, 0);
    hw_reg_write32(INTC_BASE_ADDR, INTC_SCRATCH_3_DATA, 0);
    hw_reg_write32(INTC_BASE_ADDR, INTC_SCRATCH_4_DATA, 0);
    hw_reg_write32(INTC_BASE_ADDR, INTC_SCRATCH_5_DATA, 0);
    hw_reg_write32(INTC_BASE_ADDR, INTC_SCRATCH_6_DATA, 0);
    hw_reg_write32(INTC_BASE_ADDR, INTC_SCRATCH_7_DATA, 0);
#endif
}

void disable_nodefabric_timeout()
{
#if (defined __XTENSA__)
    // since the timeouts can be triggered by memmax, turn off the timeouts in the chip TAs, let's not be clever and just set all to zero
    //hw_reg_write32(0, IC_SSX_IA_CHIP_IP_RT0_TA_CHIP_TA_AGENT_CONTROL_BYTE_ADDRESS, 0x0);

    //hw_reg_write32(0, IC_SSX_IA_CHIP_IP_RT0_RT0_RT_NETWORK_CONTROL_BYTE_ADDRESS, 0);
#endif
}

void config_error_logging()
{
#if (defined __XTENSA__)
    // These need to be disabled, otherwise we can't read the status registers after a timeout
    // TODO: regs
    //hw_reg_write32(0, SSX0_IA_SSX1_I0_IP_RT0_TA_SSX1_T0_TA_AGENT_CONTROL_BYTE_ADDRESS, 0);
    //hw_reg_write32(0, SSX1_IA_SSX0_I0_IP_RT1_TA_S32200_T0_TA_AGENT_CONTROL_BYTE_ADDRESS, 0);
    //hw_reg_write32(0, SSX1_IA_SSX0_I0_IP_RT1_TA_SSX0_T0_TA_AGENT_CONTROL_BYTE_ADDRESS, 0);
    
    // We have discovered plausible sequences that will hit the timeout, so disable the timeouts
    //hw_reg_write32(0, SSX0_IA_SSX1_I0_IP_RT0_TA_DRAMC_T0_TA_AGENT_CONTROL_BYTE_ADDRESS, 0);
    // would probably be better to do RMW, but there's nothing else set in these TAs of interest...
#endif
}

void sys_init_basram()
{
    UINTPTR destStart = hw_ddr_sram_bak_info[0];
    UINTPTR destEnd = hw_ddr_sram_bak_info[1];
    
    UINTPTR srcStart = destStart - HW_ADDR_SRAM0_CACHED_BASE + (UINTPTR)&_ddr_sram_bak_start;
    UINTPTR srcEnd = destEnd - HW_ADDR_SRAM0_CACHED_BASE + (UINTPTR)&_ddr_sram_bak_start;
    
    UINT32 size = srcEnd - srcStart;
    
    hw_memcpyAligned32((void*) destStart,(void*) srcStart, size);
    hw_cacheFlushBuffer((void*) destStart, size);
}

BOOL sys_need_dram_heap_init()
{
#if (defined __XTENSA__)
    if(!gDramHeapInitFlagChecked)
    {
        gNeedDramHeapInit = (hw_pTestConfigs->bfinit.DRAM_HEAP_INIT && (!gInitOptionPtr->bf_init_opt.skip_dram_heap_init)) ? TRUE : FALSE;
        gDramHeapInitFlagChecked = TRUE;
    }
    return gNeedDramHeapInit;
#else
    return FALSE;
#endif  
}

void sys_init_memory()
{
    UINT32 dwpid = hw_getCoreNum();
    
    if(dwpid == 0)
    {
        if(sys_need_dram_heap_init())
        {
            bss_clear_ASM((UINT32)dramHeapInfo[0]);
        }
        
        // In non-simulation platforms, additional initialization
        // required for BASRAM to clear effects of 2BL.
        // In BASRAM_ONLY mode, we don't run the real 2BL
        // anyway so this is unnecessary.
        if(hw_pTestConfigs->bfcomp != HW_BASRAM_ONLY &&
           (hw_pTestConfigs->platform == RPP_PLATFORM ||
            hw_pTestConfigs->platform == EMU_PLATFORM ||
            hw_pTestConfigs->platform == CHIP_PLATFORM))
        {
            sys_init_basram();
        }
    }
}

// TODO: PCIE
//void sys_init_pcie()
//{
//#if (defined __XTENSA__)
//    UINT32 ret = 0;
//    UINT32 dwpid = hw_getCoreNum();
//    if((dwpid == 0) && (hw_pTestConfigs->platform != RPP_PLATFORM) && (hw_pTestConfigs->platform != CHIP_PLATFORM) && hw_pTestConfigs->bfinit.REAL_FABRIC && hw_pTestConfigs->bfinit.PCIE_INIT)
//    {
//        ret = hw_pcie_init();
//        if(ret == 0)
//        {
//            hw_status("PCIe init success \n");
//        }
//        else
//        {
//            hw_errmsg("PCIe init error: %d \n", ret);
//        }
//    }
//#endif
//}

void sys_blockTest()
{
#if (defined __XTENSA__)
    if(hw_pTestConfigs->bfinit.REAL_INTC)
    {
        if(hw_reg_read32(INTC_BASE_ADDR, INTC_SCRATCH_1_DATA) & BLOCK_TEST_MASK)
        {
            hw_critical("Block tests\n");
            while(hw_reg_read32(INTC_BASE_ADDR, INTC_SCRATCH_1_DATA) & BLOCK_TEST_MASK);
        }
        hw_critical("Unblock tests\n");
        hw_reg_write32(INTC_BASE_ADDR, INTC_SCRATCH_1_CLEAR, BLOCK_TEST_MASK);
    }
#endif
}

BOOL sys_notify_host_for_standby()
{
    BOOL ret = FALSE;
#if (defined __XTENSA__)    
    UINT32 dwpid = hw_getCoreNum();
    UINT32 testID = HW_INTERNAL_TESTID;
    UINT32 responseFromHost = 0;
    
    if(hw_is_silicon())
    {
        hw_write32(&hw_extcomSpace.core[dwpid].cmd_buf.resp, 0);
        HW_EXTCOM_SIGNAL command = {{HW_HOSTCMD_NOTIFY_STANDBY, testID, dwpid, 0x1, 0x1}};
        
        hw_write32(&hw_extcomSpace.core[dwpid].cmd_buf.cmd.AsUINT32, command.AsUINT32);
        hw_raise_msi(0);        

        while(responseFromHost == 0)
        {
            responseFromHost = hw_read32(&hw_extcomSpace.core[dwpid].cmd_buf.resp);
        }
        if((responseFromHost & 0xFF) != HOST_ERROR_NONE)
        {
            hw_errmsg("%s: host indicated error (%d) for standby request\n", __func__, responseFromHost & 0xFF);
        }
        else
        {
            ret = TRUE;
        }       
        hw_write32(&hw_extcomSpace.core[dwpid].cmd_buf.resp, 0);
    }
    else if((hw_pTestConfigs->platform == SIM_PLATFORM) || (hw_pTestConfigs->platform == SIM_PLATFORM))
    {
        ret = TRUE;
    }
        
#endif  
    return ret;
}


// TODO: standby
//BOOL sys_standby()
//{
//    BOOL ret = FALSE;
//#if (defined __XTENSA__)
//    hw_bssInitFinished = 0;
//    hw_memInitFinished = 0;
//    hw_perfmonLock.owner = HW_TESTID(-1);
//    for(UINT32 i = 0; i < HW_PROC_CNT; i++)
//    {
//        hw_tsStallFlag[i] = 0;
//    }
//    hw_extcomSpace.ext_buf = NULL;
//    hw_extcomSpace.post = 0;
//    hw_extcomSpace.eot_flag = 0;
//
//    ret = sys_notify_host_for_standby();
//    if(ret)
//    {
//        hw_reg_write32(0, DRAMC_UPCTL_REG_SCTL, 0x3);
//        while(1)
//        {
//            asm volatile("waiti 15");
//        }       
//    }
//    else
//    {
//        hw_critical("System failed to go to standby \n");
//    }
//#endif  
//    return ret;
//}

void system_init()
{
    UINT32 dwpid = hw_getCoreNum();
    
    // Config node
    if(hw_pTestConfigs->bfinit.REAL_FABRIC)
    {
        if((dwpid % 2) == 0)
        {
            disable_nodefabric_timeout();
        }
        if(dwpid == 0)
        {
            config_error_logging();
            //enable_debug_trace_clock();
#if defined(TEST_FAIL_ON_HOSTMEM_ALLOC)
            //disable_pcie_transaction();
#endif

            // Program default XB priority modes for DNN/CFPU/CLBASRAM
            hw_reg_write32 (HUP_CHIP_CLSRAM_BASRAM_XB_XB_REG_MODE_ADDRESS, 0, HUP_XBAR_MODE_PRI_MODE_SET(1) | HUP_XBAR_MODE_FORCEPRI_MODE_SET(5));
            
            if(hw_pTestConfigs->platform == CHIP_PLATFORM)
            {
                init_testcfg();
            }

            // Program default pending transaction counts for mini-fab 
            bfabric_init();
        }
    }
    
    if(hw_pTestConfigs->bfinit.REAL_INTC)
    {
        if(dwpid == 0)
        {
            cleanup_intc_registers();
        }
    }
    
    if(dwpid == 0)
    {
        // Wakeup active nodes
        UINT32 core = 0;
        UINT32 node = 0;
        UINT32 mask = 0;
        for(core = 2; core < HW_PROC_CNT; core ++)
        {
            if(hw_activeCoreList[core])
            {
                node = core / 2;
                mask |= 1 << node;
            }
        }
        
        plat_post(BOOT_STATUS_DONE_SYSTEM_INIT, HW_INTERNAL_TESTID);
        lib_por::ClassPorIP porAPI;
        
	// ARAY : comment out PMU usage for now
	//        if(hw_pTestConfigs->bfinit.REAL_POR)
	//  	{
	//            if ( hw_read32(&hw_pTestConfigs->bfinit.POWER_MANAGER_CYC) || hw_pTestConfigs->bfinit.STANDALONE  ) {
	//                plat_post(PMU_ENABLE_INIT_ENTER, HW_INTERNAL_TESTID);
	//                porAPI.sys_init_pmu();
	//            }
	//        }
        porAPI.hw_wakeup_node_options(mask, FALSE, TRUE);

    }
    
    sys_init_pcie();
    
#ifdef POWER_CPLB_IN_BF
    sys_power_cplb();
#endif
    
    // ReadDDR frequency, but only if
    // the real DDR is there
    if(dwpid == hw_read32(&hw_pTestConfigs->tpid))
    {
        if(hw_read32(&hw_pTestConfigs->bfinit.REAL_MC))
        {
            print_ddr_freq_info();
            //sys_print_dramc_init_status();
        }
    }
    else
    {
        sys_tsStall();
    }
}

void sys_print_dramc_init_status()
{
    UINT32 i = 0;
    if(hw_is_silicon() && (*(UINT32*)&gInitOptionPtr->dramc_init_opt.shmoo.enable != 0))
    {
        hw_critical("DDR training configuration and result:\n");
        hw_critical("  PIR=0x%08x, DTCR=0x%08x, MR3=0x%08x, ZQ0PR=0x%08x, ZQ1PR=0x%08x, ACLCDLR=0x%08x\n",
            gBootDiagPtr->dramc.training_stat.pir,
            gBootDiagPtr->dramc.training_stat.dtcr,
            gBootDiagPtr->dramc.training_stat.mr3,
            gBootDiagPtr->dramc.training_stat.zq0pr,
            gBootDiagPtr->dramc.training_stat.zq1pr,
            gBootDiagPtr->dramc.training_stat.aclcdlr);
        hw_critical("  DTDR0=0x%08x, DTDR1=0x%08x, DTEDR0=0x%08x, DTEDR1=0x%08x\n",
            gBootDiagPtr->dramc.training_stat.dtdr[0],
            gBootDiagPtr->dramc.training_stat.dtdr[1],
            gBootDiagPtr->dramc.training_stat.dtedr[0],
            gBootDiagPtr->dramc.training_stat.dtedr[1]);
        hw_critical("  DXnLCDLR0=0x%08x,0x%08x,0x%08x,0x%08x\n",
            gBootDiagPtr->dramc.training_stat.dx0lcdlr[0],
            gBootDiagPtr->dramc.training_stat.dx1lcdlr[0],
            gBootDiagPtr->dramc.training_stat.dx2lcdlr[0],
            gBootDiagPtr->dramc.training_stat.dx3lcdlr[0]);
        hw_critical("  DXnLCDLR1=0x%08x,0x%08x,0x%08x,0x%08x\n",
            gBootDiagPtr->dramc.training_stat.dx0lcdlr[1],
            gBootDiagPtr->dramc.training_stat.dx1lcdlr[1],
            gBootDiagPtr->dramc.training_stat.dx2lcdlr[1],
            gBootDiagPtr->dramc.training_stat.dx3lcdlr[1]);
        for(i = 0; i < 6; i++)
        {
            hw_critical("  DXnBDLR%d=0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x\n", i,
                gBootDiagPtr->dramc.training_stat.dx0bdlr[i],
                gBootDiagPtr->dramc.training_stat.dx1bdlr[i],
                gBootDiagPtr->dramc.training_stat.dx2bdlr[i],
                gBootDiagPtr->dramc.training_stat.dx3bdlr[i],
                gBootDiagPtr->dramc.training_stat.dx4bdlr[i],
                gBootDiagPtr->dramc.training_stat.dx5bdlr[i],
                gBootDiagPtr->dramc.training_stat.dx6bdlr[i],
                gBootDiagPtr->dramc.training_stat.dx7bdlr[i],
                gBootDiagPtr->dramc.training_stat.dx8bdlr[i]);
        }           
        if(gInitOptionPtr->dramc_init_opt.shmoo.enable.ac ||
            gInitOptionPtr->dramc_init_opt.shmoo.enable.wdqd || 
            gInitOptionPtr->dramc_init_opt.shmoo.enable.rdqsd || 
            gInitOptionPtr->dramc_init_opt.shmoo.enable.rdqsnd)
        {
            hw_critical("DDR configuration override after training:\n");
            if(gInitOptionPtr->dramc_init_opt.shmoo.enable.ac)
            {
                hw_critical("  ACLCDLR=0x%08x\n", gBootDiagPtr->dramc.shmoo_override.aclcdlr);
            }
            else
            {
                hw_critical("  DXnLCDLR1=0x%08x,0x%08x,0x%08x,0x%08x\n\n",
                    gBootDiagPtr->dramc.shmoo_override.dx0lcdlr1,
                    gBootDiagPtr->dramc.shmoo_override.dx1lcdlr1,
                    gBootDiagPtr->dramc.shmoo_override.dx2lcdlr1,
                    gBootDiagPtr->dramc.shmoo_override.dx3lcdlr1);
            }
        }
    }
}

// TODO
//void sys_power_cplb()
//{
//    if((hw_getCoreNum() == 0) && hw_is_silicon() && isSensorConfig())
//    {
//        hw_i2c_fpga_power_cplb();
//    }
//}

void hw_wakeup_node(UINT32 mask)
{
     lib_por::ClassPorIP porAPI;
     porAPI.hw_wakeup_node_options(mask, FALSE, TRUE);  
}

void hw_shutdown_node(UINT32 mask)
{
     lib_por::ClassPorIP porAPI;
     porAPI.hw_shutdown_node_options(mask, FALSE);  
 
}

void hw_markActiveCore(HW_CORE core, BOOL active)
{
    hw_activeCoreList[core] = active;
}

// TODO: fabric
//void hw_configureFabricTimeout(BOOL enable)
//{
//#if (defined __XTENSA__)
//    UINT32 val = enable ? 0x300 : 0;
//    hw_reg_write32(0, IC_SSX_IA_CHIP_IP_RT0_RT0_RT_NETWORK_CONTROL_BYTE_ADDRESS, val);
//#endif
//}

BOOL hw_raise_msi(UINT8 vector)
{
    if((vector == 0) && (hw_getCoreNum() != bifrostCachedGlobals.tpid))
    {
        hw_errmsg("Attempting generating vector-0 MSI from test. Only test driver allowed");
        return FALSE;
    }
    const UINT32 PCIE_CONFIG_BASE = 0x04400000;
    const UINT32 XTENSA_PCIE_BASE = 0x3E000000;
    const UINT32 XTENSA_PCIE_SIZE = 0x02000000;
    const UINT32 XTENSA_PCIE_END = 0x40000000;
    UINT32 msiSocLow;
    UINT32 msiAddress;
    UINT32 msiData0;
    msiSocLow = hw_reg_read32(0, PCIE_CONFIG_BASE + 0x54);
    msiAddress = XTENSA_PCIE_END - 4096 + (msiSocLow & 0xFFCUL);
    return TRUE;
}

UINT32 hw_get_jtm_interval()
{
    return gJTMInterval;
}

INT32 hw_get_jtm_slope()
{
    return gJTMSlope;
}

INT32 hw_get_jtm_offset()
{
    return gJTMOffset;
}

// TODO: DDR
//UINT32 hw_get_ddr_frequency()
//{
//    UINT32 ret = 0;
//#if (defined __XTENSA__)
//    if(hw_pTestConfigs->bfinit.REAL_MC)
//    {
//        UINT32 regVal = 0;
//        regVal = hw_reg_read32(0, POR_ClockEnable5);
//        if((regVal & 0x02000000) == 0)
//        {
//            hw_critical("DDR not enabled \n");
//        }
//        else
//        {
//            regVal = hw_reg_read32(0, POR_DRAMDIV6PARAMSEL);
//            switch(regVal)
//            {
//                case 0:
//                    ret = 800;
//                    break;
//                case 1:
//                    ret = 667;
//                    break;
//                case 2:
//                    ret = 400;
//                    break;
//                default:
//                    hw_errmsg("DDR frequency invalid. Config SEL (%d)\n", regVal);
//                    break;
//            }
//        }
//    }
//#endif
//    return ret;
//}

HW_BOOT_DIAG_DRAMC_INIT_TIME* hw_get_dramc_init_time()
{
    return (HW_BOOT_DIAG_DRAMC_INIT_TIME*)&gBootDiagPtr->dramc.init_time;
}

BOOL32 hw_boot_outof_standby()
{
    return gBootOutOfStandby;
}

BOOL hw_is_silicon()
{
#if (defined __XTENSA__)
    if(!gSiliconFlagChecked)
    {
        if((hw_pTestConfigs->platform == CHIP_PLATFORM) && ((hw_reg_read32(0, 0x04080800) & 0xFF000000) == 0))
        {
            gIsSilicon = TRUE;
        }
        else
        {
            gIsSilicon = FALSE;
        }
        gSiliconFlagChecked = TRUE;
    }
    return gIsSilicon;
#else
    return FALSE;
#endif
}

