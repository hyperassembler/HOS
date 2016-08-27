/*-----------------------------------------------------
 |
 |      Simulation_init_seq.c
 |
 |     Simulation init sequnece for DRAM
 |     during real boot
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2013  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

/*----------------------- Header Files Inclusions ---------------------------*/

#include "init_helper.h"
//#include "hup_chip.h"
#include "kahalu_sim_init.h"

/*-------------------------- Macro Definitions for DRAMC debug --------------*/

#undef  WL_ENB
#ifdef  WL_ENB
  #define PIR_WR            0x0001FB34  // DQSGATE disabled, retrigger PLL/DCAL
  #define PSGR_RD           0x80001FBF
#else
  #define PIR_WR            0x0001F134  // WL, WLADJ (Write Level Adj-WL2) & DQSGATE disabled
  #define PSGR_RD           0x80001F1F  
#endif

#define PUREN               0
#define CUAEN               1
#define CKBDLR              0
#define ACBDLR              0x04040404
#define DXBDLR              0
#define ZQDIV               0x1B                  // ZQ0PR[7:0]             
#define MR3                 0x02
#define AC_SR               0     /* 0 = fastest, 1 = fast, 2 = medium, 3 = slow */
#define DX_SR               0     /* 0 = fastest, 1 = fast, 2 = medium, 3 = slow */

#define  PD_IDLE

//#define REG_DUMP
#define REGS_DUMP_ADDR      0x20106200

/*-------------------------- Macro Definitions ------------------------------*/

// Training setting for simulation. Partial training is enabled for real chip operation
//#define LPDDR_FULL_TRAINING
//#define LPDDR_PARTIAL_TRAINING

#define TIMEOUT_VALUE       100000
#define PRINT_STR(__X__)    /* TBD: print specific to platform */
#define PRINT_ERR(__X__)    /* TBD: print specific to platform for Error */
#define DELAY_PCLK(__X__)   delay_cycles(__X__)
#define DELAY_NCLK(__X__)   delay_cycles(__X__)

#define DRAMC_SLEEPMODE     0
#define DRAMC_WAKEUPMODE    1
#define INVALID_REQUEST     0xFFFFFFFF
#define TB_POLL_TIMEOUT     20000
#define PCIE_BAR0_MAP                           0x04000000 /* MMIO Registers */
#define PCIE_BAR1_MAP                           0x04400000 /* port logic reg */
#define PCIE_BAR2_MAP                           0x20100000 /* BaSRAM */
#define PCIE_BAR3_MAP                           0x41000000 /* LPDDR memory */

#define SCTLCMD_TO_STATE(__CMD__)  \
    (__CMD__ == UPCTL_CMD_INIT ? UPCTL_STATE_INITMEM :      \
    (__CMD__ == UPCTL_CMD_CFG ? UPCTL_STATE_CONFIG :        \
    (__CMD__ == UPCTL_CMD_GO ? UPCTL_STATE_ACCESS :         \
    (__CMD__ == UPCTL_CMD_SLEEP ? UPCTL_STATE_LOWPOWER :    \
    (__CMD__ == UPCTL_CMD_WAKEUP ? UPCTL_STATE_ACCESS :     \
        INVALID_REQUEST)))))

// State command to state conversion (origin). Returns the origin state after issuing
// a state transition with a given command.
#define SCTLCMD_TO_STATE_ORIGIN(__CMD__)                    \
    (__CMD__ == UPCTL_CMD_INIT ? UPCTL_STATE_CONFIG :       \
    (__CMD__ == UPCTL_CMD_CFG ? UPCTL_STATE_INITMEM :       \
    (__CMD__ == UPCTL_CMD_GO ? UPCTL_STATE_CONFIG :         \
    (__CMD__ == UPCTL_CMD_SLEEP ? UPCTL_STATE_ACCESS :      \
    (__CMD__ == UPCTL_CMD_WAKEUP ? UPCTL_STATE_LOWPOWER :   \
        INVALID_REQUEST)))))

/*-------------------------- Enum Definitions -------------------------------*/
typedef enum UPCTL_STATECHANGE_CMD
{
    UPCTL_CMD_INIT,     /* b000 = INIT (move to Init_mem from Config) */
    UPCTL_CMD_CFG,      /* b001 = CFG (move to Config from Init_mem or Access) */
    UPCTL_CMD_GO,       /* b010 = GO (move to Access from Config) */
    UPCTL_CMD_SLEEP,    /* b011 = SLEEP (move to Low_power from Access) */
    UPCTL_CMD_WAKEUP,   /* b100 = WAKEUP (move to Access) */

} UPCTL_STATECHANGE_CMD_e;

typedef enum UPCTL_STATES
{
    UPCTL_STATE_INITMEM,            /* b000 = Init_mem */
    UPCTL_STATE_CONFIG,             /* b001 = Config */
    UPCTL_STATE_CONFIG_REQ,         /* b010 = Config_req */
    UPCTL_STATE_ACCESS,             /* b011 = Access */
    UPCTL_STATE_ACCESSREQ,          /* b100 = Access_req */
    UPCTL_STATE_LOWPOWER,           /* b101 = Low_power */
    UPCTL_STATE_LOWPOWERENTRYREQ,   /* b110 = Low_power_entry_req */
    UPCTL_STATE_LOWPOWEREXITREQ,    /* b111 = Low_power_exit_req */

} UPCTL_STATES_e;

typedef enum reprog_req
{
    REPROGRAM_NOT_REQ = 0,
    REPROGRAM_REQ
} reprog_req_e;


/*------------------ Data Structures & Custom Typedefs ----------------------*/

u32 rev_upctl = 0;
u32 rev_phy = 0;
u32 vendor = 0; // samsung 0, elpida 1

u8 pd_idle_cycles = 0;
u8 sr_idle_cycles = 0;
u8 cs_idle_cycles = 0;
u8 precharge_power_down_type = 0;


//u32 phy_reset_lprm          = 0x0;
//u32 remain_low_power_state  = 0x0;
u32 stat_lp_trig_sr_idle    = 0x0;
int disable_ref_cmd = 0;
int rd_lat = 0;
int wr_lat = 0;

u32 t_ck = 0;

//DTPR0 Parameters
u32 t_rtp   = 0;
u32 t_wtr   = 0;
u32 t_rp    = 0;
u32 t_prea_extra = 0;
u32 t_rcd   = 0;
u32 t_ras   = 0;
u32 t_rrd   = 0;
u32 t_rc    = 0;

//DTPR1 Parameters

u32 t_mod = 0;
u32 t_faw = 0;
u32 t_rfc = 0;

//DTPR2 Parameters
u32 t_xs    = 0;
u32 t_xp    = 0;
u32 t_cke   = 0;

//DTPR3 Parameters
u32 t_dqsck     = 0;
u32 t_dqsckmax  = 0;
u32 dqsgx       = 0;
u32 tfaw_cfg    = 0;

u8 lpddr3_mr1 = 0;
u8 lpddr3_mr2 = 0;
u8 lpddr3_mr3 = 0;

u32 t_wr    = 0;
u32 t_zqcs  = 0;
u32 t_zqcl  = 0;
u32 t_zqcsi = 0;
u32 t_ckesr = 0;
u32 t_dpd   = 0;
u32 zqndr0[4]  ={0,0,0,0};

u32 ac_pdd_en = 0;
u32 bbflags_timing = 0x7;  /* UPCTL_TCU_SED_P(16) - t_rp */
u32 ac_pdd_add_del = 0x4;

u8 twl            = 0;
u8 trl            = 0;
u32 tphy_wrlat    = 0;
u32 trddata_en    = 0;
u32 pubm2_sub_hdr = 0;

u32 rTOGCNT1U   = 0;
u32 rTINIT      = 0;
u32 rTOGCNT100N = 0;
u32 rTREFI      = 0;
u32 rTMRD       = 0;
u32 rTRFC       = 0;
u32 rTRP        = 0;
u32 rTAL        = 0;
u32 rTCL        = 0;
u32 rTCWL       = 0;
u32 rTRAS       = 0;
u32 rTRC        = 0;
u32 rTRCD       = 0;
u32 rTRRD       = 0;
u32 rTRTP       = 0;
u32 rTWR        = 0;
u32 rTWTR       = 0;
u32 rTEXSR      = 0;
u32 rTXP        = 0;
u32 rTDQS       = 0;
u32 rTRTW       = 0;
u32 rTCKSRE     = 0;
u32 rTCKSRX     = 0;
u32 rTMOD       = 0;
u32 rTCKE       = 0;
u32 rTRSTH      = 0;
u32 rTRSTL      = 0;
u32 rTZQCS      = 0;
u32 rTZQCL      = 0;
u32 rTXPDLL     = 0;
u32 rTZQCSI     = 0;
u32 rTCKESR     = 0;
u32 rTDPD       = 0;

/*------------------------------ Externs ------------------------------------*/
extern INIT_OPTION* gInitOptionPtr;
extern BOOT_DIAG* gBootDiagPtr;

/*---------------------------- Prototypes -----------------------------------*/

void lpddr_calc_timing_regs_t(int, int);

/*----------------------- Function Definitions ------------------------------*/
u32 div_roundup_f(int t_ck, int tval, int max_bit)
{
    int max = (1<<max_bit) - 1;
    int ret_val = tval / t_ck;
    if(tval % t_ck)
    {
        ret_val++;
    }

    if(max && (ret_val > max))
    {
        ret_val = max;
    }
    return ret_val;
}

/**
  * \brief
  *  This function will initialize the power on reset sequence
  *  with appropriate register configuration for DRAMC(Memmax, NIF and uPCTL)
  *
  * \param
  *        None
  * \return
  *        None
  */
void por_dramc_seq(u32 speed)
{
    u32 reg_val;

    if(speed == 400)
    {
        //Fin = 24MHz Fout =400 MHz, FBdiv =100, Indiv = 1, OUTdiv =6 VCO = 2400
        //FIX_ME  u32 reg_val  = 0x20018c5;
        //FIX_ME *(r32*) HUP_CHIP_POR_PLLCONFIG_PLL2_0_ADDRESS =  reg_val; 
        //FIX_ME *(r32*) HUP_CHIP_POR_PLLCONFIG_PLL2_0_ADDRESS =  0x0; 
    }
    else // TODO: configure other speeds. 266 is set in testbench for now
    {
        
    }
    
    /* enable p_clk,n_clk */
    *(r32*)(HUP_CHIP_POR_SOFTRESETRELEASE3_ADDRESS) =  ((0x3 << 19) | (0x1 << 2)) ;
    *(r32*)(HUP_CHIP_POR_CLOCKENABLE3_ADDRESS) =  (0x3 << 1) ;

    /* Need to bring up memclk domain of afabric */
    //New registers would have to be added here. FIX_ME
    //*(r32*)(HUP_CHIP_POR_DRAMC_MISC_ADDRESS) = *(r32*)(HUP_CHIP_POR_DRAMC_MISC_ADDRESS) & ~(0x1 << 8) ; // Deassert memclk_idlereq bit[8]

    /* Need to check memclk_idleack is 0 */
    /*
    do{
       reg_val = *(r32*)(HUP_CHIP_POR_DRAMC_MISC_ADDRESS);
    }while((reg_val & (0x1 << 9))!= 0x0);
    */
    delay_ns(100); 
}

void por_pub_seq(void)
{
    *(r32*)(HUP_CHIP_POR_SOFTRESET0_ADDRESS) = 0x03200000;   //Reg Adrs: 0x04080080
    delay_us(100);
    /*
     * Software Reset release( dramc_T2_presetn, ctl_rst_n and ctl_sdr_rst_n)
     */
    *(r32*)(HUP_CHIP_POR_SOFTRESETRELEASE0_ADDRESS) = 0x03200000;   //Reg Adrs: 0x04080080
    return;
}


/**
  * \brief
  *    Used to issue a state command to SCTL register and change the
  *    operational state of PCTL. This task will block either until
  *    until the requested command has been executed and the state change has
  *    occurred or when TimeOut occurs (Error) based on TB_POLL_TIMEOUT value.
  *
  * @param[in] uPDDR status to move
  * \param[out]
  *        None
  * \return
  *        None
  */
void lpddr_state_cmd_t(u32 state_cmd)
{
#if 0
    u32 poll_count = 0;
    u32 state_origin = INVALID_REQUEST;
    u32 state = INVALID_REQUEST;
    u32 reg_val = 0;

    // initial to 0
    stat_lp_trig_sr_idle = 0;
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_OSCS_SCTL_ADDRESS) = state_cmd;    //W uPCTL 0x004

    // Get the value of the final state to be used for polling the STAT register
    state = SCTLCMD_TO_STATE(state_cmd);
    if(INVALID_REQUEST == state)
    {
        PRINT_ERR("Invalid State Request!\n");
    }

    // Get the value of the origin state from which transtion started
    state_origin = SCTLCMD_TO_STATE_ORIGIN(state_cmd);
    if(INVALID_REQUEST == state)
    {
        PRINT_ERR("Invalid State Request!\n");
    }

    /* Poll the stat register . */
    reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_OSCS_STAT_ADDRESS);      //R uPCTL 0x008

    // Transition into Low Power or
    // desired state entered
    while( ((reg_val & 0x7) != state) &&
          !(((reg_val & 0x7) == UPCTL_STATE_LOWPOWER) && (state_origin != UPCTL_STATE_LOWPOWER)) &&
           (poll_count < TB_POLL_TIMEOUT))
    {
        reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_OSCS_STAT_ADDRESS);   //R uPCTL 0x008
        poll_count = poll_count + 1;
    }

    /*
     * if transition in Low_power and not desired state
     * check if Low_power transtion done via Hardware sr_idle>0
     * If yes set stat_lp_trig_sr_idle=1 so that in_config_t does not
     * do MCFG reprograming, MCMD sending etc
     * Otherwise report error
     */
    if(((reg_val & 0x7) == UPCTL_STATE_LOWPOWER) &&
       (state != UPCTL_STATE_LOWPOWER))
    {
        if((((reg_val & 0x70) >> 0x4) == 0x1) ||
           (((reg_val & 0x70) >> 0x4) == 0x2))
        {
            stat_lp_trig_sr_idle = 1;
        }
        else
        {
            PRINT_STR("May not change state!!\n");
        }
    }

    if(poll_count == TB_POLL_TIMEOUT)
    {
        PRINT_ERR("State change request is timed out\n");
    }
#endif
    return;
}


/**
  * \brief
  *    Run time frequency change.
  *
  * @param[in] speed
  * \param[out]
  *        None
  * \return
  *        None
  */
void lpddr_set_freq_t(int keep_latency,int clk_speed)
{
    if(200 == clk_speed)  // 200MHz
    {
        t_ck = 2500;
    }
    else if (266 == clk_speed)           // 266MHz
    {
        t_ck = 1875;
    }
    else if (333 == clk_speed)           // 333MHz
    {
        t_ck = 1500;
    }
    else                                 // 400MHz
    {
        t_ck = 1250;
    }
    lpddr_calc_timing_regs_t(keep_latency, clk_speed);
}

 /**
   * \brief
   *    DFI Polling.
   *
   * @param[in] 
   *        None
   * \param[out]
   *        None
   * \return
   *        None
   */
   
void lpddr_dfi_initcomplete_poll_t()
{
#if 0
    u32 reg_val = 0;
    /*
     * 5. Monitor DFI initialization status polling DFISTSTAT0.dfi_init_complete
     */

    //DFISTSTAT0 Polling
    do{
        // Reading from register : PCTL : DFISTSTAT0 ::: and address : 'h02c0
        reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_DFIS_DFISTSTAT0_ADDRESS);
        DELAY_PCLK(100);
    }while((reg_val & 0x00000001)!= 0x1);
#endif    
}

/**
  * \brief
  *    Power Up Sequence.
  *
  * @param[in] 
  *        None
  * \param[out]
  *        None
  * \return
  *        None
  */

void lpddr_start_powerup_t()
{
#if 0
    u32 reg_val = 0;
    PLATFORM_TYPE plat = get_platform_type();
    u32 silicon_mode = (plat == PLAT_SIM && gInitOptionPtr->dramc_init_opt.std.FULL_DRAMC_INIT == 0) ? 0 : 1;

   /*
    * 6. Start power-up sequence by writing POWCTL.power_up_start = 1. Monitor powerup status 
    *    by polling POWSTAT.power_up_done = 1.
    */
    // Writing to register : uPCTL : POWCTL ::: and address : 'h044  : Data : 'h00000001
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_POWCTL_ADDRESS) = 0x00000001;
    //POWSTAT Polling
    do{
        // Reading from register : uPCTL : POWSTAT ::: and address : 'h048
        reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_POWSTAT_ADDRESS);
        if(silicon_mode)
        {
            DELAY_NCLK(2000);  // have to wait for 200 usec
        }
    }while((reg_val & 0x00000001)!= 0x1);
#endif
}

/**
  * \brief
  *    DFICTRLUPD command to MCMD.
  *
  * @param[in] 
  *        None
  * \param[out]
  *        None
  * \return
  *        None
  */

void lpddr_dfictrlupd_cmd_t()
{
#if 0
    u32 reg_val = 0;
    /*
    10. Write DFICTRLUPD command to MCMD and poll MCMD.start_cmd = 0.
    */ 
    /* DFICTRLUPD - dif_ctrlupd_req after initialization complete */
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS) = 0x80F0000A;
    do{
        reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS);
    }while((reg_val & 0x80000000) != 0);
#endif
}

 /**
   * \brief
   *    Used to move the operational state of the DWC_ddr2_pctl core to any
   *    state by calling the appropriate sequence of lpddr_state_cmd_t tasks
   *    depending on the current operational state.
   *    This task blocks until either the requested state has been entered by
   *    the DWC_ddr2_pctl core or when TimeOut (Error) occurs based on TB_POLL_TIMEOUT value.
   *
   * @param[in] uPDDR command to move status
   * \param[out]
   *        None
   * \return
   *        None
   */
void lpddr_move_state_t(u32 req_state)
{
#if 0
    u32 current_state = UPCTL_STATE_CONFIG_REQ;
    u32 poll_count = 0;
    u32 reg_val = 0;

    // Read the current state of the DWC_ddr2_pctl core.
    // Continue polling if the current state is a transitory state
    while(current_state == UPCTL_STATE_CONFIG_REQ ||
          current_state == UPCTL_STATE_ACCESSREQ ||
          current_state == UPCTL_STATE_LOWPOWERENTRYREQ ||
          current_state == UPCTL_STATE_LOWPOWEREXITREQ)
    {
        reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_OSCS_STAT_ADDRESS);  //R uPCTL 0x008

        current_state = reg_val & 0x7;
        poll_count = poll_count + 1;
        if(poll_count > TB_POLL_TIMEOUT)
        {
            PRINT_ERR("State change request is timed out\n");
        }
    }

    if(current_state != req_state)
    {
        /* Move to the requested state using the lpddr_state_cmd_t task */
        switch(current_state)
        {
            case UPCTL_STATE_INITMEM:
            {
                switch(req_state)
                {
                    case UPCTL_STATE_CONFIG:
                    {
                        lpddr_state_cmd_t(UPCTL_CMD_CFG);
                    }
                    break;

                    case UPCTL_STATE_ACCESS:
                    {
                        lpddr_state_cmd_t(UPCTL_CMD_CFG);
                        lpddr_state_cmd_t(UPCTL_CMD_GO);
                    }
                    break;

                    case UPCTL_STATE_LOWPOWER:
                    {
                        lpddr_state_cmd_t(UPCTL_CMD_CFG);
                        lpddr_state_cmd_t(UPCTL_CMD_GO);
                        lpddr_state_cmd_t(UPCTL_CMD_SLEEP);
                    }
                    break;

                    default:
                    {
                        PRINT_ERR("Unsupported state change!\n");
                    }
                    break;
                }
            }
            break;

            case UPCTL_STATE_CONFIG:
            {
                switch(req_state)
                {
                    case UPCTL_STATE_INITMEM:
                    {
                        lpddr_state_cmd_t(UPCTL_CMD_INIT);
                    }
                    break;

                    case UPCTL_STATE_ACCESS:
                    {
                        lpddr_state_cmd_t(UPCTL_CMD_GO);
                    }
                    break;

                    case UPCTL_STATE_LOWPOWER:
                    {
                        lpddr_state_cmd_t(UPCTL_CMD_GO);
                        lpddr_state_cmd_t(UPCTL_CMD_SLEEP);
                    }
                    break;

                    default:
                    {

                        PRINT_ERR("Unsupported state change!\n");
                    }
                    break;
                }
            }
            break;

            case UPCTL_STATE_ACCESS:
            {
                switch(req_state)
                {
                    case UPCTL_STATE_INITMEM:
                    {
                        lpddr_state_cmd_t(UPCTL_CMD_CFG);
                        lpddr_state_cmd_t(UPCTL_CMD_INIT);
                    }
                    break;

                    case UPCTL_STATE_CONFIG:
                    {
                        lpddr_state_cmd_t(UPCTL_CMD_CFG);
                    }
                    break;

                    case UPCTL_STATE_LOWPOWER:
                    {
                        lpddr_state_cmd_t(UPCTL_CMD_SLEEP);
                    }
                    break;

                    default:
                    {
                        PRINT_ERR("Unsupported state change!\n");
                    }
                    break;
                }
            }
            break;

            case UPCTL_STATE_LOWPOWER:
            {
                switch(req_state)
                {
                    case UPCTL_STATE_INITMEM:
                    {
                        lpddr_state_cmd_t(UPCTL_CMD_WAKEUP);
                        lpddr_state_cmd_t(UPCTL_CMD_CFG);
                        lpddr_state_cmd_t(UPCTL_CMD_INIT);
                    }
                    break;

                    case UPCTL_STATE_CONFIG:
                    {
                        lpddr_state_cmd_t(UPCTL_CMD_WAKEUP);
                        lpddr_state_cmd_t(UPCTL_CMD_CFG);
                    }
                    break;

                    case UPCTL_STATE_ACCESS:
                    {
                        lpddr_state_cmd_t(UPCTL_CMD_WAKEUP);
                    }
                    break;

                    default:
                    {
                        PRINT_ERR("Unsupported state change!\n");
                    }
                    break;
                }
            }

            default:
            {
                PRINT_STR("Unsupported state change!\n");
            }
            break;
        }
    }
    else
    {
        /* Current state is already the requested state */
    }
#endif
    return;
}


/**
  * \brief
  *     Calculate LPDDR3 Timing Register's values
  *
  * @param[in]
  * \param[out]
  *        None
  * \return
  *        None
  */
void lpddr_calc_timing_regs_t(int keep_latency, int clk_speed)
{
    PLATFORM_TYPE plat = get_platform_type();
    u32 silicon_mode = (plat == PLAT_SIM && gInitOptionPtr->dramc_init_opt.std.FULL_DRAMC_INIT == 0) ? 0 : 1;
    u32 upctl_opt = gInitOptionPtr->dramc_init_opt.std.UPCTL_OPT;
    u32 mr3_opt = 0;
	
    if(plat == PLAT_CHIP)
    {
        mr3_opt = gInitOptionPtr->dramc_init_opt.shmoo.ds.mr3;
    }
	
    if(0 == keep_latency)
    {
        if(clk_speed == 200)
        {
            rd_lat = 6;
            wr_lat = 3;
        }
        else if(clk_speed == 266)
        {
            rd_lat = 8;
            wr_lat = 4;         
        }
        else if(clk_speed == 333)
        {
            rd_lat = 10;
            wr_lat = 6;
        }
        else
        {   
            rd_lat = 12;
            wr_lat = 6;
        }
    }


    /* 200, 266, 333 and 400 MHz case */
    t_wtr = div_roundup_f(t_ck, 7500, 4);         //  7500 ps
    if(t_wtr < 4)
    {
        t_wtr = 4;        // should be greater than 4
    }

    t_rp  = div_roundup_f(t_ck, 18000, 5);        // 18000 ps  // K4E8E304EB-GGCE
    
    t_prea_extra  = div_roundup_f(t_ck, 3000, 2); //  3000 ps

    t_rcd = div_roundup_f(t_ck, 18000, 5);        // 18000 ps  // K4E8E304EB-GGCE 

    t_rtp = div_roundup_f(t_ck, 7500, 4);         //  7500 ps
    t_ras = div_roundup_f(t_ck, 42000, 6);        // 42000 ps
    t_rrd = div_roundup_f(t_ck, 10000, 4);        // 10000 ps
    t_wr  = div_roundup_f(t_ck, 15000, 5);        // 15000 ps
    t_rc  = (t_ras & 0x3F) + (t_rp & 0x1F);       //

    t_mod = 0;           // DDR3 only.
    t_faw = div_roundup_f(t_ck, 50000, 6);            // PUB. 50000 ps

    t_rfc = div_roundup_f(t_ck, 130000, 9);

    // using tfaw_cfg_offset = 0
    if(t_faw <= 4*t_rrd)
        tfaw_cfg = 0;
    else if(t_faw >= 6*t_rrd)
        tfaw_cfg = 2;
    else // default
        tfaw_cfg = 1;                                // p188 uPCTL datasheet... 00=>4*tRRD, 01=>5*tRRD, 10=>6*tRRD

    t_xs = (t_rfc & 0x1FF) + div_roundup_f(t_ck, 10000, 10);    // tRFCab + 10000 ps
    t_xp = div_roundup_f(t_ck, 7500, 5);             // 7500 ps;
    t_cke = div_roundup_f(t_ck, 7500, 4);            // 7500 ps

    t_dqsck = div_roundup_f(t_ck,2500,3);            // 2500 ps
    dqsgx = (t_ck<1876) ? 0x3 : 0x2;                 // DSGCR.DQSGX
    t_dqsckmax = t_dqsck + dqsgx - 1;

    // ZQ
    t_zqcs = div_roundup_f(t_ck, 90000, 7);          // 90000 ps
    t_zqcl = div_roundup_f(t_ck, 360000, 10);        // 360000 ps
    t_ckesr = div_roundup_f(t_ck, 15000, 4);         // 15000 ps
    t_dpd = 500;                                     // 500 us

    twl = wr_lat;
    trl = rd_lat;
    
    if(clk_speed == 200)
    {
        lpddr3_mr1 = 0x83; // nWR=6
        lpddr3_mr2 = 0x04; // Enable setting for nWR<9, RL=6/WL=3(400MHz)
    }
    else
    {
        if(clk_speed == 266)
        {
            lpddr3_mr2 = 0x6;
        }
        else if(clk_speed == 333)
        {
            //trl = rd_lat; // random {10, 11, 12}
            //twl = wr_lat;  // random {6, 8, 9}

            //MR2
            if(t_wr <=9)
            {
               lpddr3_mr2 = (lpddr3_mr2 & ~(0x1<<4)) | (0x0<<4);
            }
            else
            {
               lpddr3_mr2 = (lpddr3_mr2 & ~(0x1<<4)) | (0x1<<4);
            }
            lpddr3_mr2 = (lpddr3_mr2 & ~(0x1<<5)) | (0x0<<5);    // RFU
            lpddr3_mr2 = (lpddr3_mr2 & ~(0x1<<7)) | (0x0<<7);    // WR Leveling disabed as default

            // Following code is a placeholder for other values of trl and twl.
            if(6 == twl)
            {
                lpddr3_mr2 = (lpddr3_mr2 & ~(0x1<<6)) | (0x0<<6);
                if(10 == trl)
                {
                    lpddr3_mr2 = (lpddr3_mr2 & 0xF0) | 0x8;
                }
                else if(11 == trl)
                {
                    lpddr3_mr2 = (lpddr3_mr2 & 0xF0) | 0x9;
                }
                else if(12 == trl)
                {
                    lpddr3_mr2 = (lpddr3_mr2 & 0xF0) | 0xA;
                }
                else
                {
                    PRINT_ERR("trl value is wrong\n");
                }
            }
            else if(8 == twl)
            {
                lpddr3_mr2 = (lpddr3_mr2 & ~(0x1<<6)) | (0x1<<6);
                lpddr3_mr2 = (lpddr3_mr2 & 0xF0) | 0x08;  // RL = 10, WL = 8
            }
            else if(9 == twl)
            {
                lpddr3_mr2 = (lpddr3_mr2 & ~(0x1<<6)) | (0x1<<6);
                if(11 == trl)
                {
                   lpddr3_mr2 = (lpddr3_mr2 & 0xF0) | 0x09;  // RL = 11, WL = 9
                }
                else if(12 == trl)
                {
                   
                   lpddr3_mr2 = (lpddr3_mr2 & 0xF0) | 0x0A;  // RL = 12, WL = 9
                }
                else
                {
                   PRINT_ERR("trl value is wrong\n");
                }
            }
            else
            {
                PRINT_ERR("twl value is wrong\n");
            }
        }
        else
        {
            //trl = rd_lat;  // random { 12 }
            //twl = wr_lat;  // random { 6 or 9 }

            //Following is placeholder for different values of twl.
            if(12 == trl)
            {
                if(6 == twl)
                {
                    lpddr3_mr2 = 0x1A;
                }
                else if(9 == twl)
                {
                    lpddr3_mr2 = 0x5A;
                }
                else
                {
                   PRINT_ERR("twl value is wrong\n");
                }
            }
            else
            {
               PRINT_ERR("trl value is wrong\n");
            }
            if(t_wr <= 9)  // BYP_PLL & 250MHz uPCTL test clock
            {
                lpddr3_mr2 = (lpddr3_mr2 & ~(0x1<<4)) | (0x0<<4);
            }
        }
        lpddr3_mr1 = (lpddr3_mr1 & 0xF0) | 0x3;    

        if(((lpddr3_mr2 & 0x10) >> 0x4) == 0x1)
        {
            if(t_wr == 11)
                lpddr3_mr1 = (lpddr3_mr1 & 0x0F) | 0x20;
            else if(t_wr == 12)
                lpddr3_mr1 = (lpddr3_mr1 & 0x0F) | 0x40;
            else if(t_wr == 14)
                lpddr3_mr1 = (lpddr3_mr1 & 0x0F) | 0x80;
            else if(t_wr == 16)
                lpddr3_mr1 = (lpddr3_mr1 & 0x0F) | 0xC0;
            else if(t_wr == 10)
                lpddr3_mr1 = (lpddr3_mr1 & 0x0F) | 0x00;
            else
                PRINT_ERR("LPDDR3 MR1 setting error \n");
        }
        else
        {   
            if(t_wr == 3)
                lpddr3_mr1 = (lpddr3_mr1 & 0x0F) | 0x20;
            else if(t_wr == 6)
                lpddr3_mr1 = (lpddr3_mr1 & 0x0F) | 0x80;
            else if(t_wr == 8)
                lpddr3_mr1 = (lpddr3_mr1 & 0x0F) | 0xC0;
            else if(t_wr == 9)
                lpddr3_mr1 = (lpddr3_mr1 & 0x0F) | 0xE0;
            
            else
                PRINT_ERR("LPDDR3 MR1 setting error \n");
        }
    }

	lpddr3_mr3 = MR3; // (default) 0x2 : 40 OHM typical pull-down/pull-up(default)	// Elpida
    switch(mr3_opt)
    {
        case 1:
            lpddr3_mr3 = 0x1; // 34.3 ohm
            break;
        case 2:
            lpddr3_mr3 = 0x2; // 40 ohm
            break;
        case 3:
            lpddr3_mr3 = 0x3; // 48 ohm
            break;
        default:
            break;
    }
    
    // in uPCTL clock unit
    // uPCL clock is assumed as 2*t_ck
    rTOGCNT100N = div_roundup_f((2*t_ck), 100000, 7);   //  (pctl + 'h0cc), 'h00000028);
    rTOGCNT1U   = div_roundup_f((2*t_ck), 1000000, 10);  //  (pctl + 'h0c0), 'h00000190);

    // in 1 us unit
    if(silicon_mode)
    {
        rTINIT      = 200;
    }
    else
    {
        rTINIT = 0x00000001;
    }
    
    rTRSTH      = 0x00000000; //p79

    // in 100 ns unit
    // average time between refresh commands in 100ns unit. 3.9us x 2 using 2 burst refreshes

    if(disable_ref_cmd || ((upctl_opt & UPCTL_MASK_TREFI) != 0))
    {
        rTREFI  = 0x80010000;
    }
    else
    {
        rTREFI  = 0x8001004E;                           //  (pctl + 'h0d0), 'h8001004e);
    }
    
    rTZQCSI     = 0x00000005;                           //  (pctl + 'h11c), 'h00000005);  // FIXME - LHNA915

    // in memory clock
    rTMRD       = 0x00000005;                           //  (pctl + 'h0d4), 'h00000005);
                                                        // should be 5, p202
    rTRFC       = (t_rfc & 0x1FF);                      //  (pctl + 'h0d8), 'h00000068);
    rTRP        = (((t_prea_extra & 0x3) <<16)| (t_rp & 0x1F));  //  (pctl + 'h0dc), 'h00030014);
    rTAL        = 0x00000000;                           //  (pctl + 'h0e4), 'h00000000);
    rTCL        = trl;                                  //  (pctl + 'h0e8), 'h0000000C);
    rTCWL       = twl;                                  //  (pctl + 'h0ec), 'h00000006);
    rTRAS       = (t_ras & 0x3F);                       //  (pctl + 'h0f0), 'h00000022);
    rTRC        = (t_rc  & 0x3F);                       //  (pctl + 'h0f4), 'h00000036);
    rTRCD       = (t_rcd & 0x1F);                       //  (pctl + 'h0f8), 'h00000014);
    rTRRD       = (t_rrd & 0xF);                        //  (pctl + 'h0fc), 'h00000008);
    rTRTP       = (t_rtp & 0xF);                        //  (pctl + 'h100), 'h00000006);
    rTWR        = (t_wr  & 0x1F);                       //  (pctl + 'h104), 'h0000000C);
    rTWTR       = (t_wtr & 0xF);                        //  (pctl + 'h108), 'h00000006);
    rTEXSR      = (t_xs  & 0x3FF);                      //  (pctl + 'h10c), 'h00000070);
    rTXP        = (t_xp  & 0x1F);                       //  (pctl + 'h110), 'h00000006);
    rTDQS       = 0x00000004;                           //  (pctl + 'h120), 'h00000004);  
    rTRTW       = trl + (8/2) + t_dqsckmax + 1 - twl;   // RL + (8/2) + RU(tDQSCKmax/tCK) + 1 - WL = 15
    rTCKSRE     = 0x00000002;                           //  (pctl + 'h124), 'h00000002);  // should be 2. p214
    rTCKSRX     = 0x00000002;                           //  (pctl + 'h128), 'h00000002);  // should be 2. p215
    rTMOD       = (t_mod & 0x7);                        //  (pctl + 'h130), 'h00000000);
    rTCKE       = (t_cke & 0xF);                        //  (pctl + 'h12c), 'h00000006);
    rTRSTL      = 0x00000002;                           //  (pctl + 'h134), 'h00000002);  // LPDDR3 dont care
    rTZQCS      = (t_zqcs & 0x7F);                      //  (pctl + 'h118), 'h00000048);
    rTZQCL      = (t_zqcl & 0x3FF);                     //  (pctl + 'h138), 'h00000120);
    rTXPDLL     = 0x00000000;                           //  (pctl + 'h114), 'h00000000);  // should be 0. p211
    rTCKESR     = (t_ckesr & 0xF);                      //  (pctl + 'h140), 'h0000000C);
    rTDPD       = (t_dpd   & 0x3FF);                    //  (pctl + 'h144), 'h00000001);


    //dqsgx = (t_ck<1876)?0x0003:0x0002;                  //DSGCR.DQSGX
    //t_dqsckmax = (t_dqsck & 0x7) + (dqsgx & 0x3) - 1;
    twl = twl + 1;
    trl = trl + (t_dqsck & 0x7);

    pubm2_sub_hdr = 3;
    tphy_wrlat = (twl - pubm2_sub_hdr)/2;      // (4-3)/2 = 0, (7-3)/2  = 2
    trddata_en = (trl - pubm2_sub_hdr)/2;      // (7-3)/2 = 2, (13-3)/2 = 5
    bbflags_timing = 16 - t_rp;                // UPCTL_TCU_SED_P(16) - t_rp
    return;
}

void lpddr_phy_init_impd(u32 elpida)
{
#if 0
    PLATFORM_TYPE plat = get_platform_type();
    u32 zq0pr_opt = 0;
    u32 zq1pr_opt = 0;
    u32 reg_val = 0;

    if(plat == PLAT_CHIP)
    {
        zq0pr_opt = gInitOptionPtr->dramc_init_opt.shmoo.ds.zq0pr;
        zq1pr_opt = gInitOptionPtr->dramc_init_opt.shmoo.ds.zq1pr;
    }

    //SNPS recommendation
    //ZQCR.ZCAL = 0
    reg_val = *(r32*)(HUP_CHIP_DDRPHY_ZQCR_ADDRESS);
    reg_val = (reg_val & ~(0x1 << 1))| ((0x0)<<1);      
    *(r32*)(HUP_CHIP_DDRPHY_ZQCR_ADDRESS) = reg_val;

    reg_val = *(r32*)(HUP_CHIP_DDRPHY_ZQCR_ADDRESS);
    *(r32*)(HUP_CHIP_DDRPHY_ZQCR_ADDRESS) = (reg_val & ~(0x7 << 8));  // cfg_clk <= 100MHz

//    if (elpida == 0) {  /* FIXME */
    //ZQnDR.ODT_ZDEN = 1'b1
    reg_val = *(r32*)(HUP_CHIP_DDRPHY_ZQ0DR_ADDRESS);                 // AC, CK
    reg_val = (reg_val & ~(0x3FFF << 14))| ((0x0)<<14);      
    reg_val = (reg_val & ~(0x1 << 30))| ((0x1)<<30);      
    *(r32*)(HUP_CHIP_DDRPHY_ZQ0DR_ADDRESS) = reg_val;
    reg_val = *(r32*)(HUP_CHIP_DDRPHY_ZQ1DR_ADDRESS);                 // DQS, DQ, DM
    reg_val = (reg_val & ~(0x3FFF << 14))| ((0x0)<<14);      
    reg_val = (reg_val & ~(0x1 << 30))| ((0x1)<<30);      
    *(r32*)(HUP_CHIP_DDRPHY_ZQ1DR_ADDRESS) = reg_val;

    //DXTEMODE/DMTEMODE set to 2, DSTEMODE set to 1
    *(r32*)(HUP_CHIP_DDRPHY_DX0GCR2_ADDRESS) = 0xAAAA;
    reg_val = *(r32*)(HUP_CHIP_DDRPHY_DX0GCR3_ADDRESS); 
    *(r32*)(HUP_CHIP_DDRPHY_DX0GCR3_ADDRESS) = (reg_val & (~(0x3 << 12)) & (~(0x3 << 4))) | (0x2 << 12) | (0x1 << 4);
    *(r32*)(HUP_CHIP_DDRPHY_DX1GCR2_ADDRESS) = 0xAAAA;                                                                                                
    reg_val = *(r32*)(HUP_CHIP_DDRPHY_DX1GCR3_ADDRESS); 
    *(r32*)(HUP_CHIP_DDRPHY_DX1GCR3_ADDRESS) = (reg_val & (~(0x3 << 12)) & (~(0x3 << 4))) | (0x2 << 12) | (0x1 << 4);
    *(r32*)(HUP_CHIP_DDRPHY_DX2GCR2_ADDRESS) = 0xAAAA;                                                                                               
    reg_val = *(r32*)(HUP_CHIP_DDRPHY_DX2GCR3_ADDRESS); 
    *(r32*)(HUP_CHIP_DDRPHY_DX2GCR3_ADDRESS) = (reg_val & (~(0x3 << 12)) & (~(0x3 << 4))) | (0x2 << 12) | (0x1 << 4);
    *(r32*)(HUP_CHIP_DDRPHY_DX3GCR2_ADDRESS) = 0xAAAA;                                                                                              
    reg_val = *(r32*)(HUP_CHIP_DDRPHY_DX3GCR3_ADDRESS); 
    *(r32*)(HUP_CHIP_DDRPHY_DX3GCR3_ADDRESS) = (reg_val & (~(0x3 << 12)) & (~(0x3 << 4))) | (0x2 << 12) | (0x1 << 4);
//    }

    //LHNA-949 
    reg_val = *(r32*)(HUP_CHIP_DDRPHY_DXCCR_ADDRESS);             //R PUB 0x080
    reg_val =  (reg_val & ~(0xF<<5)) | (0x1<<5);        //DQSRES(8:5) = 0x1  --2.5K Ohm
    reg_val =  (reg_val & ~(0xF<<9)) | (0x9<<9);        //DQSNRES(12:9) = 0x9 --2.5K Ohm
  //reg_val =  (reg_val & ~(0xF<<5)) | (0x4<<5);        //DQSRES(8:5) = 0x4  --620 Ohm
  //reg_val =  (reg_val & ~(0xF<<9)) | (0xC<<9);        //DQSNRES(12:9) = 0xC --620 Ohm
  //reg_val =  (reg_val & ~(0xF<<5)) | (0x7<<5);        //355 ohm
  //reg_val =  (reg_val & ~(0xF<<9)) | (0xF<<9);        //355 Ohm
    *(r32*)(HUP_CHIP_DDRPHY_DXCCR_ADDRESS) = reg_val;             //W PUB 0x080

    reg_val = *(r32*)(HUP_CHIP_DDRPHY_ZQ0PR_ADDRESS);
    *(r32*)(HUP_CHIP_DDRPHY_ZQ0PR_ADDRESS) = (reg_val & (~0xff)) | (ZQDIV);
	
    switch(zq0pr_opt)
    {
        case 1:
            *(r32*)(HUP_CHIP_DDRPHY_ZQ0PR_ADDRESS) = (reg_val & (~0xF)) | (0xD); // 34.3 ohm
            break;
        case 2:
            *(r32*)(HUP_CHIP_DDRPHY_ZQ0PR_ADDRESS) = (reg_val & (~0xF)) | (0xB); // 40 ohm
            break;
        case 3:
            *(r32*)(HUP_CHIP_DDRPHY_ZQ0PR_ADDRESS) = (reg_val & (~0xF)) | (0x9); // 48 ohm
            break;
        default:
            break;
    }
	
    reg_val = *(r32*)(HUP_CHIP_DDRPHY_ZQ1PR_ADDRESS);
    *(r32*)(HUP_CHIP_DDRPHY_ZQ1PR_ADDRESS) = (reg_val & (~0xff)) | (ZQDIV);

    switch(zq1pr_opt)
    {
        case 1:
            *(r32*)(HUP_CHIP_DDRPHY_ZQ1PR_ADDRESS) = (reg_val & (~0xF)) | (0xD); // 34.3 ohm
            break;
        case 2:
            *(r32*)(HUP_CHIP_DDRPHY_ZQ1PR_ADDRESS) = (reg_val & (~0xF)) | (0xB); // 40 ohm
            break;
        case 3:
            *(r32*)(HUP_CHIP_DDRPHY_ZQ1PR_ADDRESS) = (reg_val & (~0xF)) | (0x9); // 48 ohm
            break;
        default:
            break;
    }
	

    /* FORCED ZQ UPDATE */
    reg_val = *(r32*)(HUP_CHIP_DDRPHY_ZQCR_ADDRESS);
    reg_val = (reg_val & ~(0x1 << 27)) | ((0x1)<<27);      
    *(r32*)(HUP_CHIP_DDRPHY_ZQCR_ADDRESS) = reg_val;
    reg_val = (reg_val & ~(0x1 << 27)) | ((0x0)<<27);      
    *(r32*)(HUP_CHIP_DDRPHY_ZQCR_ADDRESS) = reg_val;

    //ZQCR.ZCAL = 1
    reg_val = *(r32*)(HUP_CHIP_DDRPHY_ZQCR_ADDRESS);
    reg_val = (reg_val & ~(0x1 << 1))| ((0x1)<<1);      
    *(r32*)(HUP_CHIP_DDRPHY_ZQCR_ADDRESS) = reg_val;


    DELAY_PCLK(2000);                                   /* FIXME */
#endif
}


#define PGSR_ERR(x) ((((x) >> 20) & 0x1ff) != 0)  // ignore CA_WARN based on SNPS

/**
  * \brief
  *  This function intialize the phy and performs the phy training.
  * \param [in]
  * \param [out]
  *     None
  *
  * \return
  *     None
  */
u32 lpddr_start_phy_seq_t(int is_reprogram_req,int phy_reset_lprm, u32 bdlr, u32 vendor)
{
#if 0
    u32 reg_val = 0;
    u32 lane = 0;
    u32 i;
    PLATFORM_TYPE plat = get_platform_type();
    DRAMC_INIT_OPTION_DDR_MODE ddr_mode;	
    u32 lpddr3;
    u32 silicon_mode = (plat == PLAT_SIM && gInitOptionPtr->dramc_init_opt.std.FULL_DRAMC_INIT == 0) ? 0 : 1;
    u32 upctl_opt = gInitOptionPtr->dramc_init_opt.std.UPCTL_OPT;
    u32 phy_opt = gInitOptionPtr->dramc_init_opt.std.PHY_OPT;
    u32 retrain_off = phy_opt & 0x1;
    u32 elpida = vendor;

#ifdef LPDDR2
    ddr_mode = LPDDR_2;
#else
    ddr_mode = gInitOptionPtr->dramc_init_opt.std.DDR_MODE;
    if(ddr_mode == LPDDR_DEFAULT)
    {
        ddr_mode = (plat == PLAT_PAL || plat == PLAT_VEL) ? LPDDR_3 : LPDDR_2;
    }
#endif	
    lpddr3 = (ddr_mode == LPDDR_3) ? 1 : 0;

    //Reading the PHY revision 
    reg_val = *(r32 *)(HUP_CHIP_DDRPHY_RIDR_ADDRESS);
    rev_phy = reg_val & 0xFFF;

    // 0.09 Library version 1.55
    if((plat != PLAT_PAL) && (plat != PLAT_RPP) && (rev_phy != 0x155))
    {
        return 2;
    }

    if(REPROGRAM_NOT_REQ == is_reprogram_req)
    {    
        // Writing to register : uPCTL : DTUAWDT ::: and address : h0B0  : Data
        // :h0000024b
        //10bits col, 14 bits row, 2 rank, 3 bits bank
        *(r32*)(HUP_CHIP_DDRCTL_UPCTL_DTUCS_DTUAWDT_ADDRESS) = 0x0000024B;
    }

    /*
     * 1. Assert and Release uPCTL reset pins. Program registers if necessary:
     * (TOGCNT1U, TOGCNT100N, TINIT, TRSTH)
     */
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TOGCNT1U_ADDRESS)   = rTOGCNT1U;         //W uPCTL 0x0c0
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TINIT_ADDRESS)      = rTINIT;            //W uPCTL 0x0c4
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TOGCNT100N_ADDRESS) = rTOGCNT100N;       //W uPCTL 0x0cc

    // 
    // Writing to register : uPCTL : TRSTH ::: and address : 'h0c8  : Data : 'h00000000
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TRSTH_ADDRESS) = 0x00000000;             //W uPCTL 0x0c8 //p79

    /* 2. Configure uPCTL’s MCFG register for memory related attributes. */
                                                            //W uPCTL 0x080
    reg_val = ((0x1<<0) | (0x0<<8) | (0x2<<16) | (tfaw_cfg<<18) |(0x6<<20) | (0x0<<24));                                                       
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MCS_MCFG_ADDRESS) = reg_val;
    // Writing to register : uPCTL : MCFG1 ::: and address : 'h07c
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MCS_MCFG1_ADDRESS) = 0x1000000;  //zq_resistor_shared = 1

    if(plat != PLAT_PAL && plat != PLAT_RPP)
    {
        //Reading from register : PUB : PUB_CATR1 ::: and address : 'h0f0
        reg_val = *(r32 *)(HUP_CHIP_DDRPHY_CATR1_ADDRESS);
        reg_val =  (reg_val & ~(0xF << 24)) | (0x1 << 24);         //Bits(27:24) = 0x1
        reg_val =  (reg_val & ~(0xF << 20)) | (0x2 << 20);         //Bits(23:20) = 0x2
        // Writing to register : PUB : PUB_CATR1 ::: and address : 'h0f0  : Data : 'h00000409
        *(r32 *)(HUP_CHIP_DDRPHY_CATR1_ADDRESS) = reg_val;

        /*
         * 3. Start PHY initialization by accessing relevant PUB registers.
         * 4. Monitor PHY initialization by polling the PUB register PHY_PGSR0.IDONE
         * 14. Configure PUB’s PIR register to specify which training steps to run.
         * 15. Monitor PUB’s PHY_PGSR0.IDONE register to poll for completion of training sequence.
         * Currently PHY training is not performed in simulation
         */
        if(REPROGRAM_NOT_REQ == is_reprogram_req)
        { 
            reg_val = *(r32*)(HUP_CHIP_DDRPHY_DCR_ADDRESS);               //R PUB 0x088
            reg_val =  (reg_val & ~(0x7<<0)) | (lpddr3<<0);        //DDRMD(2:0) = 0x1
            reg_val =  (reg_val & ~(0x1<<27)) | (0x1<<27);        //DDRMD(2:0) = 0x1
            *(r32*)(HUP_CHIP_DDRPHY_DCR_ADDRESS) = reg_val;               //W PUB 0x088

            *(r32*)(HUP_CHIP_DDRPHY_IOVCR1_ADDRESS) = 0x49;  

            reg_val = *(r32*)(HUP_CHIP_DDRPHY_DTAR0_ADDRESS);               
            reg_val =  0x73ffff00;
            *(r32*)(HUP_CHIP_DDRPHY_DTAR0_ADDRESS) = reg_val;               

            reg_val = *(r32*)(HUP_CHIP_DDRPHY_DTAR1_ADDRESS);               
            reg_val =  0x73ffff08;
            *(r32*)(HUP_CHIP_DDRPHY_DTAR1_ADDRESS) = reg_val;               

            reg_val = *(r32*)(HUP_CHIP_DDRPHY_DTAR2_ADDRESS);               
            reg_val =  0x73ffff10;
            *(r32*)(HUP_CHIP_DDRPHY_DTAR2_ADDRESS) = reg_val;               

            reg_val = *(r32*)(HUP_CHIP_DDRPHY_DTAR3_ADDRESS);               
            reg_val =  0x73ffff18;
            *(r32*)(HUP_CHIP_DDRPHY_DTAR3_ADDRESS) = reg_val;               

            reg_val = *(r32*)(HUP_CHIP_DDRPHY_PGCR1_ADDRESS);
            reg_val = (reg_val & ~(0x3 << 7))| ((0x3)<<7);      // SSTL IO MODE
            *(r32*)(HUP_CHIP_DDRPHY_PGCR1_ADDRESS) = reg_val;

            // SR setting
            reg_val = *(r32 *)(HUP_CHIP_DDRPHY_ACIOCR0_ADDRESS);
            *(r32 *)(HUP_CHIP_DDRPHY_ACIOCR0_ADDRESS) = (reg_val & (~(0x3 << 30))) | (AC_SR << 30);
            reg_val = *(r32 *)(HUP_CHIP_DDRPHY_DXCCR_ADDRESS);
            *(r32 *)(HUP_CHIP_DDRPHY_DXCCR_ADDRESS) = (reg_val & (~(0x3 << 13))) | (DX_SR << 13);

            *(r32*)(HUP_CHIP_DDRPHY_PTR0_ADDRESS) = (div_roundup_f(16, 1000, 11) << 21) | (div_roundup_f(16, 4000, 15) << 6) | 16;
            *(r32*)(HUP_CHIP_DDRPHY_PTR1_ADDRESS) = (div_roundup_f(16, 100*1000, 16) << 16) | div_roundup_f(16, 9000, 13); 
        }

        //reg_val = *(r32*)(HUP_CHIP_DDRPHY_MR0_ADDRESS);                 //R PUB 0x09c
        //reg_val =  (reg_val & ~(0x1FFF<<0)) | (0x420<<0);     //MR0(12:0) = 0x420
        //DELAY_PCLK(5);
        //*(r32*)(HUP_CHIP_DDRPHY_MR0_ADDRESS) = reg_val;                 //W PUB 0x09c

        reg_val = *(r32*)(HUP_CHIP_DDRPHY_MR1_ADDRESS);                   //R PUB 0x0a0
        reg_val =  (reg_val & ~(0xFF<<0)) | lpddr3_mr1;         //MR1(7:0) = 0x43
        *(r32*)(HUP_CHIP_DDRPHY_MR1_ADDRESS) = reg_val;                   //W PUB 0x0a0

        reg_val = *(r32*)(HUP_CHIP_DDRPHY_MR2_ADDRESS);                   //R PUB 0x0a4
        reg_val =  (reg_val & ~(0xFF<<0)) | lpddr3_mr2;         //MR2(7:0) = 0x1A
        *(r32*)(HUP_CHIP_DDRPHY_MR2_ADDRESS) = reg_val;                   //W PUB 0x0a4

        reg_val = *(r32*)(HUP_CHIP_DDRPHY_MR3_ADDRESS);                   //R PUB 0x0a8
        reg_val =  (reg_val & ~(0xFF<<0)) | lpddr3_mr3;         //MR3(7:0) = 0x2
        *(r32*)(HUP_CHIP_DDRPHY_MR3_ADDRESS) = reg_val;                   //W PUB 0x0a8

        reg_val = *(r32*)(HUP_CHIP_DDRPHY_DSGCR_ADDRESS);                 //R PUB 0x084
        reg_val =  (reg_val & ~(0x3<<19)) | (0x0<<19);          //SDRMODE(20:19) = 0x0 HDR mode
        reg_val = (reg_val & ~(0x3<<6)) | ((dqsgx & 0x3)<<6);
        // markupf - note this bit must be cleared for ((upctl_opt & UPCTL_MASK_TREFI) != 0)
        // - users: DCU test
//        reg_val = (reg_val & ~(0x1<<5)) | (CUAEN<<5);     // DSGCR.CUAEN
//        reg_val = (reg_val & ~(0x1<<0)) | (PUREN<<0);     // DSGCR.PUREN
        *(r32*)(HUP_CHIP_DDRPHY_DSGCR_ADDRESS) = reg_val;                 //W PUB 0x084

        if(REPROGRAM_NOT_REQ == is_reprogram_req)
        {
            //*(r32*)(HUP_CHIP_DDRPHY_PGCR0_ADDRESS) = 0x07D81E3F;            //W PUB 0x008
            reg_val = *(r32*)(HUP_CHIP_DDRPHY_PGCR0_ADDRESS);
            reg_val = (reg_val & ~(0xFF << 0)) | ((0x37)<<0);    // 
            reg_val = (reg_val & ~(0xF << 27)) | ((0x3)<<27);    //
            *(r32*)(HUP_CHIP_DDRPHY_PGCR0_ADDRESS) = reg_val;

            reg_val = *(r32*)(HUP_CHIP_DDRPHY_PGCR1_ADDRESS);
            reg_val = (reg_val & ~(0x1 << 9)) | ((0x1)<<9);      // enable AC MDLEN
            reg_val = (reg_val & ~(0x3 << 23))| ((0x3)<<23);     // enable AC DLVT
        //  reg_val = (reg_val & ~(0x1 << 26))| ((0x1)<<26);     // disbale VT comp during training
            *(r32*)(HUP_CHIP_DDRPHY_PGCR1_ADDRESS) = reg_val;
        } 
        
        // reading from register : PUB : PUB_DTPR0 ::: and address : 'h08c
        reg_val = *(r32 *) (HUP_CHIP_DDRPHY_DTPR0_ADDRESS);
        reg_val = (reg_val & ~(0xF << 0))  | ((t_rtp & 0xF) << 0);
        reg_val = (reg_val & ~(0xF << 4))  | ((t_wtr & 0xF) << 4);
        reg_val = (reg_val & ~(0x1F << 8)) | ((((t_rp & 0x1F) + (t_prea_extra & 0x3)) & 0x1F) << 8);
        reg_val = (reg_val & ~(0x3F << 16))| ((t_ras & 0x3F) << 16);
        reg_val = (reg_val & ~(0xF << 22)) | ((t_rrd & 0xF) << 22);
        reg_val = (reg_val & ~(0x1F << 26))| ((t_rcd & 0x1F) << 26);
        *(r32 *) (HUP_CHIP_DDRPHY_DTPR0_ADDRESS) =  reg_val;

        // reading from register : PUB : PUB_DTPR1 ::: and address : 'h090
        reg_val = *(r32 *) (HUP_CHIP_DDRPHY_DTPR1_ADDRESS);
        reg_val = (reg_val & ~(0x7 << 2))   | ((t_mod & 0x7) << 2);
        reg_val = (reg_val & ~(0x3F << 5))  | ((t_faw & 0x3F)  << 5);
        reg_val = (reg_val & ~(0x1FF << 11))| ((t_rfc & 0x1FF) << 11);
        *(r32 *) (HUP_CHIP_DDRPHY_DTPR1_ADDRESS) =  reg_val;


        // reading from register : PUB : PUB_DTPR2 ::: and address : 'h094
        reg_val = *(r32 *) (HUP_CHIP_DDRPHY_DTPR2_ADDRESS);
        reg_val = (reg_val & ~(0x3FF << 0)) | ((t_xs & 0x3FF) << 0);
        reg_val = (reg_val & ~(0x1F << 10)) | ((t_xp & 0x1F) << 10);
        *(r32 *) (HUP_CHIP_DDRPHY_DTPR2_ADDRESS) =  reg_val;


        // reading from register : PUB : PUB_DTPR3 ::: and address : 'h098
        reg_val = *(r32 *) (HUP_CHIP_DDRPHY_DTPR3_ADDRESS);
        /* 155 PUB book, require 1 more clocks, when DQS gate training is disabled. pp. 122 */
        if(silicon_mode)
        {
            reg_val = (reg_val & ~(0x7 << 0))   | (((t_dqsck + 1) & 0x7) << 0);
            reg_val = (reg_val & ~(0x7 << 3))   | (((t_dqsckmax + 1) & 0x7)<< 3);
        }
        else
        {
            reg_val = (reg_val & ~(0x7 << 0))   | ((t_dqsck & 0x7) << 0);
            reg_val = (reg_val & ~(0x7 << 3))   | ((t_dqsckmax & 0x7)<< 3);
        }
        reg_val = (reg_val & ~(0x7F << 6))  | ((t_rc & 0x3F) << 6);
        reg_val = (reg_val & ~(0x1F << 13)) | ((t_cke & 0xF) << 13);

        if(t_ck > 1500)  // LHNA-1033 workaround
            reg_val = (reg_val & ~(0x1F << 18)) | (10 << 18);
        else
            reg_val = (reg_val & ~(0x1F << 18)) | ((div_roundup_f(t_ck, (15000), 5)) << 18); // t_mrd
      
        *(r32 *) (HUP_CHIP_DDRPHY_DTPR3_ADDRESS) =  reg_val;

        if(REPROGRAM_NOT_REQ == is_reprogram_req)
        {
            reg_val = *(r32*)(HUP_CHIP_DDRPHY_PGCR2_ADDRESS);                       //R PUB 0x010
            reg_val = (reg_val & ~(0x1<<18)) | (0x0<<18);                   //NOBUB(18) = 0x0
            reg_val = (reg_val & ~(0x1<<19)) | (0x0<<19);                   //FXDLT(19) = 0x0
            *(r32*)(HUP_CHIP_DDRPHY_PGCR2_ADDRESS) = reg_val;                       //W PUB 0x010

            if(phy_reset_lprm)
            {
                /* bypass ZQ calibration */
                reg_val = *(r32*)(HUP_CHIP_DDRPHY_PIR_ADDRESS);
                reg_val = (reg_val & ~(0x1<<30)) | (0x1<<30);
                *(r32*)(HUP_CHIP_DDRPHY_PIR_ADDRESS) = reg_val;
                DELAY_PCLK(20);
                /* Finished re-programming PHY initialization with ZQ disabled */
            }

            do{
                reg_val = *(r32*)(HUP_CHIP_DDRPHY_PGSR0_ADDRESS);   //R PUB 0x018, val = 0x8000000f
                if( PGSR_ERR(reg_val)) return 1; // error 
                DELAY_PCLK(20);
            }while((reg_val & 0x9ff0000f) != 0x8000000f);
          //during soft reset, status of other bits is not checked. 5.7.2014
        }
        post_status(DRAMC_INIT_PHY_BEFORE_TRAINING);
        lpddr_phy_init_impd(elpida);

        reg_val = *(r32*)(HUP_CHIP_DDRPHY_PLLCR_ADDRESS);                 //R PUB 0x020
        if(t_ck<2500)   
        {
            reg_val =  (reg_val & ~(0x3<<19)) | (0x1<<19);      //FRQSEL(20:19) = 0x1
        }
        else
        {
            reg_val =  (reg_val & ~(0x3<<19)) | (0x3<<19);      //FRQSEL(20:19) = 0x3
        }

        *(r32*)(HUP_CHIP_DDRPHY_PLLCR_ADDRESS) = reg_val;                 //W PUB 0x020

        /* reading from register : PUB : PUB_PTR3 ::: and address : h30 */
        reg_val = *(r32*)(HUP_CHIP_DDRPHY_PTR3_ADDRESS);
        if(silicon_mode)
        {
            reg_val = (reg_val & ~(0xFFFFF<<0)) | (div_roundup_f(t_ck,200*1000*1000,20)<<0);  // CKE high to first command .. 200us
            reg_val = (reg_val & ~(0x1FF<<20))  | (div_roundup_f(t_ck,100*1000,9)<<20);      // Min CLKE Low time
        }
        else
        {
            reg_val = (reg_val & ~(0xFFFFF<<0)) | (100<<0);  
            reg_val = (reg_val & ~(0x1FF<<20))  | (220<<20);
        }

        /* Writing to register : PUB : PUB_PTR3 : h030 */
        *(r32*)(HUP_CHIP_DDRPHY_PTR3_ADDRESS) = reg_val;

        /* reading from register : PUB : PUB_PTR4 ::: and address : h34 */
        reg_val = *(r32*)(HUP_CHIP_DDRPHY_PTR4_ADDRESS);
        if(silicon_mode)
        {
            reg_val = (reg_val & ~(0x3FFFF<<0)) | (div_roundup_f(t_ck,(11*1000*1000),18)<<0);  // Max auto initialization time
            reg_val = (reg_val & ~(0x7FF<<18))  | (div_roundup_f(t_ck,(1100*1000),11)<<18);    // ZQ to first command
        }
        else
        {
            reg_val = (reg_val & ~(0x3FFFF<<0)) | (80<<0);
            reg_val = (reg_val & ~(0x3FF<<18))  | (512<<18);
        }
        
        /* Writing to register : PUB : PUB_PTR4 : h034 */
        *(r32*)(HUP_CHIP_DDRPHY_PTR4_ADDRESS) = reg_val;

        if((REPROGRAM_NOT_REQ == is_reprogram_req) && (!phy_reset_lprm))  // don't overwrite DLR
        {
            *(r32*)(HUP_CHIP_DDRPHY_ACBDLR0_ADDRESS) = bdlr;  

            *(r32*)(HUP_CHIP_DDRPHY_ACBDLR6_ADDRESS) = ACBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_ACBDLR7_ADDRESS) = ACBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_ACBDLR8_ADDRESS) = ACBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_ACBDLR9_ADDRESS) = ACBDLR;  

            *(r32*)(HUP_CHIP_DDRPHY_DX0BDLR0_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX0BDLR1_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX0BDLR2_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX0BDLR3_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX0BDLR4_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX0BDLR5_ADDRESS) = DXBDLR;  
            
            *(r32*)(HUP_CHIP_DDRPHY_DX1BDLR0_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX1BDLR1_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX1BDLR2_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX1BDLR3_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX1BDLR4_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX1BDLR5_ADDRESS) = DXBDLR;  

            *(r32*)(HUP_CHIP_DDRPHY_DX2BDLR0_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX2BDLR1_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX2BDLR2_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX2BDLR3_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX2BDLR4_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX2BDLR5_ADDRESS) = DXBDLR;  

            *(r32*)(HUP_CHIP_DDRPHY_DX3BDLR0_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX3BDLR1_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX3BDLR2_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX3BDLR3_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX3BDLR4_ADDRESS) = DXBDLR;  
            *(r32*)(HUP_CHIP_DDRPHY_DX3BDLR5_ADDRESS) = DXBDLR;  

        }

        if(REPROGRAM_NOT_REQ == is_reprogram_req)
        {
            if(phy_reset_lprm)
            {
                reg_val = 0x40040000;        // ZQ bypassed & training are bypassed
            }
            else
            {
                if(silicon_mode && (plat == PLAT_CHIP))
                {
                  reg_val = PIR_WR;
                }
                else
                {
#ifdef LPDDR_FULL_TRAINING      
                    reg_val = 0x0001FD34;        //FULL training PIR Configuration
#elif defined LPDDR_PARTIAL_TRAINING
                    reg_val = PIR_WR;
#else               
                    reg_val = 0x40040000;
#endif
                }
            }
        }
        else
        {
            reg_val = *(r32*)(HUP_CHIP_DDRPHY_PIR_ADDRESS) & ~(0x1);
            reg_val = (reg_val & ~(0x1<<4)) | (0x1<<4);    // PLLINIT
            reg_val = (reg_val & ~(0x1<<5)) | (0x1<<5);    // DCAL INIT
        }

        if(upctl_opt & 0x2) 
            reg_val |= (1<<18);  // CTRLINIT

        *(r32*)(HUP_CHIP_DDRPHY_PIR_ADDRESS) = reg_val;
#ifdef  REG_DUMP   /* dump before */
        for(i = 0; i <= 0x112; i++) {
            *(r32*)(REGS_DUMP_ADDR+(i<<2)) = *(r32*)(HUP_CHIP_DDRPHY_ADDRESS+(i<<2));
        }
#endif
        reg_val = (reg_val & ~(0x1<<0)) | (0x1<<0);    // INIT
		
        *(r32*)(HUP_CHIP_DDRPHY_PIR_ADDRESS) = reg_val;

//      if(REPROGRAM_NOT_REQ == is_reprogram_req)
//      {
//          /*
//           * Generate ACK for DFI Controller update in PHY as it resets read data
//           * fifo internally in PHY
//           */
//          /* Reading from register : PUB : PUB_DSGCR ::: and address : h084 */
//          reg_val = *(r32*)(HUP_CHIP_DDRPHY_DSGCR_ADDRESS);
//          reg_val = (reg_val & ~(0x1<<5)) | (CUAEN<<5);     // DSGCR.CUAEN
//          // markupf - note this bit must be cleared for ((upctl_opt & UPCTL_MASK_TREFI) != 0)
//          // - users: DCU test
//          reg_val = (reg_val & ~(0x1<<0)) | (PUREN<<0);     // DSGCR.PUREN
//
//          /* Writing to register : PUB_DSGCR ::: address : h084  : Data : h002064bf */
//          *(r32*)(HUP_CHIP_DDRPHY_DSGCR_ADDRESS) = reg_val;
//      }

//        reg_val = *(r32*)(HUP_CHIP_DDRPHY_DTCR_ADDRESS);
//        reg_val = (reg_val & ~(0x1 << 20)) | (0x1 << 20);
//        reg_val = (reg_val & ~(0xF << 16)) | (0x2 << 16);
//        *(r32*)(HUP_CHIP_DDRPHY_DTCR_ADDRESS) = reg_val;

        DELAY_PCLK(32);

        if(phy_reset_lprm)
        {
            do{    // Reading from register : PUB : PUB_PGSR0 ::: and address : 'h018
                reg_val = *(r32*)(HUP_CHIP_DDRPHY_PGSR0_ADDRESS);
                DELAY_PCLK(32);
            }while((reg_val & 0xF) != 0xF);
        }
        else
        {
            if(silicon_mode && (plat == PLAT_CHIP)) // partial training
            {
                do{
                    reg_val = *(r32 *)(HUP_CHIP_DDRPHY_PGSR0_ADDRESS);  //R PUB 0x018,
                    DELAY_PCLK(32);
                    if (retrain_off == 0) {
                        if( PGSR_ERR(reg_val)) return 1; // error
                    }
                } while((reg_val & 0x9FFFFFFF) != PSGR_RD);  
            }
            else
            {
#ifdef LPDDR_FULL_TRAINING
                do{
                    reg_val = *(r32*)(HUP_CHIP_DDRPHY_PGSR0_ADDRESS);   //R  PUB  0x018,
                    DELAY_PCLK(32);
                    if( PGSR_ERR(reg_val)) return 1; // error
                } while((reg_val & 0x9FFFFFFF) != 0x80001FDF);
#elif defined LPDDR_PARTIAL_TRAINING
                do{
                    reg_val = *(r32 *)(HUP_CHIP_DDRPHY_PGSR0_ADDRESS);  //R PUB 0x018,
                    DELAY_PCLK(32);
                    if( PGSR_ERR(reg_val)) return 1; // error
                } while((reg_val & 0x9FFFFFFF) != PSGR_RD);  
#else
                do{
                    reg_val = *(r32*)(HUP_CHIP_DDRPHY_PGSR0_ADDRESS);   //R  PUB  0x018,
                    DELAY_PCLK(32);
                } while((reg_val & 0xF) != 0xF);
#endif
            }
        }
        if(plat == PLAT_CHIP)
        {

#ifdef REG_DUMP /* after dump */
            for(i = 0; i <= 0x112; i++) {
                *(r32*)(REGS_DUMP_ADDR+((i+0x128)<<2)) = *(r32*)(HUP_CHIP_DDRPHY_ADDRESS+(i<<2));
            }
#endif
            // training stat
            gBootDiagPtr->dramc.training_stat.pir = *(r32*)(HUP_CHIP_DDRPHY_PIR_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dtcr = *(r32*)(HUP_CHIP_DDRPHY_DTCR_ADDRESS);
            gBootDiagPtr->dramc.training_stat.mr3 = *(r32*)(HUP_CHIP_DDRPHY_MR3_ADDRESS);
            gBootDiagPtr->dramc.training_stat.zq0pr = *(r32*)(HUP_CHIP_DDRPHY_ZQ0PR_ADDRESS);     
            gBootDiagPtr->dramc.training_stat.zq1pr = *(r32*)(HUP_CHIP_DDRPHY_ZQ1PR_ADDRESS);
            gBootDiagPtr->dramc.training_stat.aclcdlr = *(r32*)(HUP_CHIP_DDRPHY_ACLCDLR_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dtedr[0] = *(r32*)(HUP_CHIP_DDRPHY_DTEDR0_ADDRESS);       
            gBootDiagPtr->dramc.training_stat.dtedr[1] = *(r32*)(HUP_CHIP_DDRPHY_DTEDR1_ADDRESS);

            gBootDiagPtr->dramc.training_stat.dx0lcdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX0LCDLR0_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0lcdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX0LCDLR1_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx1lcdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX1LCDLR0_ADDRESS); 
            gBootDiagPtr->dramc.training_stat.dx1lcdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX1LCDLR1_ADDRESS); 
            gBootDiagPtr->dramc.training_stat.dx2lcdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX2LCDLR0_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx2lcdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX2LCDLR1_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx3lcdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX3LCDLR0_ADDRESS); 
            gBootDiagPtr->dramc.training_stat.dx3lcdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX3LCDLR1_ADDRESS); 

            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX0BDLR0_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX0BDLR1_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX0BDLR2_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX0BDLR3_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX0BDLR4_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX0BDLR5_ADDRESS);
            
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX1BDLR0_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX1BDLR1_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX1BDLR2_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX1BDLR3_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX1BDLR4_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX1BDLR5_ADDRESS);
            
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX2BDLR0_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX2BDLR1_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX2BDLR2_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX2BDLR3_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX2BDLR4_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX2BDLR5_ADDRESS);
            
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX3BDLR0_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX3BDLR1_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX3BDLR2_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX3BDLR3_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX3BDLR4_ADDRESS);
            gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX3BDLR5_ADDRESS);
            
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX4BDLR0_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX4BDLR1_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX4BDLR2_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX4BDLR3_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX4BDLR4_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX4BDLR5_ADDRESS);
            
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX5BDLR0_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX5BDLR1_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX5BDLR2_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX5BDLR3_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX5BDLR4_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX5BDLR5_ADDRESS);
            
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX6BDLR0_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX6BDLR1_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX6BDLR2_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX6BDLR3_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX6BDLR4_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX6BDLR5_ADDRESS);
            
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX7BDLR0_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX7BDLR1_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX7BDLR2_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX7BDLR3_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX7BDLR4_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX7BDLR5_ADDRESS);
            
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX8BDLR0_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX8BDLR1_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX8BDLR2_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX8BDLR3_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX8BDLR4_ADDRESS);
            // gBootDiagPtr->dramc.training_stat.dx0bdlr[i] = *(r32*)(HUP_CHIP_DDRPHY_DX8BDLR5_ADDRESS);

            reg_val = *(r32*)(HUP_CHIP_DDRPHY_DTCR_ADDRESS);
            reg_val = (reg_val & ~(0xF << 16)) | 0x00;
            *(r32*)(HUP_CHIP_DDRPHY_DTCR_ADDRESS) = reg_val;
            gBootDiagPtr->dramc.training_stat.dtdr[0] = *(r32*)(HUP_CHIP_DDRPHY_DTDR0_ADDRESS) & 0xFF;
            gBootDiagPtr->dramc.training_stat.dtdr[1] = *(r32*)(HUP_CHIP_DDRPHY_DTDR1_ADDRESS) & 0xFF;
            reg_val = *(r32*)(HUP_CHIP_DDRPHY_DTCR_ADDRESS);
            reg_val = (reg_val & ~(0xF << 16)) | 0x01;
            *(r32*)(HUP_CHIP_DDRPHY_DTCR_ADDRESS) = reg_val;
            gBootDiagPtr->dramc.training_stat.dtdr[0] |= *(r32*)(HUP_CHIP_DDRPHY_DTDR0_ADDRESS) & 0xFF00;
            gBootDiagPtr->dramc.training_stat.dtdr[1] |= *(r32*)(HUP_CHIP_DDRPHY_DTDR1_ADDRESS) & 0xFF00;
            reg_val = *(r32*)(HUP_CHIP_DDRPHY_DTCR_ADDRESS);
            reg_val = (reg_val & ~(0xF << 16)) | 0x10;
            *(r32*)(HUP_CHIP_DDRPHY_DTCR_ADDRESS) = reg_val;
            gBootDiagPtr->dramc.training_stat.dtdr[0] |= *(r32*)(HUP_CHIP_DDRPHY_DTDR0_ADDRESS) & 0xFF0000;
            gBootDiagPtr->dramc.training_stat.dtdr[1] |= *(r32*)(HUP_CHIP_DDRPHY_DTDR1_ADDRESS) & 0xFF0000;
            reg_val = *(r32*)(HUP_CHIP_DDRPHY_DTCR_ADDRESS);
            reg_val = (reg_val & ~(0xF << 16)) | 0x11;
            *(r32*)(HUP_CHIP_DDRPHY_DTCR_ADDRESS) = reg_val;
            gBootDiagPtr->dramc.training_stat.dtdr[0] |= *(r32*)(HUP_CHIP_DDRPHY_DTDR0_ADDRESS) & 0xFF000000;
            gBootDiagPtr->dramc.training_stat.dtdr[1] |= *(r32*)(HUP_CHIP_DDRPHY_DTDR1_ADDRESS) & 0xFF000000;
    
            //shmoo parameters
            if(gInitOptionPtr->dramc_init_opt.shmoo.enable.wdqd)
            {
                reg_val = *(r32*)(HUP_CHIP_DDRPHY_DX0LCDLR1_ADDRESS);
                reg_val = (reg_val & ~(0xFF << 0)) | (gInitOptionPtr->dramc_init_opt.shmoo.wdqd.dx0lcdlr1 & 0xFF);
                *(r32*)(HUP_CHIP_DDRPHY_DX0LCDLR1_ADDRESS) = reg_val;
                gBootDiagPtr->dramc.shmoo_override.dx0lcdlr1 = reg_val;
                reg_val = *(r32*)(HUP_CHIP_DDRPHY_DX1LCDLR1_ADDRESS);           
                reg_val = (reg_val & ~(0xFF << 0)) | (gInitOptionPtr->dramc_init_opt.shmoo.wdqd.dx1lcdlr1 & 0xFF);
                *(r32*)(HUP_CHIP_DDRPHY_DX1LCDLR1_ADDRESS) = reg_val;
                gBootDiagPtr->dramc.shmoo_override.dx1lcdlr1 = reg_val;
                reg_val = *(r32*)(HUP_CHIP_DDRPHY_DX2LCDLR1_ADDRESS);           
                reg_val = (reg_val & ~(0xFF << 0)) | (gInitOptionPtr->dramc_init_opt.shmoo.wdqd.dx2lcdlr1 & 0xFF);
                *(r32*)(HUP_CHIP_DDRPHY_DX2LCDLR1_ADDRESS) = reg_val;
                gBootDiagPtr->dramc.shmoo_override.dx2lcdlr1 = reg_val;
                reg_val = *(r32*)(HUP_CHIP_DDRPHY_DX3LCDLR1_ADDRESS);           
                reg_val = (reg_val & ~(0xFF << 0)) | (gInitOptionPtr->dramc_init_opt.shmoo.wdqd.dx3lcdlr1 & 0xFF);
                *(r32*)(HUP_CHIP_DDRPHY_DX3LCDLR1_ADDRESS) = reg_val;
                gBootDiagPtr->dramc.shmoo_override.dx3lcdlr1 = reg_val;
            }
            if(gInitOptionPtr->dramc_init_opt.shmoo.enable.rdqsd)
            {
                reg_val = *(r32*)(HUP_CHIP_DDRPHY_DX0LCDLR1_ADDRESS);
                reg_val = (reg_val & ~(0xFF << 8)) | ((gInitOptionPtr->dramc_init_opt.shmoo.rdqsd.dx0lcdlr1 & 0xFF) << 8);
                *(r32*)(HUP_CHIP_DDRPHY_DX0LCDLR1_ADDRESS) = reg_val;
                gBootDiagPtr->dramc.shmoo_override.dx0lcdlr1 = reg_val;
                reg_val = *(r32*)(HUP_CHIP_DDRPHY_DX1LCDLR1_ADDRESS);           
                reg_val = (reg_val & ~(0xFF << 8)) | ((gInitOptionPtr->dramc_init_opt.shmoo.rdqsd.dx1lcdlr1 & 0xFF) << 8);  
                *(r32*)(HUP_CHIP_DDRPHY_DX1LCDLR1_ADDRESS) = reg_val;
                gBootDiagPtr->dramc.shmoo_override.dx1lcdlr1 = reg_val;         
                reg_val = *(r32*)(HUP_CHIP_DDRPHY_DX2LCDLR1_ADDRESS);           
                reg_val = (reg_val & ~(0xFF << 8)) | ((gInitOptionPtr->dramc_init_opt.shmoo.rdqsd.dx2lcdlr1 & 0xFF) << 8);
                *(r32*)(HUP_CHIP_DDRPHY_DX2LCDLR1_ADDRESS) = reg_val;
                gBootDiagPtr->dramc.shmoo_override.dx2lcdlr1 = reg_val;         
                reg_val = *(r32*)(HUP_CHIP_DDRPHY_DX3LCDLR1_ADDRESS);           
                reg_val = (reg_val & ~(0xFF << 8)) | ((gInitOptionPtr->dramc_init_opt.shmoo.rdqsd.dx3lcdlr1 & 0xFF) << 8);
                *(r32*)(HUP_CHIP_DDRPHY_DX3LCDLR1_ADDRESS) = reg_val;
                gBootDiagPtr->dramc.shmoo_override.dx3lcdlr1 = reg_val;     
            }
            if(gInitOptionPtr->dramc_init_opt.shmoo.enable.rdqsnd)
            {
                reg_val = *(r32*)(HUP_CHIP_DDRPHY_DX0LCDLR1_ADDRESS);
                reg_val = (reg_val & ~(0xFF << 16)) | ((gInitOptionPtr->dramc_init_opt.shmoo.rdqsd.dx0lcdlr1 & 0xFF) << 16);
                *(r32*)(HUP_CHIP_DDRPHY_DX0LCDLR1_ADDRESS) = reg_val;
                gBootDiagPtr->dramc.shmoo_override.dx0lcdlr1 = reg_val;         
                reg_val = *(r32*)(HUP_CHIP_DDRPHY_DX1LCDLR1_ADDRESS);           
                reg_val = (reg_val & ~(0xFF << 16)) | ((gInitOptionPtr->dramc_init_opt.shmoo.rdqsd.dx1lcdlr1 & 0xFF) << 16);
                *(r32*)(HUP_CHIP_DDRPHY_DX1LCDLR1_ADDRESS) = reg_val;
                gBootDiagPtr->dramc.shmoo_override.dx1lcdlr1 = reg_val;         
                reg_val = *(r32*)(HUP_CHIP_DDRPHY_DX2LCDLR1_ADDRESS);           
                reg_val = (reg_val & ~(0xFF << 16)) | ((gInitOptionPtr->dramc_init_opt.shmoo.rdqsd.dx2lcdlr1 & 0xFF) << 16);
                *(r32*)(HUP_CHIP_DDRPHY_DX2LCDLR1_ADDRESS) = reg_val;
                gBootDiagPtr->dramc.shmoo_override.dx2lcdlr1 = reg_val;         
                reg_val = *(r32*)(HUP_CHIP_DDRPHY_DX3LCDLR1_ADDRESS);           
                reg_val = (reg_val & ~(0xFF << 16)) | ((gInitOptionPtr->dramc_init_opt.shmoo.rdqsd.dx3lcdlr1 & 0xFF) << 16);
                *(r32*)(HUP_CHIP_DDRPHY_DX3LCDLR1_ADDRESS) = reg_val;
                gBootDiagPtr->dramc.shmoo_override.dx3lcdlr1 = reg_val;         
            }
            if(gInitOptionPtr->dramc_init_opt.shmoo.enable.ac)
            {
                reg_val = *(r32*)(HUP_CHIP_DDRPHY_ACLCDLR_ADDRESS);
                reg_val = (reg_val & ~(0xFF << 0)) | (gInitOptionPtr->dramc_init_opt.shmoo.ac.acd & 0xFF);
                *(r32*)(HUP_CHIP_DDRPHY_ACLCDLR_ADDRESS) = reg_val;
                gBootDiagPtr->dramc.shmoo_override.aclcdlr = reg_val;
            }
        }
		
        reg_val = *(r32*)(HUP_CHIP_DDRPHY_DSGCR_ADDRESS);
        reg_val = (reg_val & ~(0x1<<5)) | (CUAEN<<5);     // DSGCR.CUAEN
        reg_val = (reg_val & ~(0x1<<0)) | (PUREN<<0);     // DSGCR.PUREN
        *(r32*)(HUP_CHIP_DDRPHY_DSGCR_ADDRESS) = reg_val;

    //  reg_val = *(r32*)(HUP_CHIP_DDRPHY_DSGCR_ADDRESS);
    //  reg_val = (reg_val & ~(0x1<<5)) | (0x1<<5);          // DSGCR.CUAEN
    //  *(r32*)(HUP_CHIP_DDRPHY_DSGCR_ADDRESS) = reg_val;

    //  reg_val = *(r32*)(HUP_CHIP_DDRPHY_PGCR1_ADDRESS);
    //  reg_val = (reg_val & ~(0x1 << 26))| ((0x0)<<26);     // enable VT comp after training
    //  *(r32*)(HUP_CHIP_DDRPHY_PGCR1_ADDRESS) = reg_val;

        post_status(DRAMC_INIT_PHY_AFTER_TRAINING);

        if(REPROGRAM_NOT_REQ == is_reprogram_req)
        {
            if(phy_reset_lprm)
            {
                zqndr0[0] = (zqndr0[0] & ~(0x1<<28)) | (0x1<<28);
                zqndr0[1] = (zqndr0[1] & ~(0x1<<28)) | (0x1<<28);
                zqndr0[2] = (zqndr0[2] & ~(0x1<<28)) | (0x1<<28);
                zqndr0[3] = (zqndr0[3] & ~(0x1<<28)) | (0x1<<28);
                
                /*Register name in the RTL code is mentioned as ZQ0CR! It should be ZQ0DR.*/
                *(r32*)(HUP_CHIP_DDRPHY_ZQ0DR_ADDRESS) = zqndr0[0];
                *(r32*)(HUP_CHIP_DDRPHY_ZQ1DR_ADDRESS) = zqndr0[1];
                // *(r32*)(HUP_CHIP_DDRPHY_ZQ2DR) = zqndr0[2];
                // *(r32*)(HUP_CHIP_DDRPHY_ZQ3DR) = zqndr0[3];
            }
        }
    }
#endif
    return 0;
}

/**
  * \brief
  *  This function writes LPDDR3 timing registers
  * \param [in]
  * \param [out]
  *     None
  *
  * \return
  *     None
  */
void lpddr_write_timing_regs_seq_t(void)
{
#if 0
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TOGCNT1U_ADDRESS)   = rTOGCNT1U;             //W uPCTL 0x0c0
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TINIT_ADDRESS)      = rTINIT;                //W uPCTL 0x0c4
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TOGCNT100N_ADDRESS) = rTOGCNT100N;           //W uPCTL 0x0cc
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TREFI_ADDRESS)      = rTREFI;                //W uPCTL 0x0d0
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TMRD_ADDRESS)       = rTMRD;                 //W uPCTL 0x0d4
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TRFC_ADDRESS)       = rTRFC;                 //W uPCTL 0x0d8
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TRP_ADDRESS)        = rTRP;                  //W uPCTL 0x0dc
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TAL_ADDRESS)        = rTAL;                  //W uPCTL 0x0e4
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TCL_ADDRESS)        = rTCL;                  //W uPCTL 0x0e8
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TCWL_ADDRESS)       = rTCWL;                 //W uPCTL 0x0ec
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TRAS_ADDRESS)       = rTRAS;                 //W uPCTL 0x0f0
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TRC_ADDRESS)        = rTRC;                  //W uPCTL 0x0f4
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TRCD_ADDRESS)       = rTRCD;                 //W uPCTL 0x0f8
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TRRD_ADDRESS)       = rTRRD;                 //W uPCTL 0x0fc
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TRTP_ADDRESS)       = rTRTP;                 //W uPCTL 0x100
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TWR_ADDRESS)        = rTWR;                  //W uPCTL 0x104
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TWTR_ADDRESS)       = rTWTR;                 //W uPCTL 0x108
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TEXSR_ADDRESS)      = rTEXSR;                //W uPCTL 0x10c
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TXP_ADDRESS)        = rTXP;                  //W uPCTL 0x110
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TDQS_ADDRESS)       = rTDQS;                 //W uPCTL 0x120
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TRTW_ADDRESS)       = rTRTW;                 //W uPCTL 0x0e0
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TCKSRE_ADDRESS)     = rTCKSRE;               //W uPCTL 0x124
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TCKSRX_ADDRESS)     = rTCKSRX;               //W uPCTL 0x128
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TMOD_ADDRESS)       = rTMOD;                 //W uPCTL 0x130
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TCKE_ADDRESS)       = rTCKE;                 //W uPCTL 0x12c
    //*(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TRSTH_ADDRESS)      = rTRSTH;              //W uPCTL 0x0c8
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TRSTL_ADDRESS)      = rTRSTL;                //W uPCTL 0x134
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TZQCS_ADDRESS)      = rTZQCS;                //W uPCTL 0x118
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TZQCL_ADDRESS)      = rTZQCL;                //W uPCTL 0x138
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TXPDLL_ADDRESS)     = rTXPDLL;               //W uPCTL 0x114
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TZQCSI_ADDRESS)     = rTZQCSI;               //W uPCTL 0x11c
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TCKESR_ADDRESS)     = rTCKESR;               //W uPCTL 0x140
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TDPD_ADDRESS)       = rTDPD;                 //W uPCTL 0x144
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MTS_TREFI_MEM_DDR3_ADDRESS) = 0x00000000;        //W uPCTL 0x148
#endif
    return;
}

/**
  * \brief
  *  This function writes DFI timing registers
  * \param [in]
  * \param [out]
  *     None
  *
  * \return
  *     None
  */
void lpddr_write_dfi_timing_regs_seq_t(void)
{
#if 0
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_DFIW_DFITPHYWRLAT_ADDRESS)   = tphy_wrlat;   //W uPCTL 0x254
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_DFIR_DFITRDDATAEN_ADDRESS)   = trddata_en;   //W uPCTL 0x260
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_DFIW_DFITPHYWRDATA_ADDRESS)  = 0x00000001;   //W uPCTL 0x250
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_DFIR_DFITPHYRDLAT_ADDRESS)   = 0x0000000F;   //W uPCTL 0x264
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_DFIS_DFITDRAMCLKDIS_ADDRESS) = 0x00000002;   //W uPCTL 0x2d4
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_DFIS_DFITDRAMCLKEN_ADDRESS)  = 0x00000002;   //W uPCTL 0x2d0
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_DFIC_DFITCTRLDELAY_ADDRESS)  = 0x00000002;   //W uPCTL 0x240
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_DFIU_DFITCTRLUPDMAX_ADDRESS) = 0x00000320;   //W uPCTL 0x284
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_DFIU_DFIUPDCFG_ADDRESS)      = 0x00000003;   //W uPCTL 0x290
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_DFIU_DFITPHYUPDTYPE0_ADDRESS) = 0x00003fff;  //W uPCTL 0x270
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_DFIU_DFITPHYUPDTYPE1_ADDRESS) = 0x00003fff;  //W uPCTL 0x274
#endif
    return;
}

/**
  * \brief
  *     lpddr memory initialization sequence for uPCTL module
  *
  * \param [in]
  * \param [out]
  *     None
  *
  * \return
  *     None
  */
void lpddr_init_memory_seq_t(void)
{
#if 0
    u32 reg_val;
    u32 i, rank;

    post_status(DRAMC_INIT_MEMORY_MRW_RESET);
    if(gInitOptionPtr->dramc_init_opt.std.NO_DRAMC_RESET == 0)
    {
        /* Issuing MRW(Reset) command */
        *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS) = 0X80F003F3;
        do{
            reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS);
        }while((reg_val & 0x80000000) != 0);
    
        /*
             * Issuing NOP commands to ensure tINIT5 is met
             * 10 usec delay is critical. Software should consider how to meet this requirement.
             * Add additional delay to avoid tINIT5 (10us) error
             */
        for (i=0; i< 14; i++)
        {
            *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS) = 0x88F00000;
            do{
                reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS);
            } while((reg_val & 0x80000000) != 0);
        }
    }

    for(rank=0;rank<2;rank++) {
        /* Issuing MRW ZQinit command */
        post_status(DRAMC_INIT_MEMORY_MRW_ZQ);		
        *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS) = 0x890FF0A3 | ((1<<rank) << 20);    // tZQINIT=1us (1000/2.5 = 400) //Elpida
        do{
            reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS);
        } while((reg_val & 0x80000000) != 0);

    //  for (i=0; i< 2; i++)
    //  {
    //      *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS) = 0x88000000 | ((1<<rank) << 20);
    //      do{
    //          reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS);
    //      } while((reg_val & 0x80000000) != 0);
    //  }
    }

    /* Issuing MRW MR2 command */
    post_status(DRAMC_INIT_MEMORY_MRW_MR2);		
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS) = (((u32)0x80F<<20) | (lpddr3_mr2<<12) | 0x023);
    do{
        reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS);
    } while((reg_val & 0x80000000) != 0);

    /* Issuing MRW MR1 command */
    post_status(DRAMC_INIT_MEMORY_MRW_MR1);		
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS) = (((u32)0x80F<<20) | (lpddr3_mr1<<12) | 0x013);
    do{
        reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS);
    } while((reg_val & 0x80000000) != 0);

    /* Issuing MRW MR3 command */
    post_status(DRAMC_INIT_MEMORY_MRW_MR3);		
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS) = (((u32)0x80F<<20) | (lpddr3_mr3<<12) | 0x033);
    do{
        reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS);
    } while((reg_val & 0x80000000) != 0);

    /* Issuing REF command */
    post_status(DRAMC_INIT_MEMORY_MRW_REF);	
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS) = 0x80F00002;
    do{
        reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS);
    } while((reg_val & 0x80000000) != 0);

#endif
    /***************** Memory initialization sequence done ********************/
    return;
}

/**
  * \brief
  *     Reprogramming MR registers.
  *
  * \param [in]
  * \param [out]
  *     None
  *
  * \return
  *     None
  */

void lpddr_reprog_mr_t()
{
#if 0
    u32 reg_val = 0;

    /* Issuing MRW MR2 command */
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS) = (((u32)0x80F<<20) | (lpddr3_mr2<<12) | 0x023);
    do{
        reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS);
    } while((reg_val & 0x80000000) != 0);

     /* Issuing MRW MR2 command */
     *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS) = (((u32)0x80F<<20) | (lpddr3_mr1<<12) | 0x013);
     do{
         reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS);
     } while((reg_val & 0x80000000) != 0);

    /* Issuing MRW MR2 command */
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS) = 0x80F00002;
    do{
        reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS);
    } while((reg_val & 0x80000000) != 0);
#endif
}

 
/**
  * \brief
  *     Configure uPCLT to refine configuration.
  *
  * \param [in]
  * \param [out]
  *     None
  *
  * \return
  *     None
  */
void lpddr_override_seq_t(int remain_low_power_state)
{
#if 0
    u32 state;
    u32 reg_val;

    reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_OSCS_STAT_ADDRESS);
    state = (reg_val & 0x7);

    post_status(DRAMC_INIT_OVERRIDE_SEQ_CONFIG);
    //remain_low_power_state is not modified anywhere! redundant check as of now! TBD
    // Moving to the Config state to change the register settings.
    if(!remain_low_power_state)
    {
        if(state != UPCTL_STATE_CONFIG) // Not in CONFIG STATE!
        {
            /* Write CFG to SCTL.state_cmd command register and poll STAT.ctl_stat = config. */
            *(r32*)(HUP_CHIP_DDRCTL_UPCTL_OSCS_SCTL_ADDRESS) = UPCTL_CMD_CFG;      //W uPCTL 0x004

            do{
                reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_OSCS_STAT_ADDRESS);        //R uPCTL 0x008
            }while((reg_val & 0x7)!= UPCTL_STATE_CONFIG);

        }
    }

    //Read and write, if required, any of the Timing Registers,SCFG,and MCFG registers.
    reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MCS_MCFG_ADDRESS);
    //*(r32*)(HUP_CHIP_DDRCTL_UPCTL_MCS_MCFG_ADDRESS) = 0x00620001 | ((tfaw_cfg & 0x3) << 18);  //W uPCTL 0x080
    reg_val = (reg_val & ~(0xFF<<24))| (0x0<<24);
    reg_val = (reg_val & ~(0xFF<<8)) | (0x0<<8);
    reg_val = (reg_val & ~(0x3<<18)) | (tfaw_cfg<<18);
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MCS_MCFG_ADDRESS) = reg_val;
    DELAY_PCLK(7);

    reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MCS_MCFG1_ADDRESS);            //R uPCTL 0x07C
    DELAY_PCLK(4);

    //Zero offset
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MCS_MCFG1_ADDRESS) = 0x01000000;         //W uPCTL 0x07c zq_resistor_shared = 1
    DELAY_PCLK(7);

    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_OSCS_SCFG_ADDRESS)      = (((ac_pdd_add_del & 0x1F)<<12) | ((bbflags_timing & 0xF)<<8) | ((ac_pdd_en & 0x1)<<7)| 0x1);     //W uPCTL 0x000
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MCS_PPCFG_ADDRESS)     = 0x00000000;     //W uPCTL 0x084
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_DFIS_DFISTCFG0_ADDRESS) = 0x00000003;     //W uPCTL 0x2c4
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_DFIS_DFISTCFG1_ADDRESS) = 0x00000003;     //W uPCTL 0x2c8
    //disable parity logic
    //*(r32*)(HUP_CHIP_DDRCTL_UPCTL_DFIS_DFISTCFG2_ADDRESS) = 0x00000003;     //W uPCTL 0x2d8
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_DFIS_DFISTCFG2_ADDRESS) = 0x00000000;     //W uPCTL 0x2d8
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_DFILP_ADDRESS) = 0x00070101;     //W uPCTL 0x2f0
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ECCCS_ADDRESS)    = 0x00000000;     //W uPCTL 0x180

    lpddr_write_timing_regs_seq_t();
    lpddr_write_dfi_timing_regs_seq_t();

    post_status(DRAMC_INIT_OVERRIDE_SEQ_MCMD);
    if(!remain_low_power_state)
    { 
        //Illegal MWR based on Elpida
        //*(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS) = 0x80f00003;                 //W uPCTL 0x040
        //do{
        //    reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS);                //R uPCTL 0x040
        //}while((reg_val & 0x80000000)!= 0x0);

        *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS) = ((u32)0x80F << 20) | (lpddr3_mr1 << 12) | 0x13;
                                                                    //W uPCTL 0x040
        do{
            reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS);                //R uPCTL 0x040
        }while((reg_val & 0x80000000)!= 0x0);

        *(r32*)(HUP_CHIP_DDRPHY_MR1_ADDRESS) = (u32)lpddr3_mr1;               //W PUB 0x0A0     

    
        //Moving back to the original state
        lpddr_move_state_t(state);
    }
    else
    {
        /*
         * This is a dummy write.
         * Ensuring task override_t does not complete until all APB accesess
         * have completed
         */
        *(r32*)(HUP_CHIP_DDRCTL_UPCTL_OSCS_STAT_ADDRESS) = UPCTL_STATE_INITMEM;
    }
#endif
    return;
}

/**
  * \brief
  *     Enable Static mode
  *
  * \param [in]
  * \param [out]
  *
  * \return void
  */
  
void lpddr_enable_static_mode_t()
{
#if 0
    int state;
    int reg_val = 0;

    reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_OSCS_STAT_ADDRESS);
    state = (reg_val & 0x7);                    // save state

    
    lpddr_move_state_t(UPCTL_STATE_CONFIG);

    // reading from register : PUB : PUB_PGCR3 ::: and address : 'h14
    reg_val = *(r32*)(HUP_CHIP_DDRPHY_PGCR3_ADDRESS);
    reg_val = (reg_val & ~(0x3<<3)) | (0x3<<3);   //bits[4:3] = 0x3
    reg_val = (reg_val & ~(0xF<<5)) | (0x4<<5);   //bits[8:5] = 0x4
    reg_val = (reg_val & ~(0x1<<24))| (0x1<<24);  //bits[24]  = 0x1

    // Writing to register : PUB : PUB_PGCR3 ::: and address : 'h014  : Data : 'h00f92480
    *(r32*)(HUP_CHIP_DDRPHY_PGCR3_ADDRESS) = reg_val;
    
    lpddr_move_state_t(state);
#endif
}

/**
  * \brief
  *     Disable Static mode
  *
  * \param [in]
  * \param [out]
  *
  * \return void
  */
  
void lpddr_disable_static_mode_t()
{
#if 0
    int reg_val = 0;
    int state; 
    reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_OSCS_STAT_ADDRESS);
    state = (reg_val & 0x7);                    // save state

    // reading from register : PUB : PUB_PGCR3 ::: and address : 'h14
    reg_val = *(r32*)(HUP_CHIP_DDRPHY_PGCR3_ADDRESS);
    reg_val = (reg_val & ~(0x3<<3)) | (0x0<<3);   //bits[4:3] = 0x0
    reg_val = (reg_val & ~(0xF<<5)) | (0x0<<5);   //bits[8:5] = 0x0
    reg_val = (reg_val & ~(0x1<<24))| (0x0<<24);  //bits[24]  = 0x0

    // Writing to register : PUB : PUB_PGCR3 ::: and address : 'h014  : Data : 'h00f92480
    *(r32*)(HUP_CHIP_DDRPHY_PGCR3_ADDRESS) = reg_val;
#endif    
}


/**
  * \brief
  *     Configure scheduler.
  *
  * \param [in]
  * \param [out]
  *     None
  *
  * \return
  *     None
  */
void lpddr_scheduler_seq_t( void)
{
#if 0
    u32 reg_val;
    unsigned int ddr_conf_option;
    DRAMC_INIT_OPTION_DDR_RANKS ddr_ranks;
    
    //    reg_val = *(r32*)CHIP_AFABRIC_SCHEDULER_SCHEDULER_DDRMODE_ADDRESS;
    //    *(r32*)CHIP_AFABRIC_SCHEDULER_SCHEDULER_DDRMODE_ADDRESS = reg_val | 0x1;

    ddr_conf_option = gInitOptionPtr->dramc_init_opt.std.DDR_CONF;
    ddr_ranks = (DRAMC_INIT_OPTION_DDR_SIZE)gInitOptionPtr->dramc_init_opt.std.DDR_RANKS;
    if ((ddr_ranks == LPDDR_RANKS_DOUBLE) && (ddr_conf_option == 0))
    {
      ddr_conf_option = 2;
    }

    //    *(r32*)CHIP_AFABRIC_SCHEDULER_SCHEDULER_DDRCONF_ADDRESS = ddr_conf_option;
#endif
}


/**
  * \brief
  *     lpddr initialization sequence for uPCTL module
  *     (alias lpddr_initialize_seq_t)
  *
  * \param [in]
  * \param [out]
  *
  * \return _LAHAINA_STS_OK, If the call is successful, Error code otherwise
  */


void por_dram_seq(u32 speed) { //Setup clks and reset to MC
    UINT32 rd_data;
    UINT32 read_data, read_data1, clkdiv_read_data;
    UINT32 write_data, write_data1, i, clkdiv_write_data;
    UINT32 post_div1, post_div2;

    if(speed != 1600) {
        read_data = hw_read32((UINT32*)(HUP_CHIP_POR_PLL2CONFIG1_ADDRESS));
        write_data = read_data | HUP_POR_PLL2CONFIG1_PD_FIELD_MASK; //Setting PD to 1

        hw_write32((UINT32*)((HUP_CHIP_POR_PLL2CONFIG1_ADDRESS)),write_data);
        hw_write32((UINT32*)((HUP_CHIP_POR_PLL3CONFIG1_ADDRESS)),write_data);
    
        if(speed == 800) {
            post_div2 = 0x2;
            post_div1 = 0x2;
        }
        else if (speed == 400) {
            post_div2 = 0x2;
            post_div1 = 0x4;
        } else if (speed==1333) {
            post_div2 = 0x1;
            post_div1 = 0x2;

            read_data = hw_read32((UINT32*)(HUP_CHIP_POR_PLL2CONFIG0_ADDRESS));
            read_data = read_data & 0xFFF000FF;
            write_data = read_data | (0x36 << HUP_POR_PLL2CONFIG0_FBDIV_LSB);
            hw_write32((UINT32*)((HUP_CHIP_POR_PLL2CONFIG0_ADDRESS)),write_data);
            hw_write32((UINT32*)((HUP_CHIP_POR_PLL3CONFIG0_ADDRESS)),write_data);

            read_data = hw_read32((UINT32*)(HUP_CHIP_POR_PLL2CONFIG1_ADDRESS));
            read_data = read_data & 0xFF800000; //clear fbdiv resident value
            write_data = read_data | 0x400000;
            hw_write32((UINT32*)((HUP_CHIP_POR_PLL2CONFIG1_ADDRESS)),write_data);
            hw_write32((UINT32*)((HUP_CHIP_POR_PLL3CONFIG1_ADDRESS)),write_data);
        }   else if (speed==1066) {
            post_div2 = 0x1;
            post_div1 = 0x2;

            read_data = hw_read32((UINT32*)(HUP_CHIP_POR_PLL2CONFIG0_ADDRESS));
            read_data = read_data & 0xFFF000FF;
            write_data = read_data | (0x2B << HUP_POR_PLL2CONFIG0_FBDIV_LSB);
            hw_write32((UINT32*)((HUP_CHIP_POR_PLL2CONFIG0_ADDRESS)),write_data);
            hw_write32((UINT32*)((HUP_CHIP_POR_PLL3CONFIG0_ADDRESS)),write_data);

            read_data = hw_read32((UINT32*)(HUP_CHIP_POR_PLL2CONFIG1_ADDRESS));
            read_data = read_data & 0xFF800000; //clear fbdiv resident value
            write_data = read_data | 0x605000;
            hw_write32((UINT32*)((HUP_CHIP_POR_PLL2CONFIG1_ADDRESS)),write_data);
            hw_write32((UINT32*)((HUP_CHIP_POR_PLL3CONFIG1_ADDRESS)),write_data);
        }

        read_data = hw_read32((UINT32*)(HUP_CHIP_POR_PLL2CONFIG0_ADDRESS));
        read_data = read_data & 0xFFFFFFC0 ; //clearing POSTVI1 & POSTDIV2 values
        write_data = read_data | (post_div2 << HUP_POR_PLL2CONFIG0_POSTDIV2_LSB) | (post_div1 << HUP_POR_PLL2CONFIG0_POSTDIV1_LSB);
        hw_write32((UINT32*)((HUP_CHIP_POR_PLL2CONFIG0_ADDRESS)),write_data);
        hw_write32((UINT32*)((HUP_CHIP_POR_PLL3CONFIG0_ADDRESS)),write_data);

        //Set reg_override to 1
        read_data1 = hw_read32((UINT32*)(HUP_CHIP_POR_PLL2CONFIG0_ADDRESS));
        write_data1 = read_data1 | 0x80;
        hw_write32((UINT32*)(HUP_CHIP_POR_PLL2CONFIG0_ADDRESS),write_data1);
        hw_write32((UINT32*)(HUP_CHIP_POR_PLL3CONFIG0_ADDRESS),write_data1);

        //Set reg_override to 0
        write_data1 = read_data1 & 0xFFFFFF7F;
        hw_write32((UINT32*)(HUP_CHIP_POR_PLL2CONFIG0_ADDRESS),write_data1);
        hw_write32((UINT32*)(HUP_CHIP_POR_PLL3CONFIG0_ADDRESS),write_data1);
    }

    read_data = hw_read32((UINT32*)(HUP_CHIP_POR_PLL2CONFIG1_ADDRESS));
    write_data = read_data & 0x7FFFFFFF;

    //Set PD bit to 0
    hw_write32((UINT32*)((HUP_CHIP_POR_PLL2CONFIG1_ADDRESS)),write_data);
    hw_write32((UINT32*)((HUP_CHIP_POR_PLL3CONFIG1_ADDRESS)),write_data);

    //Set reg_override to 1
    read_data1 = hw_read32((UINT32*)(HUP_CHIP_POR_PLL2CONFIG0_ADDRESS));
    write_data1 = read_data1 | 0x80;
    hw_write32((UINT32*)(HUP_CHIP_POR_PLL2CONFIG0_ADDRESS),write_data1);
    hw_write32((UINT32*)(HUP_CHIP_POR_PLL3CONFIG0_ADDRESS),write_data1);

    //Set reg_override to 0
    write_data1 = read_data1 & 0xFFFFFF7F;
    hw_write32((UINT32*)(HUP_CHIP_POR_PLL2CONFIG0_ADDRESS),write_data1);
    hw_write32((UINT32*)(HUP_CHIP_POR_PLL3CONFIG0_ADDRESS),write_data1);

    hw_write32((UINT32*)((HUP_CHIP_POR_SOFTRESETRELEASE3_ADDRESS)),0x3<<21);
    clkdiv_read_data = hw_read32((UINT32*)HUP_CHIP_POR_CLKDIV_START_ADDRESS);
    clkdiv_write_data = clkdiv_read_data | (0x3<<HUP_POR_CLKDIV_START_DIV17_START_LSB);
    hw_write32((UINT32*)((HUP_CHIP_POR_CLKDIV_START_ADDRESS)),clkdiv_write_data);
    clkdiv_write_data = clkdiv_write_data | (0x3<<HUP_POR_CLKDIV_START_DIV24_START_LSB);
    hw_write32((UINT32*)((HUP_CHIP_POR_CLKDIV_START_ADDRESS)),clkdiv_write_data);

    //Clock Enable
    hw_write32((UINT32*)((HUP_CHIP_POR_CLOCKENABLE3_ADDRESS)),0x300A);
    delay_ns(50000);

    hw_write32((UINT32*)((HUP_CHIP_POR_DRAMC_CTRL_ADDRESS)),0x11); //DRAMC1 and DRAMC0 MUX Select
    hw_write32((UINT32*)((HUP_CHIP_POR_SOFTRESETRELEASE3_ADDRESS)),0x1<<20);

}



void dramc_init(int dpd)
{
    UINT32 write_data;
    UINT32 dram_frequency;
    
    POR_INIT_DRAMC_INTERLEAVE_MODE       dramc_interleave_mode;
    DRAMC_INIT_OPTION_FREQUENCY  frequency;
    
    dramc_interleave_mode = gInitOptionPtr->dramc_init_opt.std.DRAMC_INTERLEAVE_MODE;
    
    if(dramc_interleave_mode !=0) {
        write_data = dramc_interleave_mode;
        hw_write32((UINT32*)((HUP_CHIP_POR_FABRIC_MODE_ADDRESS)),write_data);
    } 
    
    frequency = (DRAMC_INIT_OPTION_FREQUENCY)gInitOptionPtr->dramc_init_opt.std.FREQUENCY_OPT;
    
    switch(frequency)
    {
        case FREQ_400MHZ:
            dram_frequency = 400;
            break;
        case FREQ_800MHZ:
            dram_frequency = 800;
            break;              
        case FREQ_1066MHZ:
            dram_frequency = 1066;
            break;
        case FREQ_1333MHZ:
            dram_frequency = 1333;
            break;
        case FREQ_DEFAULT:
        default:
            dram_frequency = 1600;
            break;
    }
    
    #ifdef REAL_MC
        por_dram_seq(dram_frequency);
        if(get_platform_type() == PLAT_PAL) {
                 
            simulation_init_seq(0,  //const int mc
                                1,  //const int dram_mode
                                3,  //const int active_ranks
                                75, //const int dram_cfg
                                -1, //const int autoref_int 
                                0,  //const int training_test_num
                                0,  //const int phy_training_mode
                                -1, //const int ddc_param_mode
                                0,  //const int wr_dbi_dis, 
                                0,  //const int rd_dbi_dis,
                                0,  //const int use_real_init_seq, 
                                dram_frequency,//const int dram_frequency, 
                                3,  //const int delay_model, 
                                0,  //const int enable_pmb, 
                                0,  //const int phy_init_train,
                                0,  //const int dump_phy_regs,
                                0,  //const int seed,
                                0,  //const int skip_mcu_init,
                                0,  //const int pmg_setup_en,
                                0,  //const int training_en,
                                0,  //const int skip_part_phy_init,
                                10, //const int mov_window_size,
                                1,  //const int mtc_adjust_step,
                                0,  //const int mtc_use_read_methodB,
                                0   //const int set_bdlr_val
                                );
            
            simulation_init_seq(1,1,3,75,-1,0,0,-1,0,0,0,dram_frequency,3,0,0,0,0,0,0,0,0,10,1,0,0);
        } else {
            simulation_init_seq(0,1,1,84,-1,0,0,-1,0,0,0,dram_frequency,3,0,0,0,0,0,0,0,0,10,1,0,0);
            simulation_init_seq(1,1,1,84,-1,0,0,-1,0,0,0,dram_frequency,3,0,0,0,0,0,0,0,0,10,1,0,0);

        }
        //init_mc(0);    
        //init_mc(1);   
    #else
        //DRAM Reset Release
        hw_write32((UINT32*)((HUP_CHIP_POR_SOFTRESETRELEASE3_ADDRESS)),0x1<<20);
    #endif
        
#if 0
    u32 speed;
    u32 bdlr, rtn;
    u32 reg_val;
    DRAMC_INIT_OPTION_FREQUENCY frequency;
    DRAMC_INIT_OPTION_DDR_MODE ddr_mode;
    PLATFORM_TYPE plat = get_platform_type();
    u32 detectRequired = (plat == PLAT_CHIP) ? 1 : 0;
    u32 detectCnt = 1;

    /* place holder for different settings */
    cs_idle_cycles = 0;
    pd_idle_cycles = 0x80;
    sr_idle_cycles = 0;
    post_status(DRAMC_INIT_BEGIN);

#ifdef LPDDR2
    ddr_mode = LPDDR_2;
#else
    ddr_mode = gInitOptionPtr->dramc_init_opt.std.DDR_MODE;
    if(ddr_mode == LPDDR_DEFAULT)
    {
        ddr_mode = (plat == PLAT_PAL || plat == PLAT_VEL) ? LPDDR_3 : LPDDR_2;
    }
#endif

    // only do these steps for 1st round
    do
    {
        /*
              *Set speed and frequency
              */
        if (detectCnt == 0)
        {
            frequency = FREQ_400MHZ;
        }
        else
        {
            frequency = (DRAMC_INIT_OPTION_FREQUENCY)gInitOptionPtr->dramc_init_opt.std.FREQUENCY_OPT;
        }
        switch(frequency)
        {
            case FREQ_400MHZ:
                speed = 200;
                break;
            case FREQ_533MHZ:
                speed = 266;
                break;              
            case FREQ_667MHZ:
                speed = 333;
                break;
            case FREQ_800MHZ:
                speed = 400;
                break;
            case FREQ_DEFAULT:
            default:
                speed = (ddr_mode == LPDDR_3) ? 400 : 266;
                break;
        }
        post_status(DRAMC_INIT_FREQUENCY);  
        lpddr_set_freq_t(dpd, speed);
         /*
        *POR
        */      
        post_status(DRAMC_INIT_POR);
        por_dramc_seq(speed);

#ifndef REAL_MC
        return;
#endif

        /* 
          *Scheduler
          */
        post_status(DRAMC_INIT_SCHEDULER);
        lpddr_scheduler_seq_t();
     
        /*
          *UPCTL and PHY
          */
        post_status(DRAMC_INIT_PHY_STARTED);
        if(0 == dpd)
        {
           bdlr = CKBDLR;
           while((rtn = lpddr_start_phy_seq_t(REPROGRAM_NOT_REQ,0, bdlr, vendor)) != 0)
           {
              if(rtn == 2)
              {
                  // PUB version mismatch
                  return;                 
              }
              por_pub_seq();
           }
        }
        else
        {
           //Deep Power Down: Start from Power-Up sequence
        }

         /*
         * 5. Monitor DFI initialization status polling DFISTSTAT0.dfi_init_complete
         */
        post_status(DRAMC_INIT_DFI);
        lpddr_dfi_initcomplete_poll_t();

        /*
         * 6. Start power-up sequence by writing POWCTL.power_up_start = 1. Monitor powerup status 
         *    by polling POWSTAT.power_up_done = 1.
         */
         post_status(DRAMC_INIT_POWER_UP);
         lpddr_start_powerup_t();

         /*
         * 7. Configure reset of uPCTL. Program all timing T* registers 
         */
         post_status(DRAMC_INIT_TIMING_REGS);
        lpddr_write_timing_regs_seq_t();

        // SCFG configuration
        *(r32*)(HUP_CHIP_DDRCTL_UPCTL_OSCS_SCFG_ADDRESS) = (((ac_pdd_add_del & 0x1F)<<12) | ((bbflags_timing & 0xF)<<8) | ((ac_pdd_en & 0x1)<<7));
    
        /* 
         * 8. Configure the DFI timing parameter registers 
         */
         post_status(DRAMC_INIT_DFI_TIMING_REGS);
        lpddr_write_dfi_timing_regs_seq_t();

        /*
         * 9. Follow memory initialization procedure.
         */
         post_status(DRAMC_INIT_MEMORY_STARTED);
        lpddr_init_memory_seq_t();

        /*
         * 10. Write DFICTRLUPD command to MCMD and poll MCMD.start_cmd = 0.
         */
         post_status(DRAMC_INIT_DFICTRLUPD);
         lpddr_dfictrlupd_cmd_t();

         /* 
          * read vendor id
          */
         if (detectCnt == 0) {
             *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MRRCFG0_ADDRESS) = 0x2;
             *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS) = 0x80100058;
             do{
                 reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MCMD_ADDRESS);
             }while((reg_val & 0x80000000) != 0);
             reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_ICS_MRRSTAT0_ADDRESS);
             if ((reg_val & 0xff)  == 0x1) {
                 vendor = 0;
             } else if ((reg_val & 0xff) == 0x3){
                 vendor = 1;
             }
         }

         detectCnt++;
    }
    while(detectRequired && (detectCnt < 2));

    /*
     * Set up phy power saving
     */
    reg_val = *(r32*)(HUP_CHIP_DDRPHY_PGCR3_ADDRESS);
    reg_val = (reg_val & ~(0x1<<9)) | (0x1<<9);         //GATEACCTLCLK
    reg_val = (reg_val & ~(0x1<<10)) | (0x1<<10);       //GATEACDDRCLK
    reg_val = (reg_val & ~(0x1<<11)) | (0x1<<11);       //GATEACRDCLK
    reg_val = (reg_val & ~(0x1<<12)) | (0x1<<12);       //DISACOE
    reg_val = (reg_val & ~(0x1<<13)) | (0x1<<13);       //GATEDXCTLCLK
    reg_val = (reg_val & ~(0x1<<14)) | (0x1<<14);       //GATEDXDDRCLK
    reg_val = (reg_val & ~(0x1<<15)) | (0x1<<15);       //GATEDXRDCLK
	*(r32*)(HUP_CHIP_DDRPHY_PGCR3_ADDRESS) = reg_val;

    /*
     * 11. Write CFG to SCTL.state_cmd register and poll STAT.ctl_stat = Config.
     */
     post_status(DRAMC_INIT_MOVE_STATE_CONFIG);
    lpddr_move_state_t(UPCTL_STATE_CONFIG);

    /*
     * 12. Configure uPCTL to refine configuration Optional step only neccessary if 
     *     changing settings programmed in step 8
     */
     post_status(DRAMC_INIT_OVERRIDE_SEQ_STARTED);
    lpddr_override_seq_t(0);

    //start_pub_training - nothing to do


#ifdef PD_IDLE
    /* enabled auto power down fucntion */
    reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MCS_MCFG_ADDRESS);
    *(r32*)(HUP_CHIP_DDRCTL_UPCTL_MCS_MCFG_ADDRESS) = (reg_val & (~(0xff << 8))) | (pd_idle_cycles << 8);
#endif

    /*
     * 16. Write GO into SCTL.state_cmd register and poll STAT.ctl_stat = Access.
     */
     post_status(DRAMC_INIT_MOVE_STATE_ACCESS);
     lpddr_move_state_t(UPCTL_STATE_ACCESS);
     
     //*(r32*)(0xCC000000) = 0xc001c0de;
     //reg_val = *(r32*)(0xCC000000);
#endif
}

/**
  * \brief
  * This is DRAM low power mode set and reset
  *
  * \param [in]     1:Power On(Wakeup) 0:Power Down(Sleep)
  * \param [out]
  *
  * \return _LAHAINA_STS_OK
  */
int dram_powerdownup(int powerMode)
{
#if 0
    u32 reg_val;

    /*
     * Check LPDDR3 Self Refresh through Nodes and SC
     * Test will flow from :
     *          -> Node Write
     *          -> SC Self Refresh entry
     *          -> Clock frequency Change
     *          -> SC Self Refresh exit
     *          -> Node Read with AutoPrecharge
     *          -> Compare
     *  The body sequence follows P2.6.1.1 of uPCTL doc.
     */

    if(DRAMC_SLEEPMODE == powerMode)
    {
        /*
         * Perform execution of sequence to enter Self-Refresh mode.
         * Perform write operation to SCTL register with SLEEP Command to enter
         * into Low Power Down mode.
         */
        *(r32*)(HUP_CHIP_DDRCTL_UPCTL_OSCS_SCTL_ADDRESS) = UPCTL_CMD_SLEEP;

        /* wait for uPCTL to go into Low power state */
        do{
            reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_OSCS_STAT_ADDRESS);
            DELAY_PCLK(10);
        }while((reg_val & 0x7)!= UPCTL_STATE_LOWPOWER);
    }
    else if(DRAMC_WAKEUPMODE == powerMode)

    {
        /*
         * if n_clk is disabled then enable it. (by default we shall always enable training)
         * Enable DTU by writing to DTUCFG register (Write DTUCFG.dtu_enable = 1)
         */
        *(r32*)(HUP_CHIP_DDRCTL_UPCTL_DTUCS2_DTUCFG_ADDRESS) = 0x00000001;

        /*
         * Perform write operation to SCTL register with WAKEUP Command
         * to exit Low Power Down mode
         */
        *(r32*)(HUP_CHIP_DDRCTL_UPCTL_OSCS_SCTL_ADDRESS) = UPCTL_CMD_WAKEUP;

        /* wait for uPCTL to go into Access state from Low power state */
        do{
            reg_val = *(r32*)(HUP_CHIP_DDRCTL_UPCTL_OSCS_STAT_ADDRESS);
            DELAY_PCLK(10);
        }while((reg_val & 0x7)!= UPCTL_STATE_ACCESS);

        /* Disable DTU by writing DTUCFG register (Write DTUCFG.dtu_enable = 0) */
        *(r32*)(HUP_CHIP_DDRCTL_UPCTL_DTUCS2_DTUCFG_ADDRESS) = 0x00000000;
    }
#endif
    return 0;
}


/**
  * \brief
  * This is LPDDR low power entry function
  *
  * \param [in]
  * \param [out]
  *
  * \return
  */
void lpddr_low_power_entry(int master_node)
{
#if 0
    lpddr_move_state_t(UPCTL_STATE_LOWPOWER);

    // wait a while for dfi_cke going low to propagate to memories 
    DELAY_NCLK(10);

    //The following is an external signal that has to be driven from outside.
    // assert retention enable 
    //force prjx_top.lahaina_dut_inst.DDR_Data_Retention_N = 0;
    //force prjx_top.lahaina_dut_inst.dramc.wrap_dramphy.dramphyW.dramphy.u_DWC_DDR3PHY.u_DWC_DDR3PHYAC_top.u_DWC_DDR3PHYAC_io.ac_ib[72]= 1;
    DELAY_NCLK(10);

    // read current value of ZQ0CR0 and write it back to all ZQnCR0
    zqndr0[0] = *(r32*)(HUP_CHIP_DDRPHY_ZQ0DR_ADDRESS);
    zqndr0[1] = *(r32*)(HUP_CHIP_DDRPHY_ZQ1DR_ADDRESS);
    // zqndr0[2] = *(r32*)(HUP_CHIP_DDRPHY_ZQ2DR);
    // zqndr0[3] = *(r32*)(HUP_CHIP_DDRPHY_ZQ3DR);

    // lpddr3_reset_pctl_t()
    // reset ctl_sdr_rst_n, ctl_rst_n, dramc_T2_presetn, dramc_T1_presetn, n_rstn
    *(r32*)(HUP_CHIP_POR_SOFTRESET0_ADDRESS) = 0x03380000;          // Reg Adrs: 0x04080000
#endif
}

/**
  * \brief
  * This is LPDDR low power entry function
  *
  * \param [in]
  * \param [out]
  *
  * \return
  */

void lpddr_low_power_exit()
{
#if 0
    u32 reg_val = 0;
    u32 phy_reset_lprm = 0;
    u32 remain_low_power_state = 0;	
    u32 rtn;
	//HACK: workaround naming update conflict in sanity
    //*(r32 *)(HUP_CHIP_POR_DRAMC_MISC_ADDRESS) = 0x00000001;
    *(r32 *)(0x04080560) = 0x00000001;
    *(r32 *)(HUP_CHIP_POR_CLOCKENABLE2_ADDRESS)  = 0x00000007;        // enable p_clk,n_clk,n_clkdiv2
    *(r32 *)(HUP_CHIP_POR_CLOCKENABLE5_ADDRESS)  = 0x02000000;        // enable memmax clock
    *(r32 *)(HUP_CHIP_POR_SOFTRESETRELEASE0_ADDRESS) = 0x03380000;    // release ctl_sdr_rst_n, ctl_rst_n,
                                                   //   dramc_T2_presetn, dramc_T1_presetn, n_rstn 
    *(r32 *)(HUP_CHIP_POR_SOFTRESETRELEASE4_ADDRESS) = 0x00003000;    // release sys_memreset_ni, memmax_T0_MReset_ni

    /* FIXME */
    while((rtn = lpddr_start_phy_seq_t(REPROGRAM_NOT_REQ,1, 0, vendor)) != 0) {
       if(rtn == 2) 
         return;                 // PUB version mismatch
       por_pub_seq();
    }

    DELAY_NCLK(10);

    // Reprogram uPCTL
    remain_low_power_state = 1;
    lpddr_override_seq_t(remain_low_power_state);
    remain_low_power_state = 0;

    DELAY_NCLK(10);

    //The following is an external signal that has to be driven from outside.
    //force prjx_top.lahaina_dut_inst.DDR_Data_Retention_N = 1;
    //force prjx_top.lahaina_dut_inst.dramc.wrap_dramphy.dramphyW.dramphy.u_DWC_DDR3PHY.u_DWC_DDR3PHYAC_top.u_DWC_DDR3PHYAC_io.ac_ib[72] = 0;

    DELAY_NCLK(10);

    zqndr0[0] = (zqndr0[0] & ~(0x1<<28));
    zqndr0[1] = (zqndr0[1] & ~(0x1<<28));
    zqndr0[2] = (zqndr0[2] & ~(0x1<<28));
    zqndr0[3] = (zqndr0[3] & ~(0x1<<28));

    /*Register name in the RTL code is mentioned as ZQ0CR! It should be ZQ0DR.*/
    *(r32*)(HUP_CHIP_DDRPHY_ZQ0DR_ADDRESS) = zqndr0[0];
    *(r32*)(HUP_CHIP_DDRPHY_ZQ1DR_ADDRESS) = zqndr0[1];
    // *(r32*)(HUP_CHIP_DDRPHY_ZQ2DR) = zqndr0[2];
    // *(r32*)(HUP_CHIP_DDRPHY_ZQ3DR) = zqndr0[3];


    reg_val = *(r32 *)(HUP_CHIP_DDRPHY_PIR_ADDRESS);
    reg_val =  (reg_val & ~(0x1<< 0)) | (0x1<< 0);       // PIR.INIT
    reg_val =  (reg_val & ~(0x1<< 1)) | (0x0<< 1);       // ZQ calibration - PIR.ZCAL
    reg_val =  (reg_val & ~(0x1<<18)) | (0x1<<18);       // DRAM calibration - PIR.CTLDINIT
    *(r32 *)(HUP_CHIP_DDRPHY_PIR_ADDRESS) = reg_val;

    DELAY_PCLK(20);

    reg_val = *(r32 *)(HUP_CHIP_DDRPHY_PGSR0_ADDRESS);
    // poll general status register until phy initialization flag is set bit[10

    while((reg_val & 0x1) != 0x1)
    {
        DELAY_PCLK(20);
        reg_val = *(r32 *)(HUP_CHIP_DDRPHY_PGSR0_ADDRESS);
    }    
    
    // re-enable Controller Updates adfter reset
    // DFIUPDCFG.dfi_ctrlupd_en
    reg_val = *(r32 *)(HUP_CHIP_DDRCTL_UPCTL_DFIU_DFIUPDCFG_ADDRESS);
    reg_val =  (reg_val & ~(0x1<<0)) | (0x1<<0);  // DFIUPDCFG.dfi_ctrlupd_en
    *(r32 *)(HUP_CHIP_DDRCTL_UPCTL_DFIU_DFIUPDCFG_ADDRESS) = reg_val;

       
    // Set DFITCTRLUPDI time
    //read_data = ({$random} % 10) + 5;
    reg_val = 5;
    *(r32 *)(HUP_CHIP_DDRCTL_UPCTL_DFIU_DFITCTRLUPDI_ADDRESS) = reg_val;
    
    DELAY_NCLK(1);
    
    lpddr_move_state_t(UPCTL_STATE_ACCESS);
#endif
}    

