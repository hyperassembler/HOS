#include <mtc_train.h>
#include <mtc_init.h>
#include "conf_defs.h"
#include <math.h>
#include <assert.h>

int last_rd_val;
int auto_exp_right_adjust_wr[NUM_RANKS][NUM_SLICES],auto_exp_left_adjust_wr[NUM_RANKS][NUM_SLICES];
int auto_exp_trfail_wr,auto_calc_delay_wr,auto_adjusted_delay_wr;
int auto_exp_right_adjust[NUM_RANKS][NUM_SLICES],auto_exp_left_adjust[NUM_RANKS][NUM_SLICES];
int auto_exp_trfail,auto_calc_delay,auto_adjusted_delay ; // Read training delays 
int mtc_phy_update_fail_expected;
int right_move_delay_predict_wr;
int right_move_delay_predict_rd;
int force_fail[NUM_SLICES] = {0,0};

// --------------------------------
// get_wire_delay_reg_wr()
// --------------------------------
void get_wire_delay_reg_wr()
{
    int r, s;
    hw_status("%s: mtc_test: %s called\n", get_sim_time(), __func__);
    for (r=0; r<NUM_RANKS; r++)
    {
        for (s=0; s<NUM_SLICES; s++)
        {
            global_wire_delay_reg_wr[r][s] = sv_get_wire_delay_reg_wr(r, s);
            hw_status("%s: mtc_test: %s: Got global_wire_delay_reg_wr[%d][%d] = %d\n", 
                get_sim_time(), __func__, r, s, global_wire_delay_reg_wr[r][s]);
        }
    }
}

// --------------------------------
// get_wire_delay_reg_rd()
// --------------------------------
void get_wire_delay_reg_rd()
{
    int r, s;
    hw_status("%s: mtc_test: %s called\n", get_sim_time(), __func__);
    for (r=0; r<NUM_RANKS; r++)
    {
        for (s=0; s<NUM_SLICES; s++)
        {
            global_wire_delay_reg_rd[r][s] = sv_get_wire_delay_reg_rd(r, s);
            hw_status("%s: mtc_test: %s: Got global_wire_delay_reg_rd[%d][%d] = %d\n", 
                get_sim_time(), __func__, r, s, global_wire_delay_reg_rd[r][s]);
        }
    }
}

// --------------------------------
// mtc_common_init_setup()
// --------------------------------
void mtc_common_init_setup(const int active_ranks)
{
    int r, s, sr, er, rdata;
    hw_status("%s: mtc_test: %s called\n", get_sim_time(), __func__);
    if (active_ranks == 1)
    {
        sr = 0;
        er = 0;
    }
    else if (active_ranks == 2)
    {
        sr = 1;
        er = 1;
    }
    else if (active_ranks == 3)
    {
        sr = 0;
        er = 1;
    }

    /*
    for (s=0; s<NUM_SLICES; s++) {
       global_wdqdprd[s] = get_WDQPRD(s,0);
    }
    */

    for (r=sr; r<=er; r++)
    {
        for (s=0; s<NUM_SLICES; s++)
        {
            //WDQSL
            global_golden_wdqsl[r][s] = get_phy_dxngtr0_wdqsl(s, r);
            hw_status("%s: mtc_test: %s: global_golden_wdqsl[%d][%d] = %d\n", 
                get_sim_time(), __func__, r, s, global_golden_wdqsl[r][s]);
            global_wdqsl[r][s] = global_golden_wdqsl[r][s];
            hw_status("%s: mtc_test: %s: global_wdqsl[%d][%d] = %d\n", 
                get_sim_time(), __func__, r, s, global_wdqsl[r][s]);

            //WR Delay
            rdata = (get_phy_delay_write_reg(s, r)) & WDLY_MASK;
            global_golden_wr_delay_val[r][s] = global_golden_wdqsl[r][s] * global_wdqdprd[s] + rdata;
            hw_status("%s: mtc_test: %s: global_golden_wr_delay_val[%d][%d] = %d\n", 
                get_sim_time(), __func__, r, s, global_golden_wr_delay_val[r][s]);
        }
    }
}

// --------------------------------
// read_reg_until_stable()
// --------------------------------
uint32_t read_reg_until_stable(uint32_t *reg_addr) {
    uint32_t rdata, last_rdata, prevlast_rdata, rdata1;
    uint32_t i;
    //Doing many reads consecutively to make sure we don't get caught by transitions
    for (i=0; i<8; i++) {
        rdata = hw_read32(reg_addr);
        if (i > 2) {
            if ((last_rdata == rdata) && (last_rdata == prevlast_rdata)) break;
        }
        prevlast_rdata = last_rdata;
        last_rdata = rdata;
    }
    //Dummy Read a different register; this is required for the checking mechanism
    //to work correctly.
    rdata1 = hw_read32((uint32_t *)MCU_REGS_CRB_HIER_REGS_INTERRUPT_EN_ADDRESS);
    return rdata;
}

// --------------------------------
// intr_status_handler()
//   Actual checking of the register values is performed
//   by mcu_regs_monitor.
// --------------------------------
void intr_status_handler() {
    uint32_t rdata;
    //The following read should be checked by the mcu_regs_monitor against the tb_mcp_mru_if value.
    read_reg_until_stable((uint32_t *)MCU_REGS_CRB_HIER_REGS_INTERRUPT_STATUS_ADDRESS);
    //Now write the inverse of rdata (and make sure the status does not get cleared)
    hw_write32((uint32_t *)MCU_REGS_CRB_HIER_REGS_INTERRUPT_STATUS_ADDRESS, (~rdata));
    //The following read is to confirm that the previous write did NOT clear the status bits that were set.
    //The checking is done by mcu_regs_monitor
    read_reg_until_stable((uint32_t *)MCU_REGS_CRB_HIER_REGS_INTERRUPT_STATUS_ADDRESS);
    //Now write the rdata (and make sure the status does get cleared, provided there were no other events)
    hw_write32((uint32_t *)MCU_REGS_CRB_HIER_REGS_INTERRUPT_STATUS_ADDRESS, rdata);
    //The following read is to confirm that the previous write did clear the status bits that were set, 
    //provided there were no other events.
    //The checking is done by mcu_regs_monitor
    read_reg_until_stable((uint32_t *)MCU_REGS_CRB_HIER_REGS_INTERRUPT_STATUS_ADDRESS);
}

// --------------------------------
// fatal_status_handler()
//   Actual checking of the register values is performed
//   by mcu_regs_monitor.
// --------------------------------
void fatal_status_handler() {
    uint32_t rdata;
    //The following read should be checked by the mcu_regs_monitor against the tb_mcp_mru_if value.
    read_reg_until_stable((uint32_t *)MCU_REGS_CRB_HIER_REGS_FATAL_ERROR_STATUS_ADDRESS);
    //Now write the inverse of rdata (and make sure the status does not get cleared)
    hw_write32((uint32_t *)MCU_REGS_CRB_HIER_REGS_FATAL_ERROR_STATUS_ADDRESS, (~rdata));
    //The following read is to confirm that the previous write did NOT clear the status bits that were set.
    //The checking is done by mcu_regs_monitor
    read_reg_until_stable((uint32_t *)MCU_REGS_CRB_HIER_REGS_FATAL_ERROR_STATUS_ADDRESS);
    //Now write the rdata (and make sure the status does get cleared, provided there were no other events)
    hw_write32((uint32_t *)MCU_REGS_CRB_HIER_REGS_FATAL_ERROR_STATUS_ADDRESS, rdata);
    //The following read is to confirm that the previous write did clear the status bits that were set, 
    //provided there were no other events.
    //The checking is done by mcu_regs_monitor
    read_reg_until_stable((uint32_t *)MCU_REGS_CRB_HIER_REGS_FATAL_ERROR_STATUS_ADDRESS);
}

// --------------------------------
// non_fatal_status_handler()
//   Actual checking of the register values is performed
//   by mcu_regs_monitor.
// --------------------------------
void non_fatal_status_handler() {
    uint32_t rdata;
    //The following read should be checked by the mcu_regs_monitor against the tb_mcp_mru_if value.
    read_reg_until_stable((uint32_t *)MCU_REGS_CRB_HIER_REGS_NON_FATAL_ERROR_STATUS_ADDRESS);
    //Now write the inverse of rdata (and make sure the status does not get cleared)
    hw_write32((uint32_t *)MCU_REGS_CRB_HIER_REGS_NON_FATAL_ERROR_STATUS_ADDRESS, (~rdata));
    //The following read is to confirm that the previous write did NOT clear the status bits that were set.
    //The checking is done by mcu_regs_monitor
    read_reg_until_stable((uint32_t *)MCU_REGS_CRB_HIER_REGS_NON_FATAL_ERROR_STATUS_ADDRESS);
    //Now write the rdata (and make sure the status does get cleared, provided there were no other events)
    hw_write32((uint32_t *)MCU_REGS_CRB_HIER_REGS_NON_FATAL_ERROR_STATUS_ADDRESS, rdata);
    //The following read is to confirm that the previous write did clear the status bits that were set, 
    //provided there were no other events.
    //The checking is done by mcu_regs_monitor
    read_reg_until_stable((uint32_t *)MCU_REGS_CRB_HIER_REGS_NON_FATAL_ERROR_STATUS_ADDRESS);
}


// ----------------------
// get_freq_based_delay
// ----------------------
int get_freq_based_delay(int delay){

  float freq_based_delay;

  if ( global_dram_frequency == 400 ) {
     freq_based_delay = 0.25 * delay;
  } else if (  global_dram_frequency ==  1333 ) {
     freq_based_delay = 0.83 * delay;
  } else if ( global_dram_frequency == 1600 ) {
        freq_based_delay = delay;
  }
  return (int) freq_based_delay;

}

// ------------------------------------
// get_freq_based_golden_write_delay
// ------------------------------------
int get_freq_based_golden_write_delay(int rank, int slice){
  int freq_based_delay;

  if ( global_dram_frequency == 400 ) {
        freq_based_delay = global_golden_wr_delay_val[rank][slice];
  } else if (  global_dram_frequency ==  1333 ) {
        freq_based_delay = global_golden_wr_delay_val[rank][slice];
  } else if ( global_dram_frequency == 1600 ) {
#ifdef CONF_HAS___PHY_RTL
        freq_based_delay = global_golden_wr_delay_val[rank][slice];
#else
        freq_based_delay = GOLDEN_WR_DELAY_VAL_1600;
#endif
  }
  return freq_based_delay;
}

// ------------------------------------
// get_freq_based_golden_read_delay
// ------------------------------------
int get_freq_based_golden_read_delay(){
  int freq_based_delay;

  if ( global_dram_frequency == 400 ) {
        freq_based_delay = GOLDEN_RD_DELAY_VAL_400;
  } else if (  global_dram_frequency ==  1333 ) {
        freq_based_delay = GOLDEN_RD_DELAY_VAL_1333;
  } else if ( global_dram_frequency == 1600 ) {
        freq_based_delay = GOLDEN_RD_DELAY_VAL_1600;
  }
  return freq_based_delay;
}


// --------------------------------
// set_phy_delay_write_reg()
// --------------------------------
void set_phy_delay_write_reg(int dqs_no,int value,int rankid)
{
    int phy_reg_address;
    int mtc_reg_address;
    int write_value, rdval;
    int wdqsl;
    int tmp_active_ranks;

    tmp_active_ranks = 1 << rankid;

    // Get the value of WDQDPRD using algo in section 7.1.1
    wdqsl = get_phy_dxngtr0_wdqsl(dqs_no,rankid);
    hw_status("mtc_test: %s: Rank%d Slice%d, value = %d, wdqsl = %d\n", __func__, rankid, dqs_no, value, wdqsl);

    // ------- Program MTC shadow registers ------------------------
    if ( rankid == 0 ) 
    {
        switch(dqs_no) 
        {
            case 0 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0LCDLR1_RANK0_ADDRESS; break;
            case 1 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR1_RANK0_ADDRESS; break;
        }
    } 
    else 
    {
        switch(dqs_no) 
        {
            case 0 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0LCDLR1_RANK1_ADDRESS; break;
            case 1 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR1_RANK1_ADDRESS; break;
        }
    }

#ifdef CONF_HAS___PHY_RTL
    /*
    if ( value < 0 ) 
    {
        write_value = ((wdqsl-1) * global_wdqdprd[dqs_no]) + (value + global_wdqdprd[dqs_no]);
    } 
    else 
    {
        write_value = (wdqsl * global_wdqdprd[dqs_no]) + value;
    }
    */
    write_value = value;
#else 
    write_value = value;
#endif

    hw_write32((uint32_t *)  mtc_reg_address,DDC_REGS_MC_DX0LCDLR1_RANK0_VALUE_SET(write_value));
    hw_status("mtc_test: Programming MTC Addr = %xH , Data = %xH wdqsl =%xH global_wdqdprd=%xH \n",
        mtc_reg_address,write_value,wdqsl,global_wdqdprd[dqs_no]);

    if ( value < 0 ) 
    {
        write_value = PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_DGSL_SET(1) |
                      PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_WDQSL_SET(wdqsl-1) |
                      PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_WLSL_SET(1);
    } 
    else 
    {
        write_value = PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_DGSL_SET(1) |
                      PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_WDQSL_SET(wdqsl) |
                      PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_WLSL_SET(1);
    }
  
    hw_status("mtc_test: %s: Rank%d Slice%d: DX%dGTR0 set to 0x%x\n", __func__, rankid, dqs_no, dqs_no, write_value);
    set_mtc_dxngtr0(dqs_no,rankid,write_value);
    set_phy_dxngtr0(dqs_no,rankid,write_value);

    // ----- program the DXn delay registers ------------------

    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR1_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2LCDLR1_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3LCDLR1_ADDRESS; break;
    }

    /*
    if ( value < 0 ) {
       write_value = ((wdqsl-1) * global_wdqdprd[dqs_no]) + (value + global_wdqdprd[dqs_no]);
    } else {
       write_value = (wdqsl * global_wdqdprd[dqs_no]) + value;
    }
    */
    write_value = value;
    hw_phy_write32((uint32_t *)  phy_reg_address, PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_WDQD_SET(write_value));

    hw_status("mtc_test: %s: Rank%d Slice%d: PHY_PUB_DX%dLCDLR1 set to 0x%x\n", __func__, rankid, dqs_no, dqs_no, write_value);

    simulation_wait_ns(15);
    
    //hw_status("mtc_test: Programming Addr = %xH , Data = %xH wdqsl =%xH global_wdqdprd=%xH \n",
    //     phy_reg_address,PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_WDQD_SET(write_value),wdqsl,global_wdqdprd[dqs_no]);

    //Initialize the global dgsl and wdqsl variables correctly.
    //mtc_common_init_setup(tmp_active_ranks);
    global_wdqsl[rankid][dqs_no] = value % global_wdqdprd[dqs_no];
}

// --------------------------------
// copy_phy_rd_dly_regs_to_mc()
// --------------------------------
void copy_phy_rd_dly_regs_to_mc(int active_ranks){
    int phy_reg_address;
    int mtc_reg_address;
    int value;
    
    set_rank_id(0);

    value = hw_phy_read32((uint32_t *) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR3_ADDRESS);
    hw_write32((uint32_t *) MCU_REGS_DDC_REGS_MC_DX0LCDLR3_RANK0_ADDRESS, value);
    value = hw_phy_read32((uint32_t *) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR3_ADDRESS);
    hw_write32((uint32_t *) MCU_REGS_DDC_REGS_MC_DX1LCDLR3_RANK0_ADDRESS, value);

    value = hw_phy_read32((uint32_t *) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR4_ADDRESS);
    hw_write32((uint32_t *) MCU_REGS_DDC_REGS_MC_DX0LCDLR4_RANK0_ADDRESS, value);
    value = hw_phy_read32((uint32_t *) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR4_ADDRESS);
    hw_write32((uint32_t *) MCU_REGS_DDC_REGS_MC_DX1LCDLR4_RANK0_ADDRESS, value);

    value = hw_phy_read32((uint32_t *) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR2_ADDRESS);
    hw_write32((uint32_t *) MCU_REGS_DDC_REGS_MC_DX0LCDLR2_RANK0_ADDRESS, value);
    value = hw_phy_read32((uint32_t *) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR2_ADDRESS);
    hw_write32((uint32_t *) MCU_REGS_DDC_REGS_MC_DX1LCDLR2_RANK0_ADDRESS, value);

    // Copy GTR0 from  PHY to MC reg 
    value = get_phy_dxngtr0(0,0); //Slice0, Rank0
    set_mtc_dxngtr0(0,0,value); //Slice0, Rank0
    value = get_phy_dxngtr0(1,0); //Slice1, Rank0
    set_mtc_dxngtr0(1,0,value); //Slice1, Rank0

    // Copy GSR0 from PHY to MC reg
    value = get_phy_dxngsr0(0,0); //Slice0, Rank0
    set_mtc_dxngsr0(0,0,value); //Slice0, Rank0
    value = get_phy_dxngsr0(1,0); //Slice1, Rank0
    set_mtc_dxngsr0(1,0,value); //Slice1, Rank0

    if (active_ranks == 3)
    {
        set_rank_id(1);
    
        value = hw_phy_read32((uint32_t *) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR3_ADDRESS);
        hw_write32((uint32_t *) MCU_REGS_DDC_REGS_MC_DX0LCDLR3_RANK1_ADDRESS, value);
        value = hw_phy_read32((uint32_t *) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR3_ADDRESS);
        hw_write32((uint32_t *) MCU_REGS_DDC_REGS_MC_DX1LCDLR3_RANK1_ADDRESS, value);
    
        value = hw_phy_read32((uint32_t *) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR4_ADDRESS);
        hw_write32((uint32_t *) MCU_REGS_DDC_REGS_MC_DX0LCDLR4_RANK1_ADDRESS, value);
        value = hw_phy_read32((uint32_t *) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR4_ADDRESS);
        hw_write32((uint32_t *) MCU_REGS_DDC_REGS_MC_DX1LCDLR4_RANK1_ADDRESS, value);
    
        value = hw_phy_read32((uint32_t *) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR2_ADDRESS);
        hw_write32((uint32_t *) MCU_REGS_DDC_REGS_MC_DX0LCDLR2_RANK1_ADDRESS, value);
        value = hw_phy_read32((uint32_t *) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR2_ADDRESS);
        hw_write32((uint32_t *) MCU_REGS_DDC_REGS_MC_DX1LCDLR2_RANK1_ADDRESS, value);
    
        // Copy GTR0 from  PHY to MC reg 
        value = get_phy_dxngtr0(0,1); //Slice0, Rank1
        set_mtc_dxngtr0(0,1,value); //Slice0, Rank1
        value = get_phy_dxngtr0(1,1); //Slice1, Rank1
        set_mtc_dxngtr0(1,1,value); //Slice1, Rank1
    
        // Copy GSR0 from PHY to MC reg
        value = get_phy_dxngsr0(0,1); //Slice0, Rank1
        set_mtc_dxngsr0(0,1,value); //Slice0, Rank1
        value = get_phy_dxngsr0(1,1); //Slice1, Rank1
        set_mtc_dxngsr0(1,1,value); //Slice1, Rank1
    
        set_rank_id(0);
    }
}

// --------------------------------
// set_phy_delay_read_reg()
// --------------------------------
void set_phy_delay_read_reg(int dqs_no,int value,int rankid){
int phy_reg_address;
int mtc_reg_address;
int write_value;
    
    set_rank_id(rankid);

    if ( rankid == 0 ) {
      /*
      switch(dqs_no) {
        case 0 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0LCDLR2_RANK0_ADDRESS; break;
        case 1 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR2_RANK0_ADDRESS; break;
        case 2 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR2_RANK0_ADDRESS; break;
        case 3 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR2_RANK0_ADDRESS; break;

        case 4 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0LCDLR3_RANK0_ADDRESS; break;
        case 5 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR3_RANK0_ADDRESS; break;
        case 6 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR3_RANK0_ADDRESS; break;
        case 7 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR3_RANK0_ADDRESS; break;

        case 8 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0LCDLR4_RANK0_ADDRESS; break;
        case 9 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR4_RANK0_ADDRESS; break;
        case 10 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR4_RANK0_ADDRESS; break;
        case 11 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR4_RANK0_ADDRESS; break;
      } 
      */
      switch(dqs_no) {
        case 0 :  
            mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0LCDLR2_RANK0_ADDRESS; 
            hw_status("%s: mtc_test: %s: Programming Rank%d Slice0 LCDLR2 = 0x%x\n", get_sim_time(), __func__, rankid, value);
            break;
        case 1 ... 3 :  
            mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR2_RANK0_ADDRESS; 
            hw_status("%s: mtc_test: %s: Programming Rank%d Slice1 LCDLR2 = 0x%x\n", get_sim_time(), __func__, rankid, value);
            break;

        case 4 :  
            mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0LCDLR3_RANK0_ADDRESS; 
            hw_status("%s: mtc_test: %s: Programming Rank%d Slice0 LCDLR3 = 0x%x\n", get_sim_time(), __func__, rankid, value);
            break;
        case 5 ... 7 :  
            mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR3_RANK0_ADDRESS; 
            hw_status("%s: mtc_test: %s: Programming Rank%d Slice1 LCDLR3 = 0x%x\n", get_sim_time(), __func__, rankid, value);
            break;

        case 8 :  
            mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0LCDLR4_RANK0_ADDRESS; 
            hw_status("%s: mtc_test: %s: Programming Rank%d Slice0 LCDLR4 = 0x%x\n", get_sim_time(), __func__, rankid, value);
            break;
        case 9 ... 11 :  
            mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR4_RANK0_ADDRESS; 
            hw_status("%s: mtc_test: %s: Programming Rank%d Slice1 LCDLR4 = 0x%x\n", get_sim_time(), __func__, rankid, value);
            break;
      } 
    } else {
      switch(dqs_no) {
        case 0 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0LCDLR2_RANK1_ADDRESS; break;
        case 1 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR2_RANK1_ADDRESS; break;
        case 2 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR2_RANK1_ADDRESS; break;
        case 3 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR2_RANK1_ADDRESS; break;

        case 4 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0LCDLR3_RANK1_ADDRESS; break;
        case 5 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR3_RANK1_ADDRESS; break;
        case 6 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR3_RANK1_ADDRESS; break;
        case 7 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR3_RANK1_ADDRESS; break;

        case 8 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0LCDLR4_RANK1_ADDRESS; break;
        case 9 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR4_RANK1_ADDRESS; break;
        case 10 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR4_RANK1_ADDRESS; break;
        case 11 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR4_RANK1_ADDRESS; break;
      } 
    }

    hw_write32((uint32_t *)  mtc_reg_address, value);
    hw_status("%s: mtc_test: %s: Programming MTC Addr = %xH , Data = %xH\n", get_sim_time(), __func__, mtc_reg_address, value);
    
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR2_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR2_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2LCDLR2_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3LCDLR2_ADDRESS; break;
        case 4 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR3_ADDRESS; break;
        case 5 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR3_ADDRESS; break;
        case 6 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2LCDLR3_ADDRESS; break;
        case 7 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3LCDLR3_ADDRESS; break;
        case 8 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR4_ADDRESS; break;
        case 9 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR4_ADDRESS; break;
        case 10 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2LCDLR4_ADDRESS; break;
        case 11 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3LCDLR4_ADDRESS; break;
    }
    hw_phy_write32((uint32_t *)  phy_reg_address, value);
    simulation_wait_ns(15);

    hw_status("%s: mtc_test: %s: Programming Read delay Addr = %xH , Data = %xH\n", get_sim_time(), __func__, phy_reg_address, value);

    if ( dqs_no == 0 || dqs_no == 1) {
      // Copy GTR0 from  PHY to MC reg 
       write_value = get_phy_dxngtr0(dqs_no,rankid);
       set_mtc_dxngtr0(dqs_no,rankid,write_value);

      // Copy GSR0 from PHY to MC reg
       write_value = get_phy_dxngsr0(dqs_no,rankid);
       set_mtc_dxngsr0(dqs_no,rankid,write_value);
    }
}


// --------------------------------
// get_phy_delay_read_reg()
// --------------------------------
int get_phy_delay_read_reg(int dqs_no,int rankid){
int phy_reg_address;
    
    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR2_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR2_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2LCDLR2_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3LCDLR2_ADDRESS; break;
        case 4 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR3_ADDRESS; break;
        case 5 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR3_ADDRESS; break;
        case 6 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2LCDLR3_ADDRESS; break;
        case 7 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3LCDLR3_ADDRESS; break;
        case 8 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR4_ADDRESS; break;
        case 9 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR4_ADDRESS; break;
        case 10 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2LCDLR4_ADDRESS; break;
        case 11 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3LCDLR4_ADDRESS; break;
    }
    return hw_phy_read32((uint32_t *)  phy_reg_address);
}

// --------------------------------
// get_mtc_delay_read_reg()
// --------------------------------
int get_mtc_delay_read_reg(int dqs_no,int rankid){
int mtc_reg_address;
    
    set_rank_id(rankid);
    if ( rankid == 0 ) {
      switch(dqs_no) {
        case 0 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0LCDLR2_RANK0_ADDRESS; break;
        case 1 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR2_RANK0_ADDRESS; break;
        case 2 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR2_RANK0_ADDRESS; break;
        case 3 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR2_RANK0_ADDRESS; break;

        case 4 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0LCDLR3_RANK0_ADDRESS; break;
        case 5 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR3_RANK0_ADDRESS; break;
        case 6 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR3_RANK0_ADDRESS; break;
        case 7 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR3_RANK0_ADDRESS; break;

        case 8 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0LCDLR4_RANK0_ADDRESS; break;
        case 9 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR4_RANK0_ADDRESS; break;
        case 10 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR4_RANK0_ADDRESS; break;
        case 11 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR4_RANK0_ADDRESS; break;
      }
    } else {
      switch(dqs_no) {
        case 0 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0LCDLR2_RANK1_ADDRESS; break;
        case 1 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR2_RANK1_ADDRESS; break;
        case 2 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR2_RANK1_ADDRESS; break;
        case 3 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR2_RANK1_ADDRESS; break;

        case 4 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0LCDLR3_RANK1_ADDRESS; break;
        case 5 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR3_RANK1_ADDRESS; break;
        case 6 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR3_RANK1_ADDRESS; break;
        case 7 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR3_RANK1_ADDRESS; break;

        case 8 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0LCDLR4_RANK1_ADDRESS; break;
        case 9 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR4_RANK1_ADDRESS; break;
        case 10 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR4_RANK1_ADDRESS; break;
        case 11 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR4_RANK1_ADDRESS; break;
      }
    }
    return hw_read32((uint32_t *)  mtc_reg_address);
}

// -------------------------------
// in_golden_write_delay_range()
// -------------------------------
int in_golden_write_delay_range(int val, int rank, int slice) {
   int  status = 0;

   switch (global_dram_frequency) 
   {
       case 400 : 
           if ((val >= global_golden_wr_delay_val[rank][slice]-35) && (val <= global_golden_wr_delay_val[rank][slice]+35))
               status = 1;
           break;     
       case 1333 : 
           if ((val >= global_golden_wr_delay_val[rank][slice]-22) && (val  <= global_golden_wr_delay_val[rank][slice]+30))
               status = 1;
           break;     
       case 1600 : 
#ifdef CONF_HAS___PHY_RTL
           if ((val >= global_golden_wr_delay_val[rank][slice] -31) && (val  <= global_golden_wr_delay_val[rank][slice] +31))
#else 
           if (((val == GOLDEN_WR_DELAY_VAL_1600  - 1)   &&  (val < GOLDEN_WR_DELAY_VAL_1600 + 1))  || 
                (val == GOLDEN_WR_DELAY_VAL_1600))
#endif
               status = 1;
           break;
    }

    return status;
}

// -------------------------------
// in_golden_read_delay_range()
// -------------------------------
int in_golden_read_delay_range(int val) {
   unsigned int  modified_val; 
   int status = 0;

   hw_status("mtc_test: %s: val= %xH\n", __func__, val);
  // Cap negative values just like the RTL 
   if ( val < 0 )  {
      modified_val = 0;
   } else {
      modified_val = val;
   }
   hw_status("mtc_test: %s: modified_val (after cap negative to 0)= %xH\n", __func__, modified_val);

   switch (global_dram_frequency) {
       case 400 : 
               if ((modified_val >= GOLDEN_RD_DELAY_VAL_400 -35) && (modified_val  <= GOLDEN_RD_DELAY_VAL_400 +35))
                  status = 1;
              break;
       case 1333 : 
               if ((modified_val >= GOLDEN_RD_DELAY_VAL_1333 -22) && (modified_val  <= GOLDEN_RD_DELAY_VAL_1333 +30))
                  status = 1;
              break;
       case 1600 : 
#ifdef CONF_HAS___PHY_RTL
               hw_status("mtc_test: %s: upper limit = %xH, lower limit = %xH\n", __func__, (GOLDEN_RD_DELAY_VAL_1600 +31), (GOLDEN_RD_DELAY_VAL_1600 -30));
               if ((modified_val >= GOLDEN_RD_DELAY_VAL_1600 -30) && (modified_val  <= GOLDEN_RD_DELAY_VAL_1600 +31))
#else 
               hw_status("mtc_test: %s: upper limit = %xH, lower limit = %xH\n", __func__, (GOLDEN_RD_DELAY_VAL_1600 +2), (GOLDEN_RD_DELAY_VAL_1600 -1));
               if ((modified_val >= GOLDEN_RD_DELAY_VAL_1600 -1) && (modified_val  <= GOLDEN_RD_DELAY_VAL_1600 +2))
#endif 
                  status = 1;
              break;
   }
     if (status) {
         hw_status("mtc_test: %s: status = pass\n", __func__);
     } else {
         hw_status("mtc_test: %s: status = fail\n", __func__);
     }
     return status;
}

// --------------------------------------
// enable_auto_refresh_with_alignment()
// --------------------------------------
void enable_auto_refresh_with_alignment() {

 // program All Ranks-Auto Refresh Align Register
  if ( program_single_rank ) {
    set_crb_active_ranks(1);
    hw_write32((uint32_t *)  MCU_REGS_SCH_REGS_SCH_REF_ALIGN_CTRL_ADDRESS, 0x0);
   
  } else {
    hw_write32((uint32_t *)  MCU_REGS_SCH_REGS_SCH_REF_ALIGN_CTRL_ADDRESS, 0x3);
  }
  // Enable autorefresh
  set_autorefresh(0, 1480,global_dram_mode, global_dram_frequency); // 400 x sch_clk = 400 *2.5ns = 1000 ns = 1us refresh interva
}

// ---------------------------------
// program_single_rank_config
// ---------------------------------
void program_single_rank_config() {
  program_single_rank = 1;
}


// ----------------------------------
// check_mtc_training_status_bits()
// ----------------------------------
void check_mtc_training_status_bits( ) {
  uint32_t rd_val;

    /* -- deprecated register
    rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_NON_DFI_RD_WR_TR_FAIL_ADDRESS);

    if ( rd_val & 0x1  == 1) {
       hw_status("check_mtc_training_status_bits: MTC read training FAILED !!.\n");
    } else if ( rd_val & 0x2  == 2) {
       hw_status("check_mtc_training_status_bits: MTC write training FAILED !!.\n");
    } else if ( rd_val == 0 ) {
       hw_status("check_mtc_training_status_bits: MTC R/W training PASSED !!.\n");
    }
    */
}



// -----------------------------
// check_write_training_stats()
// -----------------------------
void check_write_training_stats
(
    int sample_cnt, 
    int initial_delay_val[NUM_SLICES], 
    int mov_window,
    int adj_step,
    int pass_criteria,
    int num_writes_reads,
    int sample_break,
    int no_of_ranks_polled,
    int rankid,
    int force_fail[NUM_SLICES]
) 
{
  uint32_t rd_val;
  int i;
  int right_mov_status, left_mov_status;
  int adjust;
  int auto_exp_training_test_pass = 1; // by default test should always pass
  int auto_exp_trans_right_pass_count;
  int auto_exp_trans_left_pass_count;
  int slice;
  int slice_reg_address;
  int compare_val;
  int wdqsl;
  int actual_wdqsl;
  int tmp_val;
  int possible_movement_count[NUM_SLICES] = {0, 0};
  int pass_criteria_flag = 0;

  //possible_movement_count=0;
  auto_exp_trfail_wr = 0;

  for (slice=0; slice < NUM_SLICES; slice++) 
  {
    auto_exp_training_test_pass = 1; // by default slice comparison always passes 
    hw_status("%s: mtc_test: %s: Rank%d Slice%d ------------ delay= %d wire_delay_reg=0x%x\n",
           get_sim_time(), __func__, rankid,slice,initial_delay_val[slice],global_wire_delay_reg_wr[rankid][slice]);

    //wdqsl = get_phy_dxngtr0_wdqsl(slice,rankid);
    wdqsl = global_wdqsl[rankid][slice];
    hw_status("%s: mtc_test: %s: Rank%d Slice%d: wdqsl = %xH\n",get_sim_time(), __func__, rankid, slice, wdqsl);

    /*
    if ( right_move_delay_predict_wr == 0 )  {
      auto_adjusted_delay_wr = initial_delay_val[slice] + global_wire_delay_reg_wr;
      auto_calc_delay_wr = initial_delay_val[slice] + global_wire_delay_reg_wr;
    } else {
      auto_adjusted_delay_wr = initial_delay_val[slice] - global_wire_delay_reg_wr;
      auto_calc_delay_wr = initial_delay_val[slice] - global_wire_delay_reg_wr;
    }
    */
    // Positive number for global_write_delay_reg_wr means DQ is
    // delayed; thus the movement will be to the left;
    // Negative number for global_write_delay_reg_wr means DQS is
    // delayed; thus the movement will be to the right.
    auto_adjusted_delay_wr = initial_delay_val[slice] + global_wire_delay_reg_wr[rankid][slice];
    auto_calc_delay_wr = initial_delay_val[slice] + global_wire_delay_reg_wr[rankid][slice];

    hw_status("%s: mtc_test: %s: Rank%d Slice%d: auto_adjusted_delay_wr = %xH\n", get_sim_time(), __func__, rankid, slice, auto_adjusted_delay_wr);
    hw_status("%s: mtc_test: %s: Rank%d Slice%d: auto_calc_delay_wr = %xH\n", get_sim_time(), __func__, rankid, slice, auto_calc_delay_wr);
    
#ifdef CONF_HAS___PHY_RTL
         if ( auto_adjusted_delay_wr < 0 && (global_wire_delay_reg_wr[rankid][slice] == 0))  {
              hw_status("%s: mtc_test: %s: Rank%d Slice%d: Detected negative initial delay = %ddec\n",get_sim_time(), __func__, rankid, slice, auto_adjusted_delay_wr);
              auto_adjusted_delay_wr = auto_adjusted_delay_wr +  global_wdqdprd[slice];
         }
#endif

    auto_exp_trans_right_pass_count = 0;
    auto_exp_trans_left_pass_count = 0;

    for (i=0; i < sample_cnt * (sample_break+1) ; i++ ) 
    {
 
      hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d\n", get_sim_time(), __func__, rankid, slice, i);
      if ( (sample_break && (i%2 == 0)) || !sample_break ) {
        if (!force_fail[slice])
        {
          adjust = auto_calc_delay_wr + mov_window;
          if ( in_golden_write_delay_range(adjust, rankid, slice)) {  // reads will fail on right mov 
             right_mov_status = 1; // pass
             hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: right mov PASS\n", get_sim_time(), __func__, rankid, slice, i);
             hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: right adjust 0x%x\n", get_sim_time(), __func__, rankid, slice, i,adjust);
             auto_exp_trans_right_pass_count += num_writes_reads;
             hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: auto_exp_trans_right_pass_count = 0x%x\n", get_sim_time(), __func__, rankid, slice, i, auto_exp_trans_right_pass_count);
          } else {
             right_mov_status = 0; // fail 
             hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: right mov FAIL\n", get_sim_time(), __func__, rankid, slice, i);
             hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: right adjust 0x%x\n", get_sim_time(), __func__, rankid, slice, i, adjust);
          }
        }
        else
        {
          right_mov_status = 0; // fail 
          hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: right mov FAIL due to force_fail\n", get_sim_time(), __func__, rankid, slice, i);
        }
      }

      if ( (sample_break && (i%2 == 1)) || !sample_break ) {
        if (!force_fail[slice])
        {
          adjust = auto_calc_delay_wr - mov_window;
          if ( in_golden_write_delay_range(adjust, rankid, slice)) {  // reads will fail on left mov 
             left_mov_status = 1; // pass
             auto_exp_trans_left_pass_count += num_writes_reads;
             hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: left mov PASS\n", get_sim_time(), __func__, rankid, slice, i);
             hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: left adjust %xH\n", get_sim_time(), __func__, rankid, slice, i,adjust);
             hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: auto_exp_trans_left_pass_count = %xH\n", get_sim_time(), __func__, rankid, slice, i,auto_exp_trans_left_pass_count);
          } else {
              left_mov_status = 0;
             hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: left mov FAIL\n", get_sim_time(), __func__, rankid, slice, i);
             hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: left adjust %xH\n", get_sim_time(), __func__, rankid, slice, i,adjust);
          }
        }
        else
        {
          left_mov_status = 0; // fail 
          hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: left mov FAIL due to force_fail\n", get_sim_time(), __func__, rankid, slice, i);
        }
      }

      if ( !sample_break) 
      {
        if (!force_fail[slice])
        {
          if ( right_mov_status && !left_mov_status && (i == (sample_cnt - 1))) {
             auto_adjusted_delay_wr += adj_step;
             auto_adjusted_delay_wr &= WDLY_MASK;
             auto_exp_right_adjust_wr[rankid][slice]++;
             possible_movement_count[slice]++;
          } 
 
          if ( !right_mov_status && left_mov_status && (i == (sample_cnt - 1))) {
             auto_adjusted_delay_wr -= adj_step;
             auto_adjusted_delay_wr &= WDLY_MASK;
             auto_exp_left_adjust_wr[rankid][slice]++;
             possible_movement_count[slice]++;
          } 

          if ( !right_mov_status && !left_mov_status ) 
          {
             //if (global_wire_delay_reg_wr > 0)
             if (global_wire_delay_reg_wr[rankid][slice] != 0)
             {
                 auto_adjusted_delay_wr = initial_delay_val[slice];
             }
             auto_adjusted_delay_wr &= WDLY_MASK;
             //auto_exp_right_adjust = 0;
             //auto_exp_left_adjust = 0;

             if ( i == (sample_cnt - 1)) {
                 auto_exp_trfail_wr++;
                 auto_exp_training_test_pass = 0;
             }
          }

          if ( right_mov_status && left_mov_status) {
             //Don't change the adjusted delay as there is no movement.
             //if (global_wire_delay_reg_wr > 0)
             if (global_wire_delay_reg_wr[rankid][slice] != 0)
             {
                 auto_adjusted_delay_wr = initial_delay_val[slice];
             }
             auto_adjusted_delay_wr &= WDLY_MASK;
             possible_movement_count[slice]=0;
             auto_exp_trans_left_pass_count += num_writes_reads;
             auto_exp_trans_right_pass_count += num_writes_reads;
             //auto_exp_trfail_wr = 0;
          }
        } // if (!force_fail[slice])
        else
        {
          //Don't change the adjusted delay as there is no movement.
          //if (global_wire_delay_reg_wr > 0)
          if (global_wire_delay_reg_wr[rankid][slice] != 0)
          {
              auto_adjusted_delay_wr = initial_delay_val[slice];
          }
          auto_adjusted_delay_wr &= WDLY_MASK;
          possible_movement_count[slice]=0;
          //auto_exp_trans_left_pass_count += num_writes_reads;
          //auto_exp_trans_right_pass_count += num_writes_reads;
          auto_exp_training_test_pass = 0;
          auto_exp_trfail_wr++;
        }
      } // if (!sample_break)

     if ( sample_break ) {
         if ( i % 2 == 0 ) {
             if (!force_fail[slice])
             {
                 if ( right_mov_status && (i == (sample_cnt*2 - 2))) {
                   auto_adjusted_delay_wr += adj_step;
                   auto_adjusted_delay_wr &= 0x1ff;
                   auto_exp_right_adjust_wr[rankid][slice]++;
                   possible_movement_count[slice]++;
                 }
             }
         } else {
             if (!force_fail[slice])
             {
                 if ( left_mov_status && (i == (sample_cnt*2 - 1))) {
                   auto_adjusted_delay_wr -= adj_step;
                   auto_adjusted_delay_wr &= 0x1ff;
                   auto_exp_left_adjust_wr[rankid][slice]++;
                   possible_movement_count[slice]++;
                 }

                 if ( !right_mov_status && !left_mov_status ) {
                    if ( i == (sample_cnt*2 - 1)) {
                        auto_exp_trfail_wr++;
                        auto_exp_training_test_pass = 0;
                    }
                 }

                 if (right_mov_status && left_mov_status ) {
                      //auto_exp_trfail_wr = 0;
                      //Don't change the adjusted delay as there is no movement.
                      //if (global_wire_delay_reg_wr > 0)
                      if (global_wire_delay_reg_wr[rankid][slice] != 0)
                      {
                          auto_adjusted_delay_wr = initial_delay_val[slice];
                      }
                      auto_adjusted_delay_wr &= WDLY_MASK;
                      possible_movement_count[slice]=0;
                      auto_exp_trans_left_pass_count += num_writes_reads;
                      auto_exp_trans_right_pass_count += num_writes_reads;
                      //auto_exp_trfail_wr = 0;
                 }
             }
             else
             {
                 if ( i == (sample_cnt*2 - 1)) {
                     auto_exp_trfail_wr++;
                     auto_exp_training_test_pass = 0;
                 }
                 //Don't change the adjusted delay as there is no movement.
                 //if (global_wire_delay_reg_wr > 0)
                 if (global_wire_delay_reg_wr[rankid][slice] != 0)
                 {
                     auto_adjusted_delay_wr = initial_delay_val[slice];
                 }
                 auto_adjusted_delay_wr &= WDLY_MASK;
                 possible_movement_count[slice]=0;
                 //auto_exp_trans_left_pass_count += num_writes_reads;
                 //auto_exp_trans_right_pass_count += num_writes_reads;
             } //(force_fail[slice])
         }
     }

      hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: checker auto_exp_right_adjust val = %xH\n", get_sim_time(), __func__, rankid, slice, i, auto_exp_right_adjust_wr[rankid][slice]);
      hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: checker auto_exp_left_adjust = %xH\n", get_sim_time(), __func__, rankid, slice, i, auto_exp_left_adjust_wr[rankid][slice]);
      hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: checker adjusted delay val = %xH\n", get_sim_time(), __func__, rankid, slice, i, auto_adjusted_delay_wr);
      hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: checker auto_exp_trfail val = %xH\n", get_sim_time(), __func__, rankid, slice, i, auto_exp_trfail_wr);
      hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: checker possible_movement_count val = %xH\n", get_sim_time(), __func__, rankid, slice, i, possible_movement_count[slice]);
      hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: checker auto_exp_training_test_pass val = %xH\n", get_sim_time(), __func__, rankid, slice, i, auto_exp_training_test_pass);
      hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: checker auto_exp_trans_right_pass_count val = %xH\n", get_sim_time(), __func__, rankid, slice, i, auto_exp_trans_right_pass_count);
      hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: checker auto_exp_trans_left_pass_count val = %xH\n", get_sim_time(), __func__, rankid, slice, i, auto_exp_trans_left_pass_count);
      
    } //for (sample_cnt)

    // ----- Training status expected calculation  ----
    if ((auto_exp_trans_right_pass_count < pass_criteria) && ( auto_exp_trans_left_pass_count < pass_criteria)) {
        if ( pass_criteria_flag == 0 ) {
         auto_exp_training_test_pass = 0;
        }
    } else {
         pass_criteria_flag = 1;
    }

    if ( pass_criteria == 0 ) {
         auto_exp_right_adjust_wr[rankid][slice] = 0;
         auto_exp_left_adjust_wr[rankid][slice] = 0;
         auto_exp_trfail_wr = 0;
    }

    if ( slice == 0 ) {
      // stats 
      slice_reg_address = rankid ? MCU_REGS_DDC_REGS_WR_RIGHT_DQS_ADJUSTS_SLICE0_RANK1_ADDRESS 
                               : MCU_REGS_DDC_REGS_WR_RIGHT_DQS_ADJUSTS_SLICE0_RANK0_ADDRESS;

      rd_val = hw_read32((uint32_t *) slice_reg_address);
      hw_status("%s: mtc_test: %s: Rank%d Slice%d: WR_RIGHT_DQS_ADJUSTS_SLICE0 regval = %xH\n", get_sim_time(), __func__, rankid, slice, rd_val);
      if ( rd_val != auto_exp_right_adjust_wr[rankid][slice] ) {
         hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR : WR_RIGHT_DQS_ADJUSTS_SLICE0 comparison failed!!. exp =%xH \n", get_sim_time(), __func__, rankid, slice, auto_exp_right_adjust_wr[rankid][slice]);
      } else {
         hw_status("%s: mtc_test: %s: Rank%d Slice%d: WR_RIGHT_DQS_ADJUSTS_SLICE0 comparison PASSED!!.\n", get_sim_time(), __func__, rankid, slice);
      }

      slice_reg_address = rankid ? MCU_REGS_DDC_REGS_WR_LEFT_DQS_ADJUSTS_SLICE0_RANK1_ADDRESS 
                                 : MCU_REGS_DDC_REGS_WR_LEFT_DQS_ADJUSTS_SLICE0_RANK0_ADDRESS;

      rd_val = hw_read32((uint32_t *) slice_reg_address);
      hw_status("%s: mtc_test: %s: Rank%d Slice%d: WR_LEFT_DQS_ADJUSTS_SLICE0 regval = %xH\n", get_sim_time(), __func__, rankid, slice, rd_val);
      if ( rd_val != auto_exp_left_adjust_wr[rankid][slice] ) {
         hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR : WR_LEFT_DQS_ADJUSTS_SLICE0 comparison failed!!. exp=%xH\n", get_sim_time(), __func__, rankid, slice, auto_exp_left_adjust_wr[rankid][slice]);
      } else {
         hw_status("%s: mtc_test: %s: Rank%d Slice%d: WR_LEFT_DQS_ADJUSTS_SLICE0 comparison PASSED!!.\n", get_sim_time(), __func__, rankid, slice);
      }
    }

    if ( slice == 1 ) {

     slice_reg_address = rankid ? MCU_REGS_DDC_REGS_WR_RIGHT_DQS_ADJUSTS_SLICE1_RANK1_ADDRESS 
                               : MCU_REGS_DDC_REGS_WR_RIGHT_DQS_ADJUSTS_SLICE1_RANK0_ADDRESS;
 

    rd_val = hw_read32((uint32_t *) slice_reg_address);
    hw_status("%s: mtc_test: %s: Rank%d Slice%d: WR_RIGHT_DQS_ADJUSTS_SLICE1 regval = %xH\n", get_sim_time(), __func__, rankid, slice, rd_val);
    if ( rd_val != auto_exp_right_adjust_wr[rankid][slice] ) {
       hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR : WR_RIGHT_DQS_ADJUSTS_SLICE1 comparison failed!!. exp=%xH\n", get_sim_time(), __func__, rankid, slice,auto_exp_right_adjust_wr[rankid][slice]);
    } else {
       hw_status("%s: mtc_test: %s: Rank%d Slice%d: WR_RIGHT_DQS_ADJUSTS_SLICE1 comparison PASSED!!.\n", get_sim_time(), __func__, rankid, slice);
    }


    slice_reg_address = rankid ? MCU_REGS_DDC_REGS_WR_LEFT_DQS_ADJUSTS_SLICE1_RANK1_ADDRESS 
                              : MCU_REGS_DDC_REGS_WR_LEFT_DQS_ADJUSTS_SLICE1_RANK0_ADDRESS;


    rd_val = hw_read32((uint32_t *) slice_reg_address);
    hw_status("%s: mtc_test: %s: Rank%d Slice%d: WR_LEFT_DQS_ADJUSTS_SLICE1 regval = %xH\n", get_sim_time(), __func__, rankid, slice, rd_val);
    if ( rd_val != auto_exp_left_adjust_wr[rankid][slice] ) {
       hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR : WR_LEFT_DQS_ADJUSTS_SLICE1 comparison failed!!. exp = %xH\n", get_sim_time(), __func__, rankid, slice,auto_exp_left_adjust_wr[rankid][slice]);
    } else {
       hw_status("%s: mtc_test: %s: Rank%d Slice%d: WR_LEFT_DQS_ADJUSTS_SLICE1 comparison PASSED!!.\n", get_sim_time(), __func__, rankid, slice);
    }
    }

    rd_val = (get_phy_delay_write_reg(slice,rankid)) & WDLY_MASK;
    hw_status("%s: mtc_test: %s: Rank%d Slice%d: Delay[%d] = %xH\n", get_sim_time(), __func__, rankid, slice,slice,rd_val);
    hw_status("%s: mtc_test: %s: Rank%d Slice%d: DelayN[%d] = %xH\n", get_sim_time(), __func__, rankid, slice,slice,rd_val);

    if ( possible_movement_count[slice] == 0 ) {
      compare_val = auto_adjusted_delay_wr;
      compare_val &= WDLY_MASK;
    } else {
      /*
      if ( right_move_delay_predict_wr == 0 ) {
        compare_val = auto_adjusted_delay_wr - global_wire_delay_reg_wr;
        compare_val &= WDLY_MASK;
      } else {
        compare_val = auto_adjusted_delay_wr + global_wire_delay_reg_wr;      
        compare_val &= WDLY_MASK;
      }
      */
      compare_val = auto_adjusted_delay_wr - global_wire_delay_reg_wr[rankid][slice];
      //compare_val &= WDLY_MASK;
    }

    //sugopala: fixed this to take care of the wdqsl value
#ifdef CONF_HAS___PHY_RTL
    if (0)
    {
    if (compare_val < 0)
    {
        wdqsl--;
        if (wdqsl < 0)
        {
            hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR: WDQSL decremented below 0\n", get_sim_time(), __func__, rankid, slice);
        }
        else
        {
            hw_status("%s: mtc_test: %s: Rank%d Slice%d: WDQSL decremented to %0d\n", get_sim_time(), __func__, rankid, slice, wdqsl);
        }
        compare_val +=  global_wdqdprd[slice];
    }
    else if (compare_val > global_wdqdprd[slice])
    {
        wdqsl++;
        if (wdqsl > 7)
        {
            hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR: WDQSL incremented above 7\n", get_sim_time(), __func__, rankid, slice);
        }
        else
        {
            hw_status("%s: mtc_test: %s: Rank%d Slice%d: WDQSL incremented to %0d\n", get_sim_time(), __func__, rankid, slice, wdqsl);
        }
        compare_val -=  global_wdqdprd[slice];
    }
    }
    wdqsl = compare_val/global_wdqdprd[slice];
    global_wdqsl[rankid][slice] = wdqsl;
    //global_final_adj_values[rankid][slice] = compare_val + ((wdqsl-GOLDEN_WDQSL_VAL_1600)*global_wdqdprd[slice]);
    //global_final_adj_values[rankid][slice] = compare_val + ((wdqsl-global_golden_wdqsl[rankid][slice])*global_wdqdprd[slice]);
    global_final_adj_values[rankid][slice] = compare_val;
    //Now update the wdqsl with expected value after training
    //wdqsl = (global_final_adj_values[rankid][slice]/global_wdqdprd[slice]) + (global_golden_wdqsl[rankid][slice]);
    //global_wdqsl[rankid][slice] = wdqsl;
    //Verify the WDQSL got updated correctly.
    actual_wdqsl = get_phy_dxngtr0_wdqsl(slice, rankid);
    if (actual_wdqsl == wdqsl)
    {
       hw_status("%s: mtc_test: %s: Rank%d Slice%d: WDQSL matched: %0d\n", get_sim_time(), __func__, rankid, slice, actual_wdqsl);
    }
    else
    {
       hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR: WDQSL MISMATCH: Exp %0d != %0d Actual\n", get_sim_time(), __func__, rankid, slice, wdqsl, actual_wdqsl);
    }
    compare_val = compare_val % global_wdqdprd[slice];
#else 
    global_final_adj_values[rankid][slice] = compare_val;
#endif

    if (compare_val  == (get_phy_delay_write_reg(slice,rankid))) {
       hw_status("%s: mtc_test: %s: Rank%d Slice%d: adjusted delay comparison PASSED!!.\n", get_sim_time(), __func__, rankid, slice);
    } else {
       hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR : adjusted delay comparison failed!! exp =%xH rd_val=%xH .\n", get_sim_time(), __func__, rankid, slice,compare_val,rd_val);
    }

    // --- check MC delay registers ----
    rd_val = get_mtc_delay_write_reg(slice,rankid);

//sugopala## #ifdef CONF_HAS___PHY_RTL
//sugopala##     if ( possible_movement_count[slice] == 0) {
//sugopala##         compare_val = auto_adjusted_delay_wr + (wdqsl * global_wdqdprd[slice]);
//sugopala##         compare_val &= WDLY_MASK;
//sugopala##     }  else {
//sugopala##         /*
//sugopala##         if ( right_move_delay_predict_wr == 0 ) {
//sugopala##           compare_val = auto_adjusted_delay_wr + (wdqsl * global_wdqdprd[slice]) - global_wire_delay_reg_wr;
//sugopala##           compare_val &= WDLY_MASK;
//sugopala##         } else {
//sugopala##           compare_val = auto_adjusted_delay_wr + (wdqsl * global_wdqdprd[slice]) + global_wire_delay_reg_wr;
//sugopala##           compare_val &= WDLY_MASK;
//sugopala##         }
//sugopala##         */
//sugopala##         compare_val = auto_adjusted_delay_wr + (wdqsl * global_wdqdprd[slice]) - global_wire_delay_reg_wr[rankid][slice];
//sugopala##         compare_val &= WDLY_MASK;
//sugopala##     }
//sugopala## #else 
//sugopala##      compare_val = auto_adjusted_delay_wr;
//sugopala## #endif 

#ifdef CONF_HAS___PHY_RTL
    compare_val += (wdqsl * global_wdqdprd[slice]);
    compare_val &= WDLY_MASK;
#endif 

    if (compare_val == rd_val) {
       hw_status("%s: mtc_test: %s: Rank%d Slice%d: mtc reg adjusted delay comparison PASSED!!.\n", get_sim_time(), __func__, rankid, slice);
    } else {
       hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR : mtc reg adjusted delay comparison failed exp =%xH. rd_val=%xH \n", get_sim_time(), __func__, rankid, slice,compare_val,rd_val);
    }

  } //for (slice) 


    //if ( possible_movement_count > 0) {
    //     auto_exp_trfail_wr = 0;
    //} 
    hw_status("%s: mtc_test: %s: Rank%d checker auto_exp_trfail val = %xH\n", get_sim_time(), __func__, rankid, auto_exp_trfail_wr);
    //hw_status("%s: mtc_test: %s: Rank%d checker possible_movement_count val = %xH\n", get_sim_time(), __func__, rankid, possible_movement_count);
    hw_status("%s: mtc_test: %s: Rank%d checker auto_exp_training_test_pass val = %xH\n", get_sim_time(), __func__, rankid, auto_exp_training_test_pass);

    //  ---- Final pass , fail status checking -----
    hw_status("%s: mtc_test: %s: Rank%d last_rd_val = 0x%x\n", get_sim_time(), __func__, rankid, last_rd_val);
    if ( auto_exp_training_test_pass && (auto_exp_trfail_wr == 0) ) 
    {
       if (DDC_REGS_TRAINING_STATUS_WR_FAIL_GET(last_rd_val)== 0) {
          hw_status("%s: mtc_test: %s: Rank%d MTC WR TRAINING PASSED !! regval =%xH\n", get_sim_time(), __func__, rankid, last_rd_val);
       } else {
          hw_status("%s: mtc_test: %s: Rank%d ERROR : MTC wr_fail status MISMATCH !! actual 0x%x != 0x0 expected\n", get_sim_time(), __func__, rankid, DDC_REGS_TRAINING_STATUS_WR_FAIL_GET(last_rd_val));
       }
    } 
    else if ((auto_exp_training_test_pass == 0) && (auto_exp_trfail_wr != 0))
    { 
       if (DDC_REGS_TRAINING_STATUS_WR_FAIL_GET(last_rd_val)) 
       {
          hw_status("%s: mtc_test: %s: Rank%d MTC WR TRAINING TEST FAIL STATUS AS EXPECTED  !! regval =%xH\n", get_sim_time(), __func__, rankid, last_rd_val);
       } else {
          hw_status("%s: mtc_test: %s: Rank%d ERROR : MTC wr_fail status MISMATCH !! actual 0x%x != 0x1 expected\n", get_sim_time(), __func__, rankid, DDC_REGS_TRAINING_STATUS_WR_FAIL_GET(last_rd_val));
       }
    }
    
    // ---- Debug only pass cnt registers 
    //rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_RD_WR_TR_RIGHT_PASS_CNT_ADDRESS);
    //hw_status("%s: WR_TR_RIGHT_PASS_CNT regval = %xH\n", rd_val);
    //rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_RD_WR_TR_LEFT_PASS_CNT_ADDRESS);
    //hw_status("%s: WR_TR_LEFT_PASS_CNT regval = %xH\n", rd_val);

}

// -----------------------------
// check_read_training_stats()
// -----------------------------
void check_read_training_stats
(
    int sample_cnt, 
    int initial_delay_val[NUM_SLICES], 
    int mov_window,
    int adj_step,
    int pass_criteria,
    int num_writes_reads,
    int sample_break,
    int no_of_ranks_polled,
    int rankid,
    int force_fail[NUM_SLICES]
) 
{
    uint32_t rd_val;
    int i,j;
    int right_mov_status, left_mov_status;
    int adjust;
    int auto_exp_training_test_pass = 1; // by default test should always pass
    int auto_exp_trans_right_pass_count;
    int auto_exp_trans_left_pass_count;
    int slice;
    int rd_value;
    int slice_reg_address;
    int possible_movement_count;
    int pass_criteria_flag;
    int compare_val;

    possible_movement_count=0;
    pass_criteria_flag=0;

    for (slice=0; slice < NUM_SLICES; slice++) 
    {
        auto_exp_training_test_pass = 1; // slice comparison always passes by default 
        hw_status("%s: mtc_test: %s: Rank%d Slice%d ------------delay= %d wire_delay=0x%x\n",
            get_sim_time(), __func__, rankid, slice, initial_delay_val[slice], global_wire_delay_reg_rd[rankid][slice]);

        hw_status("%s: mtc_test: %s: Rank%d Slice%d right_move_delay_predict_rd = %d\n", 
            get_sim_time(), __func__, rankid, slice, right_move_delay_predict_rd);

        /*
        if ( right_move_delay_predict_rd == 1 )  
        {
            auto_adjusted_delay= initial_delay_val[slice] - global_wire_delay_reg_rd;
            auto_calc_delay = initial_delay_val[slice] - global_wire_delay_reg_rd;
            adjust_gate_delay[rankid][slice] = initial_delay_val[slice] - global_wire_delay_reg_rd; 
        } 
        else 
        {
            auto_adjusted_delay= initial_delay_val[slice] + global_wire_delay_reg_rd;
            auto_calc_delay= initial_delay_val[slice] + global_wire_delay_reg_rd;
            adjust_gate_delay[rankid][slice] = initial_delay_val[slice] + global_wire_delay_reg_rd; 
        }
        */
        auto_adjusted_delay= initial_delay_val[slice] + global_wire_delay_reg_rd[rankid][slice];
        auto_calc_delay= initial_delay_val[slice] + global_wire_delay_reg_rd[rankid][slice];
        adjust_gate_delay[rankid][slice] = initial_delay_val[slice] + global_wire_delay_reg_rd[rankid][slice]; 

        hw_status("%s: mtc_test: %s: Rank%d Slice%d auto_adjusted_delay = %d\n", get_sim_time(), __func__, rankid, slice, auto_adjusted_delay);
        hw_status("%s: mtc_test: %s: Rank%d Slice%d auto_calc_delay = %d\n", get_sim_time(), __func__, rankid, slice, auto_calc_delay);
        hw_status("%s: mtc_test: %s: Rank%d Slice%d adjust_gate_delay = %d\n", get_sim_time(), __func__, rankid, slice, adjust_gate_delay[rankid][slice]);

        auto_exp_trans_right_pass_count = 0;
        auto_exp_trans_left_pass_count = 0;

        for (i=0; i < sample_cnt * (sample_break+1) ; i++ ) 
        {
            if ( (sample_break && (i%2 == 0)) || !sample_break ) 
            {
                if (!force_fail[slice])
                {
                    //hw_status("%s: mtc_test: %s: Rank%d Slice%d: checking right MOV sample # %d\n", get_sim_time(), __func__, rankid, slice, i);
                    //adjust = ((int) (auto_calc_delay + mov_window))  & 0x1ff;
                    adjust = auto_calc_delay + mov_window;
                    hw_status("%s: mtc_test: %s: Rank%d Slice%d Sample%d: adjust = 0x%x\n", get_sim_time(), __func__, rankid, slice, i, adjust);
                    if (in_golden_read_delay_range(adjust)) {  // reads will fail on right mov 
                        right_mov_status = 1; // pass
                        hw_status("%s: mtc_test: %s: Rank%d Slice%d Sample%d: checker right mov PASS\n", get_sim_time(), __func__, rankid, slice, i);
                        hw_status("%s: mtc_test: %s: Rank%d Slice%d Sample%d: checker adjust = %xH\n", get_sim_time(), __func__, rankid, slice, i, adjust);
                        auto_exp_trans_right_pass_count += num_writes_reads;
                        hw_status("%s: mtc_test: %s: Rank%d Slice%d Sample%d: checker auto_exp_trans_right_pass_count = %xH\n", get_sim_time(), __func__, rankid, slice, i, auto_exp_trans_right_pass_count);
                    } else {
                        right_mov_status = 0; // fail 
                        hw_status("%s: mtc_test: %s: Rank%d Slice%d Sample%d: checker right mov FAIL\n", get_sim_time(), __func__, rankid, slice, i);
                        hw_status("%s: mtc_test: %s: Rank%d Slice%d Sample%d: checker adjust = %xH\n", get_sim_time(), __func__, rankid, slice, i, adjust);
                    }
                } //(!force_fail[slice])
                else
                {
                    right_mov_status = 0; // fail 
                    hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: right mov FAIL due to force_fail\n", get_sim_time(), __func__, rankid, slice, i);
                }
            }

            if ( (sample_break && (i%2 == 1)) || !sample_break ) 
            {
                if (!force_fail[slice])
                {
                    //hw_status("%s: mtc_test: check_read_training_stats: checking left MOV sample # %d\n",i);
                    //adjust = ((int) (auto_calc_delay - mov_window))  & 0x1ff;
                    adjust = auto_calc_delay - mov_window; 
                    hw_status("%s: mtc_test: %s: Rank%d Slice%d Sample%d: adjust = 0x%x\n", get_sim_time(), __func__, rankid, slice, i, adjust);
                    if ( in_golden_read_delay_range(adjust)) 
                    {  // reads will fail on left mov 
                        left_mov_status = 1; // pass
                        auto_exp_trans_left_pass_count += num_writes_reads;
                        hw_status("%s: mtc_test: %s: Rank%d Slice%d Sample%d: checker left mov PASS\n", get_sim_time(), __func__, rankid, slice, i);
                        hw_status("%s: mtc_test: %s: Rank%d Slice%d Sample%d: checker adjust = %xH\n", get_sim_time(), __func__, rankid, slice, i, adjust);
                    } 
                    else 
                    {
                        left_mov_status = 0;
                        hw_status("%s: mtc_test: %s: Rank%d Slice%d Sample%d: checker left mov FAIL\n", get_sim_time(), __func__, rankid, slice, i);
                        hw_status("%s: mtc_test: %s: Rank%d Slice%d Sample%d: checker adjust = %xH\n", get_sim_time(), __func__, rankid, slice, i, adjust);
                    }
                } //(!force_fail[slice])
                else
                {
                    left_mov_status = 0; // fail 
                    hw_status("%s: mtc_test: %s: Rank%d Slice%d Sample%d: left mov FAIL due to force_fail\n", get_sim_time(), __func__, rankid, slice, i);
                }
            }

            if (!sample_break) 
            {
                if (!force_fail[slice])
                {
                    if (right_mov_status && !left_mov_status && (i == (sample_cnt - 1))) 
                    {
                       auto_adjusted_delay += adj_step;
                       auto_adjusted_delay &= 0x1ff;
                       auto_exp_right_adjust[rankid][slice]++;
                       possible_movement_count++;
                       if ( adjust_gate_delay[rankid][slice] + adj_step >  dxngsr0_gdqsprd[rankid][slice] ) {

                           if ( dxngtr0_dgsl[rankid][slice] <= 17 ) {
                             dxngtr0_dgsl[rankid][slice]++;
                              adjust_gate_delay[rankid][slice] +=  adj_step;
                             adjust_gate_delay[rankid][slice] -=  dxngsr0_gdqsprd[rankid][slice];
                             hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: Increment dxngtr0_dgsl\n", 
                                 get_sim_time(), __func__, rankid, slice, i);
                           } else {
                               adjust_gate_delay[rankid][slice]= dxngsr0_gdqsprd[rankid][slice];
                           }
                       } else {
                              adjust_gate_delay[rankid][slice] += adj_step;
                       }
            
                    } 
    
                    if ( !right_mov_status && left_mov_status && (i == (sample_cnt - 1))) {
                        auto_adjusted_delay -= adj_step;
                        auto_adjusted_delay &= 0x1ff;
                        auto_exp_left_adjust[rankid][slice]++;
                        possible_movement_count++;
                        if ( adjust_gate_delay[rankid][slice] - adj_step < 0) {
                            if (  dxngtr0_dgsl[rankid][slice] !=0 ) {
                              dxngtr0_dgsl[rankid][slice]--;
                              adjust_gate_delay[rankid][slice] -=  adj_step;
                              adjust_gate_delay[rankid][slice] +=  dxngsr0_gdqsprd[rankid][slice];
                              hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: Decrement dxngtr0_dgsl\n",
                                  get_sim_time(), __func__, rankid, slice, i);
                            } else {
                              dxngtr0_dgsl[rankid][slice] = 0;
                            }
                        } else {
                               adjust_gate_delay[rankid][slice] -= adj_step;
                        }
                    } 

                    if ( !right_mov_status && !left_mov_status ) 
                    {
                        auto_adjusted_delay = initial_delay_val[slice];
                        auto_adjusted_delay &= 0x1ff;
                        //auto_exp_right_adjust = 0;
                        //auto_exp_left_adjust = 0;

                        if ( i == (sample_cnt - 1)) {
                             auto_exp_trfail++;
                             auto_exp_training_test_pass = 0;
                        }
                    }
                } //(!force_fail[slice])
                else
                {
                     //Don't change the adjusted delay as there is no movement.
                     if ( i == (sample_cnt - 1)) 
                     {
                         auto_exp_trfail++;
                         auto_exp_training_test_pass = 0;
                     }
                }
            } //(!sample_break)

            if ( sample_break ) 
            {
                if ( i % 2 == 0 ) 
                {
                    if (!force_fail[slice])
                    {
                        if ( right_mov_status && (i == (sample_cnt*2 - 2))) {
                          auto_adjusted_delay += adj_step;
                          auto_adjusted_delay &= 0x1ff;
                          auto_exp_right_adjust[rankid][slice]++;
                          possible_movement_count++;
                          if ( adjust_gate_delay[rankid][slice] >  dxngsr0_gdqsprd[rankid][slice] ) {
                                dxngtr0_dgsl[rankid][slice]++;
                                adjust_gate_delay[rankid][slice] +=  adj_step;
                                adjust_gate_delay[rankid][slice] -=  dxngsr0_gdqsprd[rankid][slice];
                                hw_status(" Increment dxngtr0_dgsl sample_break \n");
                          } else {
                                adjust_gate_delay[rankid][slice] += adj_step;
                          }
                        }
                    } //(!force_fail[slice])
                    else
                    {
                        //Nothing to do here
                    }
                } 
                else 
                {
                    if (!force_fail[slice])
                    {
                        if ( left_mov_status && (i == (sample_cnt*2 - 1))) {
                          auto_adjusted_delay -= adj_step;
                          auto_adjusted_delay &= 0x1ff;
                          auto_exp_left_adjust[rankid][slice]++;
                          possible_movement_count++;
                          if ( adjust_gate_delay[rankid][slice] < 0) {
                              dxngtr0_dgsl[rankid][slice]--;
                              adjust_gate_delay[rankid][slice] -=  adj_step;
                              adjust_gate_delay[rankid][slice] +=  dxngsr0_gdqsprd[rankid][slice];
                              hw_status(" Decrement dxngtr0_dgsl sample_break \n");
                          } else {
                              adjust_gate_delay[rankid][slice] -= adj_step;
                          }
                        }

                        if ( !right_mov_status && !left_mov_status ) {
                           if ( i == (sample_cnt*2 - 1)) {
                                 auto_exp_trfail++;
                                 auto_exp_training_test_pass = 0;
                           }
                        }
                    } //(!force_fail[slice])
                    else
                    {
                        //No movement, and make the training fail by force.
                        if ( i == (sample_cnt*2 - 1)) {
                              auto_exp_trfail++;
                              auto_exp_training_test_pass = 0;
                        }
                    } //(force_fail[slice])
                }
            } //(sample_break)

            hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: checker auto_exp_right_adjust val = %xH\n",
                get_sim_time(), __func__, rankid, slice, i, auto_exp_right_adjust[rankid][slice]);
            hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: checker auto_exp_left_adjust = %xH\n",
                get_sim_time(), __func__, rankid, slice, i, auto_exp_left_adjust[rankid][slice]);
            hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: checker adjusted delay val = %xH\n",
                get_sim_time(), __func__, rankid, slice, i, auto_adjusted_delay);
            hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: checker auto_exp_trfail val = %xH\n",
                get_sim_time(), __func__, rankid, slice, i, auto_exp_trfail);
            hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: checker possible_movement_count val = %xH\n",
                get_sim_time(), __func__, rankid, slice, i, possible_movement_count);
            hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: checker auto_exp_training_test_pass val = %xH\n",
                get_sim_time(), __func__, rankid, slice, i, auto_exp_training_test_pass);
            hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: checker auto_exp_trans_right_pass_count val = %xH\n",
                get_sim_time(), __func__, rankid, slice, i, auto_exp_trans_right_pass_count);
            hw_status("%s: mtc_test: %s: Rank%d Slice%d: Sample%d: checker auto_exp_trans_left_pass_count val = %xH\n",
                get_sim_time(), __func__, rankid, slice, i, auto_exp_trans_left_pass_count);
              
        } //for (sample_cnt)

        if ((auto_exp_trans_right_pass_count < pass_criteria) && ( auto_exp_trans_left_pass_count < pass_criteria)) 
        {
            if ( pass_criteria_flag == 0 ) {
                auto_exp_training_test_pass = 0;
            }
        } 
        else 
        {
            pass_criteria_flag = 1;
        }

        // ----- Training status expected calculation  ----
        if ( pass_criteria == 0 ) 
        {
            auto_exp_right_adjust[rankid][slice] = 0;
            auto_exp_left_adjust[rankid][slice] = 0;
            auto_exp_trfail = 0;
        }

        if ( phy_based_delay_adj == 1) 
        {   // No adjustment happens for phy_based delay feature 
            auto_exp_right_adjust[rankid][slice] = 0;
            auto_exp_left_adjust[rankid][slice] = 0;
            auto_adjusted_delay = initial_delay_val[slice];
            adjust_gate_delay[rankid][slice] = initial_delay_val[slice]; // to be changed later 
        }

        if ( slice == 0 ) {
            // stats 

            slice_reg_address = rankid ? MCU_REGS_DDC_REGS_RD_RIGHT_DQS_ADJUSTS_SLICE0_RANK1_ADDRESS
                                    : MCU_REGS_DDC_REGS_RD_RIGHT_DQS_ADJUSTS_SLICE0_RANK0_ADDRESS;

            rd_val = hw_read32((uint32_t *) slice_reg_address);
            hw_status("%s: mtc_test: %s: Rank%d Slice%d: RD_RIGHT_DQS_ADJUSTS_SLICE0 regval = 0x%x\n", get_sim_time(), __func__, rankid, slice, rd_val);
            if ( rd_val != auto_exp_right_adjust[rankid][slice]) 
            {
                hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR : RD_RIGHT_DQS_ADJUSTS_SLICE0 comparison failed!!. exp = 0x%x\n",
                    get_sim_time(), __func__, rankid, slice, auto_exp_right_adjust[rankid][slice]);
            } 
            else 
            {
                hw_status("%s: mtc_test: %s: Rank%d Slice%d: RD_RIGHT_DQS_ADJUSTS_SLICE0 comparison PASSED!!.\n", get_sim_time(), __func__, rankid, slice);
            }


            slice_reg_address = rankid ? MCU_REGS_DDC_REGS_RD_LEFT_DQS_ADJUSTS_SLICE0_RANK1_ADDRESS 
                                   : MCU_REGS_DDC_REGS_RD_LEFT_DQS_ADJUSTS_SLICE0_RANK0_ADDRESS;

            rd_val = hw_read32((uint32_t *) slice_reg_address);
            hw_status("%s: mtc_test: %s: Rank%d Slice%d: RD_LEFT_DQS_ADJUSTS_SLICE0 regval = 0x%x\n", 
                get_sim_time(), __func__, rankid, slice, rd_val);
            if ( rd_val != auto_exp_left_adjust[rankid][slice]) {
               hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR : RD_LEFT_DQS_ADJUSTS_SLICE0 comparison failed!!. exp = 0x%x\n",
                   get_sim_time(), __func__, rankid, slice, auto_exp_left_adjust[rankid][slice]);
            } else {
               hw_status("%s: mtc_test: %s: Rank%d Slice%d: RD_LEFT_DQS_ADJUSTS_SLICE0 comparison PASSED!!.\n", get_sim_time(), __func__, rankid, slice);
            }
        }  //if (slice==0)  

        if ( slice == 1) 
        {

        slice_reg_address  = rankid ? MCU_REGS_DDC_REGS_RD_RIGHT_DQS_ADJUSTS_SLICE1_RANK1_ADDRESS 
                                 : MCU_REGS_DDC_REGS_RD_RIGHT_DQS_ADJUSTS_SLICE1_RANK0_ADDRESS;

        rd_val = hw_read32((uint32_t *) slice_reg_address);
        hw_status("%s: mtc_test: %s: Rank%d Slice%d: RD_RIGHT_DQS_ADJUSTS_SLICE1 regval = %xH\n",
            get_sim_time(), __func__, rankid, slice, rd_val);
        if ( rd_val != auto_exp_right_adjust[rankid][slice]) 
        {
           hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR : RD_RIGHT_DQS_ADJUSTS_SLICE1 comparison failed!!. exp=%xH\n",
            get_sim_time(), __func__, rankid, slice,auto_exp_right_adjust[rankid][slice]);
        } 
        else 
        {
           hw_status("%s: mtc_test: %s: Rank%d Slice%d: RD_RIGHT_DQS_ADJUSTS_SLICE1 comparison PASSED!!.\n", get_sim_time(), __func__, rankid, slice);
        }


        slice_reg_address  = rankid ? MCU_REGS_DDC_REGS_RD_LEFT_DQS_ADJUSTS_SLICE1_RANK1_ADDRESS
                             : MCU_REGS_DDC_REGS_RD_LEFT_DQS_ADJUSTS_SLICE1_RANK0_ADDRESS;

        rd_val = hw_read32((uint32_t *) slice_reg_address);
        hw_status("%s: mtc_test: %s: Rank%d Slice%d: RD_LEFT_DQS_ADJUSTS_SLICE1 regval = %xH\n",
            get_sim_time(), __func__, rankid, slice, rd_val);
        if ( rd_val != auto_exp_left_adjust[rankid][slice]) 
        {
           hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR : RD_LEFT_DQS_ADJUSTS_SLICE1 comparison failed!!. exp=%xH\n",
            get_sim_time(), __func__, rankid, slice,auto_exp_left_adjust[rankid][slice]);
        } 
        else 
        {
           hw_status("%s: mtc_test: %s: Rank%d Slice%d: RD_LEFT_DQS_ADJUSTS_SLICE1 comparison PASSED!!.\n",
            get_sim_time(), __func__, rankid, slice);
        }
        }  //if (slice==1)  

        for (i=1; i < NUM_READ_DELAYS ; i++) 
        {
            /*
            if ( possible_movement_count == 0 ) 
            {
                if ( right_move_delay_predict_rd == 1 )
                {
                    compare_val = auto_adjusted_delay + global_wire_delay_reg_rd;
                    compare_val &= WDLY_MASK;
                } 
                else 
                {
                    compare_val = auto_adjusted_delay - global_wire_delay_reg_rd;
                    compare_val &= WDLY_MASK;
                } 
            } 
            else 
            {
                if ( right_move_delay_predict_rd == 1 ) 
                {
                    compare_val = auto_adjusted_delay + global_wire_delay_reg_rd;
                    compare_val &= WDLY_MASK;
                } 
                else 
                {
                    compare_val = auto_adjusted_delay - global_wire_delay_reg_rd;
                    compare_val &= WDLY_MASK;
                }
            }
            */
            compare_val = auto_adjusted_delay - global_wire_delay_reg_rd[rankid][slice];
            compare_val &= WDLY_MASK;

            rd_val = get_phy_delay_read_reg(i*MAX_SLICES+slice,rankid);
            //global_final_adj_values[rankid][slice] = auto_adjusted_delay;
#ifdef CONF_HAS___PHY_RTL
            global_final_adj_values[rankid][slice] = compare_val + ((dxngtr0_dgsl[rankid][slice]-GOLDEN_RDGSL_VAL_1600)*dxngsr0_gdqsprd[rankid][slice]);
#else
            global_final_adj_values[rankid][slice] = compare_val;
#endif
            hw_status("%s: mtc_test: %s: Rank%d Slice%d: Setting global_final_adj_values[%d][%d] = 0x%x\n",
             get_sim_time(), __func__, rankid, slice, rankid, slice, global_final_adj_values[rankid][slice]);

            hw_status("%s: mtc_test: %s: Rank%d Slice%d: Delay[%d] = %xH\n",
             get_sim_time(), __func__, rankid, slice,i,rd_val);
            if ( compare_val != rd_val) {
               hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR : adjusted delay comparison failed!!. exp=%xH rd_val =%xH\n",
             get_sim_time(), __func__, rankid, slice,auto_adjusted_delay,rd_val);
            } else {
               hw_status("%s: mtc_test: %s: Rank%d Slice%d: adjusted delay comparison PASSED!!.\n",
             get_sim_time(), __func__, rankid, slice);
            }
        } //for (i<NUM_READ_DELAYS)

        // --- check MC delay registers ----
        for (i=1; i < NUM_READ_DELAYS ; i++) {
            /*
            if ( possible_movement_count == 0 ) {
                if ( right_move_delay_predict_rd == 1 ){
                    compare_val = auto_adjusted_delay + global_wire_delay_reg_rd;
                    compare_val &= WDLY_MASK;
                } else {
                    compare_val = auto_adjusted_delay - global_wire_delay_reg_rd;
                    compare_val &= WDLY_MASK;
                } 
            } else {
                if ( right_move_delay_predict_rd == 1 ) {
                    compare_val = auto_adjusted_delay + global_wire_delay_reg_rd;
                    compare_val &= WDLY_MASK;
                } else {
                    compare_val = auto_adjusted_delay - global_wire_delay_reg_rd;
                    compare_val &= WDLY_MASK;
                }
            }
            */
            compare_val = auto_adjusted_delay - global_wire_delay_reg_rd[rankid][slice];
            compare_val &= WDLY_MASK;

            rd_val = get_mtc_delay_read_reg(i*MAX_SLICES+slice,rankid);

            //global_final_gate_adj_values[rankid][slice] = auto_adjusted_delay;
            global_final_gate_adj_values[rankid][slice] = compare_val;

            hw_status("%s: mtc_test: %s: Rank%d Slice%d: Delay[%d] = %xH\n",
             get_sim_time(), __func__, rankid, slice,i,rd_val);
            if ( compare_val != rd_val) {
               hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR : mtc reg adjusted delay comparison failed!!. exp=%xH rd_val=%xH\n",
             get_sim_time(), __func__, rankid, slice,auto_adjusted_delay,rd_val);
            } else {
               hw_status("%s: mtc_test: %s: Rank%d Slice%d: mtc reg adjusted delay comparison PASSED!!.\n",
             get_sim_time(), __func__, rankid, slice);
            }
        } //for (i<NUM_READ_DELAYS), second loop

        if ( gate_adj_disable == 1 )  {// gate delay adjustment mode is enabled 
         for (i=0; i < 1 ; i++) {
          if ( possible_movement_count == 0 ) {
            /*
            if ( right_move_delay_predict_rd == 1 ){
              compare_val = auto_adjusted_delay + global_wire_delay_reg_rd;
              compare_val &= WDLY_MASK;
            } else {
              compare_val = auto_adjusted_delay - global_wire_delay_reg_rd;
              compare_val &= WDLY_MASK;
            } 
            */
            compare_val = auto_adjusted_delay - global_wire_delay_reg_rd[rankid][slice];
            compare_val &= WDLY_MASK;
          } else {
            /*
            if ( right_move_delay_predict_rd == 1 ) {
              compare_val = adjust_gate_delay[rankid][slice] + global_wire_delay_reg_rd;
              compare_val &= WDLY_MASK;
            } else {
              compare_val = adjust_gate_delay[rankid][slice] - global_wire_delay_reg_rd;
              compare_val &= WDLY_MASK;
            }
            */
            compare_val = adjust_gate_delay[rankid][slice] - global_wire_delay_reg_rd[rankid][slice];
            compare_val &= WDLY_MASK;
          }
           rd_val = get_phy_delay_read_reg(i*MAX_SLICES+slice,rankid);
           hw_status("%s: mtc_test: %s: Rank%d Slice%d: Gate Delay[%d] = %xH\n",
            get_sim_time(), __func__, rankid, slice,i,rd_val);
           if ( phy_based_delay_adj == 0 ) {
              if ( compare_val != rd_val) {
                 hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR : adjusted delay comparison failed!!. exp=%xH rd_val=%xH\n",
            get_sim_time(), __func__, rankid, slice,adjust_gate_delay[rankid][slice],rd_val);
              } else {
                 hw_status("%s: mtc_test: %s: Rank%d Slice%d: gate adjusted delay comparison PASSED!!.\n",
            get_sim_time(), __func__, rankid, slice);
              }
           } else { // phy_based_delay_adj ==  1
               if ( global_drift_enabled_for_phy_based_training) {

                   /*  --- delay checking to end of test as drift detection hasnt taken place yet 
                   if ( compare_val != rd_val ) {

                    hw_status("%s: mtc_test: %s: Rank%d Slice%d: Change detected in gate adjusted delay comparison for PHY based test drift detection orig=%xH , new_val=%xH!!.\n",
            get_sim_time(), __func__, rankid, slice,
                            adjust_gate_delay[rankid][slice],rd_val);
                   } else {
                    hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR : adjusted delay comparison change failed for PHY based test !!. exp=%xH rd_val=%xH\n",
            get_sim_time(), __func__, rankid, slice,
                            adjust_gate_delay[rankid][slice],rd_val);
                   }
                   */
    
               }
               else {  // global_drift_enabled_for_phy_based_training == 0
                   if ( compare_val != rd_val ) {
                    hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR : adjusted delay comparison change failed for PHY based test with no drift addition !!. exp=%xH rd_val=%xH\n",
            get_sim_time(), __func__, rankid, slice,
                            adjust_gate_delay[rankid][slice],rd_val);
                   } else {
                    hw_status("%s: mtc_test: %s: Rank%d Slice%d: gate adjusted delay comparison for PHY based test with no drift addition PASSED  orig=%xH , new_val=%xH!!.\n",
            get_sim_time(), __func__, rankid, slice,
                            adjust_gate_delay[rankid][slice],rd_val);
                   }
               }
           }

           // --- check MC delay registers ----
           rd_val = get_mtc_delay_read_reg(i*MAX_SLICES+slice,rankid);
           hw_status("%s: mtc_test: %s: Rank%d Slice%d: MTC Gate Delay[%d] = %xH\n",
            get_sim_time(), __func__, rankid, slice,i,rd_val);
           if ( compare_val  != rd_val) {
              hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR : mtc adjusted delay comparison failed!!. exp=%xH rd_val=%xH\n",
            get_sim_time(), __func__, rankid, slice,adjust_gate_delay[rankid][slice],rd_val);
           } else {
              hw_status("%s: mtc_test: %s: Rank%d Slice%d: mtc gate adjusted delay comparison PASSED!!.\n",
            get_sim_time(), __func__, rankid, slice);
           }
         }

           rd_value = get_phy_dxngtr0_dgsl(slice,rankid);
           hw_status("%s: mtc_test: %s: Rank%d Slice%d: DGSL[%d] = %xH\n",
            get_sim_time(), __func__, rankid, slice,slice,rd_value);
           if ( dxngtr0_dgsl[rankid][slice] != rd_value ) {
              hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR : DGSL delay comparison failed!!. exp=%xH\n",
            get_sim_time(), __func__, rankid, slice,dxngtr0_dgsl[rankid][slice]);
           } else {
              hw_status("%s: mtc_test: %s: Rank%d Slice%d: DGSL delay comparison PASSED!!.\n",
            get_sim_time(), __func__, rankid, slice);
           }

           // --- check mtc_gtr0 dgsl -----
           rd_value = get_mtc_dxngtr0_dgsl(slice,rankid);
           hw_status("%s: mtc_test: %s: Rank%d Slice%d: MTC DGSL[%d] = %xH\n",
            get_sim_time(), __func__, rankid, slice,slice,rd_value);
           if ( dxngtr0_dgsl[rankid][slice] != rd_value ) {
              hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR : MTC DGSL delay comparison failed!!. exp=%xH\n",
            get_sim_time(), __func__, rankid, slice,dxngtr0_dgsl[rankid][slice]);
           } else {
              hw_status("%s: mtc_test: %s: Rank%d Slice%d: MTC DGSL delay comparison PASSED!!.\n",
            get_sim_time(), __func__, rankid, slice);
           }

           // ----- This check is not required ------
           rd_value = get_phy_dxngsr0_gdqsprd(slice,rankid);
           hw_status("%s: mtc_test: %s: Rank%d Slice%d: GDQSPRD[%d] = %xH\n",
            get_sim_time(), __func__, rankid, slice,slice,rd_value);

           //if ( dxngsr0_gdqsprd[rankid][slice] != rd_value ) {
           //   hw_status("%s: mtc_test: %s: Rank%d Slice%d: ERROR : GDQSPRD delay comparison failed!!. exp=%xH\n",dxngsr0_gdqsprd[rankid][slice]);
           //} else {
           //   hw_status("%s: mtc_test: %s: Rank%d Slice%d: GDQSPRD delay comparison PASSED!!.\n");
           //}
        }

    } //for (Slice)

    //if ( possible_movement_count > 0) {
    //     auto_exp_trfail = 0;
    //} 

    hw_status("%s: mtc_test: %s: Rank%d checker auto_exp_trfail val = %xH\n", get_sim_time(), __func__, rankid, auto_exp_trfail);
    hw_status("%s: mtc_test: %s: Rank%d checker auto_exp_training_test_pass val = %xH\n", get_sim_time(), __func__, rankid, auto_exp_training_test_pass);

    //  ---- Final pass , fail status checking -----
    if ( auto_exp_training_test_pass && (auto_exp_trfail == 0))
    {
        if (DDC_REGS_TRAINING_STATUS_RD_FAIL_GET(last_rd_val) == 0) 
        {
            hw_status("%s: mtc_test: %s: Rank%d: MTC RD TRAINING PASSED !! regval =%xH\n", get_sim_time(), __func__, rankid, last_rd_val);
        }
        else
        {
            hw_status("%s: mtc_test: %s: Rank%d: ERROR : MTC rd_fail status MISMATCH !! actual 0x%x != 0x0 expected\n", get_sim_time(), __func__, rankid, DDC_REGS_TRAINING_STATUS_RD_FAIL_GET(last_rd_val));
        }
    } else if ( (auto_exp_training_test_pass == 0) && (auto_exp_trfail != 0)) 
    {
        if (DDC_REGS_TRAINING_STATUS_RD_FAIL_GET(last_rd_val)) 
        {
            hw_status("%s: mtc_test: %s: Rank%d: MTC RD TRAINING TEST FAIL STATUS AS EXPECTED  !! regval =%xH\n", get_sim_time(), __func__, rankid, last_rd_val);
        } 
        else 
        {
            hw_status("%s: mtc_test: %s: Rank%d: ERROR : MTC rd_Fail status MISMATCH !! actual 0x%x != 0x1 expected\n", get_sim_time(), __func__, rankid, DDC_REGS_TRAINING_STATUS_RD_FAIL_GET(last_rd_val));
        }
    }
}


// ---------------------------------------------
// poll_status_reg_for_phy_update_completion()
// ----------------------------------------------
void poll_status_reg_for_phy_update_completion() {

  uint32_t rd_val;
  uint32_t expect_val;
  int trans_limit = 0;

      // ------------ poll till design signals start of training -------------------
      rd_val = 0x0;
      //expect_val = DDC_REGS_TRAINING_STATUS_BUSY_SET(1) | DDC_REGS_TRAINING_STATUS_TRAINING_IN_PROGRESS_SET(1);
      expect_val = DDC_REGS_TRAINING_STATUS_BUSY_SET(0);
      rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);

      while (rd_val != expect_val) { 
         rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
         last_rd_val = rd_val;
         hw_status("mtc_phyupdate_training_test:  polling for busy training status regval = %xH expect_val = %xH\n",
           rd_val,expect_val);

          if ( DDC_REGS_TRAINING_STATUS_PHYUPD_FAIL_GET(rd_val)) {
             if ( mtc_phy_update_fail_expected == 0 ) {
               hw_status("mtc_phyupdate_training_test: ERROR : MTC PHY UPDATE FAILED !! regval =%xH\n",rd_val);
             }
             break;
          }
      }

      // -------------- poll for status completion of training -------------------
      rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
      //expect_val =  DDC_REGS_TRAINING_STATUS_BUSY_SET(0) |  DDC_REGS_TRAINING_STATUS_TRAINING_IN_PROGRESS_SET(0);
      expect_val =  DDC_REGS_TRAINING_STATUS_BUSY_SET(1);

      hw_status("mtc_phyupdate_training_test:  phy update training status regval = %xH expect_val = %xH\n",
           rd_val,expect_val);

      while ((rd_val & DDC_REGS_TRAINING_STATUS_BUSY_FIELD_MASK) != expect_val) { 

         rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
         last_rd_val = rd_val;
         hw_status("mtc_phyupdate_training_test:  polling for done phy training status regval = %xH expect_val = %xH\n",
           rd_val,expect_val);

           trans_limit++;
           //if ( trans_limit == 50) {
           //   break;
           //}

           // check phyupdate training fail status 

          if ( DDC_REGS_TRAINING_STATUS_PHYUPD_FAIL_GET(rd_val)) {
             if ( mtc_phy_update_fail_expected == 0 ) {
               hw_status("mtc_phyupdate_training_test: ERROR : MTC PHY UPDATE FAILED !! regval =%xH\n",rd_val);
             }
             break;
          }
      }

      if ( DDC_REGS_TRAINING_STATUS_PHYUPD_FAIL_GET(rd_val)) {
            if (  mtc_phy_update_fail_expected == 0 ) {
               hw_status("mtc_phyupdate_training_test: ERROR : MTC PHY UPDATE FAILED !! regval =%xH\n",rd_val);
            }
      }

}


// ------------------------------------
// mtc_phy_update_training_test()
// ------------------------------------
void mtc_phy_update_training_test() {
  uint32_t rd_val;
  uint32_t expect_val;

    hw_status("Entering %s\n", __func__);

    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_CNT_ADDRESS, 1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_DFI_TP_CTRLUPD_MAX_ADDRESS, 200);

     // enable 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x0);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);
    hw_status("mtc_all_training_test: Enable MTC PHY UPDATE Training\n");

    enable_auto_refresh_with_alignment();

    poll_status_reg_for_phy_update_completion();

    if ( ! DDC_REGS_TRAINING_STATUS_PHYUPD_FAIL_GET(last_rd_val)) {
             hw_status("mtc_phyupdate_training_test: MTC PHY UPDATE TRAINING COMPLETED!!! regval =%xH\n",last_rd_val);
    }

    // Disable all training 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);

    // Dont allow any activity 
    simulation_tb_wait(10000,"sch");

    hw_status("Exiting %s\n", __func__);

}

// --------------------------------------------
// mtc_phy_update_training_test_wait_expired()
// --------------------------------------------
void mtc_phy_update_training_test_wait_expired() {
  uint32_t rd_val;
  uint32_t expect_val;

    hw_status("Entering %s\n", __func__);
   mtc_phy_update_fail_expected = 1;

    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_CNT_ADDRESS, 1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_DFI_TP_CTRLUPD_MAX_ADDRESS, 30);

     // enable 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x0);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);
    hw_status("mtc_all_training_test: Enable MTC PHY UPDATE Training\n");

    enable_auto_refresh_with_alignment();

    poll_status_reg_for_phy_update_completion();

    if ( ! DDC_REGS_TRAINING_STATUS_PHYUPD_FAIL_GET(last_rd_val)) {
             hw_status("mtc_phyupdate_training_test: MTC PHY UPDATE TRAINING COMPLETED!!! regval =%xH\n",last_rd_val);
    }

    // Disable all training 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);

    // Dont allow any activity 
    simulation_tb_wait(10000,"sch");

    hw_status("Exiting %s\n", __func__);

}

// ---------------------------------------------------------
// mtc_phy_update_training_test_wait_expired_with_ack()
// ---------------------------------------------------------
void mtc_phy_update_training_test_wait_expired_with_ack() {
  uint32_t rd_val;
  uint32_t expect_val;

    hw_status("Entering %s\n", __func__);
   mtc_phy_update_fail_expected = 1;

    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_CNT_ADDRESS, 1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_DFI_TP_CTRLUPD_MAX_ADDRESS, 100);

     // enable 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x0);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);
    hw_status("mtc_all_training_test: Enable MTC PHY UPDATE Training\n");

    enable_auto_refresh_with_alignment();

    poll_status_reg_for_phy_update_completion();

    if ( ! DDC_REGS_TRAINING_STATUS_PHYUPD_FAIL_GET(last_rd_val)) {
             hw_status("mtc_phyupdate_training_test: MTC PHY UPDATE TRAINING COMPLETED!!! regval =%xH\n",last_rd_val);
    }

    // Disable all training 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);

    // Dont allow any activity 
    simulation_tb_wait(10000,"sch");

    hw_status("Exiting %s\n", __func__);

}

// ---------------------------------------------------------
// mtc_phy_update_training_test_wait_expired_with_req()
// ---------------------------------------------------------
void mtc_phy_update_training_test_wait_expired_with_req() {
  uint32_t rd_val;
  uint32_t expect_val;

    hw_status("Entering %s\n", __func__);
   mtc_phy_update_fail_expected = 1;

    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_CNT_ADDRESS, 1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_DFI_TP_CTRLUPD_MAX_ADDRESS, 10);

     // enable 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x0);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);
    hw_status("mtc_all_training_test: Enable MTC PHY UPDATE Training\n");

    enable_auto_refresh_with_alignment();

    poll_status_reg_for_phy_update_completion();

    if ( ! DDC_REGS_TRAINING_STATUS_PHYUPD_FAIL_GET(last_rd_val)) {
             hw_status("mtc_phyupdate_training_test: MTC PHY UPDATE TRAINING COMPLETED!!! regval =%xH\n",last_rd_val);
    }

    // Disable all training 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);

    // Dont allow any activity 
    simulation_tb_wait(10000,"sch");

    hw_status("Exiting %s\n", __func__);

}

// ---------------------------------------------------------
// mtc_phy_update_training_test_wait_timeout_max()
// ---------------------------------------------------------
void mtc_phy_update_training_test_wait_timeout_max() {
  uint32_t rd_val;
  uint32_t expect_val;

    hw_status("Entering %s\n", __func__);
   mtc_phy_update_fail_expected = 1;

    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_CNT_ADDRESS, 1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_DFI_TP_CTRLUPD_MAX_ADDRESS, 0xffffffff);

     // enable 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x0);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);
    hw_status("mtc_all_training_test: Enable MTC PHY UPDATE Training\n");

    enable_auto_refresh_with_alignment();

    poll_status_reg_for_phy_update_completion();

    if ( ! DDC_REGS_TRAINING_STATUS_PHYUPD_FAIL_GET(last_rd_val)) {
             hw_status("mtc_phyupdate_training_test: MTC PHY UPDATE TRAINING COMPLETED!!! regval =%xH\n",last_rd_val);
    }

    // Disable all training 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);

    // Dont allow any activity 
    simulation_tb_wait(10000,"sch");

    hw_status("Exiting %s\n", __func__);

}

// ---------------------------------------------------------
// mtc_phy_update_training_test_random_CTRLUPD()
// ---------------------------------------------------------
void mtc_phy_update_training_test_random_CTRLUPD() {
  uint32_t rd_val;
  uint32_t expect_val;

    hw_status("Entering %s\n", __func__);
//   mtc_phy_update_fail_expected = 1;

    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_CNT_ADDRESS, 1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_DFI_TP_CTRLUPD_MAX_ADDRESS, rand_interval(100,0xffff));

     // enable 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x0);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);
    hw_status("mtc_all_training_test: Enable MTC PHY UPDATE Training\n");

    enable_auto_refresh_with_alignment();

    poll_status_reg_for_phy_update_completion();

    if ( ! DDC_REGS_TRAINING_STATUS_PHYUPD_FAIL_GET(last_rd_val)) {
             hw_status("mtc_phyupdate_training_test: MTC PHY UPDATE TRAINING COMPLETED!!! regval =%xH\n",last_rd_val);
    }

    // Disable all training 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);

    // Dont allow any activity 
    simulation_tb_wait(10000,"sch");

    hw_status("Exiting %s\n", __func__);

}

// ----------------------------------
// mtc_zqcal_training_test_lpddr3()  ZQ 
// ----------------------------------
// cal_type = 0 (short) or 1 (long), zqcal init(2)
void mtc_zqcal_training_test_lpddr3(int cal_type) {
  uint32_t rd_val;
  uint32_t expect_val;

    hw_status("Entering %s cal_type - %xH\n", __func__,cal_type);

     // enable  zq cal 
    /// Enable LPDDR3 ZQ Cal 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_ZQ_CAL_MODE_ADDRESS, 0x1);

    // ---- MR10 Write for ZQ Cal for LPDDR3 ---
    // 0xFF: Calibration command after initialization 
    // 0xAB: Long calibration
    // 0x56: Short calibration
    // 0xC3: ZQ Reset
 
    if ( cal_type == 1) {  // Long training 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MR_VALUE_ADDRESS, 0xAB);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT1_ADDRESS, 144);   // 360 ns / 2.5 ns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MR_ADDR_ADDRESS, 0xa);
       //simulation_wait_ns(1000); // wait for tzqinit min time 1us 

       hw_status("mtc_zqcal_training_test_lpddr3: Enable MTC ZQ Cal Long Training\n");
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x1);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);

       enable_auto_refresh_with_alignment();

    }
    else if(cal_type == 0) {  // Short training 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MR_VALUE_ADDRESS, 0x56);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT1_ADDRESS, 36); // 90ns / 2.5 ns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MR_ADDR_ADDRESS, 0xa);

       hw_status("mtc_zqcal_training_test_lpddr3: Enable MTC ZQ Cal Short Training\n");
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x1);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);

       enable_auto_refresh_with_alignment();
       
    } else if ( cal_type == 2) { // ZQ cal after init 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MR_VALUE_ADDRESS, 0xFF);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT1_ADDRESS, 400); // 1us / 2.5 ns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MR_ADDR_ADDRESS, 0xa);

       hw_status("mtc_zqcal_training_test_lpddr3: Enable MTC ZQ Cal Long Training\n");
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x1);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);
       
       enable_auto_refresh_with_alignment();

    }

      // ------------ poll till design signals start of training -------------------
      rd_val = 0;
      expect_val = DDC_REGS_TRAINING_STATUS_BUSY_SET(1) | DDC_REGS_TRAINING_STATUS_TRAINING_IN_PROGRESS_SET(2);

      while (rd_val != expect_val) { 
         rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
         hw_status("mtc_zqcal_training_test_lpddr3:  polling for busy training status regval = %xH expect_val = %xH\n",
           rd_val,expect_val);
      }

      // -------------- poll for status completion of training -------------------
      rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
      expect_val =  DDC_REGS_TRAINING_STATUS_BUSY_SET(0) |  DDC_REGS_TRAINING_STATUS_TRAINING_IN_PROGRESS_SET(0);

      hw_status("mtc_zqcal_training_test_lpddr3:  zqcal training status regval = %xH expect_val = %xH\n",
           rd_val,expect_val);

      while (rd_val != expect_val) { 

         rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
         hw_status("mtc_zqcal_training_test_lpddr3:  polling for done zqcal training status regval = %xH expect_val = %xH\n",
           rd_val,expect_val);
           // check zqcal training fail status 

          //if ( DDC_REGS_TRAINING_STATUS_ZQCAL_FAIL_GET(rd_val)) {
          //   hw_status("mtc_zqcal_training_test_lpddr3: ERROR : MTC ZQ CAL FAILED !! regval =%xH\n",rd_val);
          //   break;
          //}
      }

      //if ( ! DDC_REGS_TRAINING_STATUS_ZQCAL_FAIL_GET(rd_val)) {
      //      hw_status("mtc_zqcal_training_test_lpddr3: : MTC ZQ CAL COMPLETED!! regval =%xH\n",rd_val);
      //}
      // Disable all training 
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);

      // wait for tzqinit min time 1us 
      // Dont allow any activity
      simulation_tb_wait(10000,"sch");

    hw_status("Exiting %s\n", __func__);

}

// --------------------------------------------------
// poll_status_reg_for_zqcal_training_completion()
// --------------------------------------------------
void poll_status_reg_for_zqcal_training_completion() {
  uint32_t rd_val;
  uint32_t expect_val;

      // ------------ poll till design signals start of training -------------------
      rd_val = 0x0;
      //expect_val = DDC_REGS_TRAINING_STATUS_BUSY_SET(1) | DDC_REGS_TRAINING_STATUS_TRAINING_IN_PROGRESS_SET(2);
      expect_val = DDC_REGS_TRAINING_STATUS_BUSY_SET(0);

      while (rd_val != expect_val) { 
         rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
         hw_status("poll_status_reg_for_zqcal_training_completion:  polling for zqcal busy training status regval = %xH expect_val = %xH\n",
           rd_val,expect_val);
      }

      // -------------- poll for status completion of training -------------------
      rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
      //expect_val =  DDC_REGS_TRAINING_STATUS_BUSY_SET(0) |  DDC_REGS_TRAINING_STATUS_TRAINING_IN_PROGRESS_SET(0);
      expect_val =  DDC_REGS_TRAINING_STATUS_BUSY_SET(1);

      hw_status("poll_status_reg_for_zqcal_training_completion:  zqcal training status regval = %xH expect_val = %xH\n",
           rd_val,expect_val);

      while ((rd_val & DDC_REGS_TRAINING_STATUS_BUSY_FIELD_MASK) != expect_val) { 

         rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
         hw_status("poll_status_reg_for_zqcal_training_completion:  polling for zqcal done training status regval = %xH expect_val = %xH\n",
           rd_val,expect_val);
           // check zqcal training fail status 


          //if ( DDC_REGS_TRAINING_STATUS_ZQCAL_FAIL_GET(rd_val)) {
          //   hw_status("poll_status_reg_for_zqcal_training_completion: ERROR : MTC ZQ CAL FAILED !! regval =%xH\n",rd_val);
          //   break;
          //}
      }

      //if ( ! DDC_REGS_TRAINING_STATUS_ZQCAL_FAIL_GET(rd_val)) {
      //      hw_status("poll_status_reg_for_zqcal_training_completion: : MTC ZQ CAL COMPLETED!! regval =%xH\n",rd_val);
      //}

}


// --------------------------------------------------
// poll_status_reg_for_write_training_completion()
// --------------------------------------------------
void poll_status_reg_for_write_training_completion(const int global_hold_ctrl) 
{
    uint32_t rd_val;
    uint32_t expect_val, expect_mask;
    int display_flag=0;

    // ------------ poll till design signals start of training -------------------
    rd_val = 0;
    //expect_val = DDC_REGS_TRAINING_STATUS_BUSY_SET(1) | DDC_REGS_TRAINING_STATUS_TRAINING_IN_PROGRESS_SET(8) |
    //             DDC_REGS_TRAINING_STATUS_UNFINISHED_WR_TRAINING_SET(1);
    expect_val = DDC_REGS_TRAINING_STATUS_BUSY_SET(0) | DDC_REGS_TRAINING_STATUS_UNFINISHED_WR_TRAINING_SET(1);

    while ((rd_val & DDC_REGS_TRAINING_STATUS_UNFINISHED_WR_TRAINING_FIELD_MASK) != expect_val) { 
        rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
        last_rd_val = rd_val;
        if (display_flag == 0) 
        {
           hw_status("%s: mtc_test: poll_status_reg_for_write_training_completion:  polling for busy training status regval = 0x%x expect_val = 0x%x\n",
               get_sim_time(), rd_val,expect_val);
           display_flag = 1;
        } 
        else 
        {
           hw_status(".");
        }

        if ( DDC_REGS_TRAINING_STATUS_WR_FAIL_GET(rd_val)) 
        {
             hw_status("\n%s: mtc_test: %s: WARNING : MTC WR TRAINING FAILED STATUS !! Not clearning this time; regval = 0x%x\n", get_sim_time(), __func__, rd_val);
             // clear status fail bit 
             // sugopala:  Commented this clearing
             //hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS,  1 << DDC_REGS_TRAINING_STATUS_WR_FAIL_MSB);
             break;
        }
    }

    if (global_hold_ctrl == 1) 
    {
        hw_status("%s: mtc_test: %s: Releasing global hold\n", get_sim_time(), __func__);
        release_global_hold_seq(0, 1, "sch");
    }

    // -------------- poll for status completion of training -------------------
    rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
    //sugopala: updated this status expectation
    expect_val =  DDC_REGS_TRAINING_STATUS_BUSY_SET(1) |  DDC_REGS_TRAINING_STATUS_TRAINING_IN_PROGRESS_SET(0) |
                  DDC_REGS_TRAINING_STATUS_UNFINISHED_WR_TRAINING_SET(0);
    //expect_val =   DDC_REGS_TRAINING_STATUS_UNFINISHED_WR_TRAINING_SET(0);
    expect_mask = DDC_REGS_TRAINING_STATUS_BUSY_FIELD_MASK | DDC_REGS_TRAINING_STATUS_TRAINING_IN_PROGRESS_FIELD_MASK |
                  DDC_REGS_TRAINING_STATUS_UNFINISHED_WR_TRAINING_FIELD_MASK;

    //hw_status("\n%s: mtc_test: poll_status_reg_for_write_training_completion:  write training status regval = 0x%x; expect_val = 0x%x; expect_mask = 0x%x\n",
    //      get_sim_time(), rd_val, expect_val, expect_mask);
    //last_rd_val = rd_val;
    display_flag = 0;
    //while ((rd_val & DDC_REGS_TRAINING_STATUS_UNFINISHED_WR_TRAINING_FIELD_MASK) != expect_val) 
    do { 
        rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
        last_rd_val = rd_val;
        if ( display_flag == 0 ) 
        {
            hw_status("%s: mtc_test: poll_status_reg_for_write_training_completion:  polling for done training status regval = 0x%x; expect_val = 0x%x; expect_mask = 0x%x\n",
                get_sim_time(), rd_val,expect_val, expect_mask);
            display_flag = 1;
        } else {
            hw_status("%");
        }
    }
    while ((rd_val & expect_mask) != expect_val);
    // check write training fail status 
    if (DDC_REGS_TRAINING_STATUS_WR_FAIL_GET(rd_val)) 
    {
        hw_status("\n%s: mtc_test: poll_status_reg_for_write_training_completion: WARNING : MTC WR TRAINING FAILED STATUS !! Clearing this time; regval = 0x%x; saving to last_rd_val = 0x%x\n",get_sim_time(), rd_val, last_rd_val);
        // clear status fail bit 
        hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS,  1 << DDC_REGS_TRAINING_STATUS_WR_FAIL_MSB);
        //sugopala: updated to not break here
        //break;
    }
    hw_status("%s: mtc_test: %s:  last_rd_val = 0x%x\n", get_sim_time(), __func__, last_rd_val);
}


// --------------------------------------
// mtc_write_training_basic_test() W R
// --------------------------------------
void mtc_write_training_basic_test(uint32_t sample_cnt, uint32_t read_wr_num,uint32_t adj_step,uint32_t pass_criteria, uint32_t mov_window,
      uint32_t byte_cnt) {
  uint32_t rd_val;
  uint32_t expect_val;


    hw_status("Entering %s\n", __func__);
    hw_status(" sample_cnt = %xH \n",sample_cnt);
    hw_status(" read_wr_num = %xH \n",read_wr_num);
    hw_status(" adj_step = %xH \n",adj_step);
    hw_status(" pass_criteria = %xH \n",pass_criteria);
    hw_status(" mov_window = %xH \n",mov_window);
    hw_status(" byte_cnt = %xH \n",byte_cnt);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_CNT_ADDRESS, sample_cnt);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_READ_WRITE_NUM_ADDRESS, read_wr_num);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_BREAK_ADDRESS, 0);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MOV_WINDOW_ADDRESS, mov_window << DDC_REGS_MOV_WINDOW_WINDOW_WR_LSB);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_ADJ_STEP_ADDRESS, adj_step);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_BYTE_CNT_ADDRESS, byte_cnt);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_PASS_CRITERIA_ADDRESS, pass_criteria);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);

   if ( random_write_data == 0 ) {
       // Write data patterns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_0_ADDRESS, 0x55555555);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_1_ADDRESS, 0xAAAAAAAA);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_2_ADDRESS, 0xFFFFFFFF);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_3_ADDRESS, 0x00000000);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_4_ADDRESS, 0x11111111);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_5_ADDRESS, 0x22222222);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_6_ADDRESS, 0x33333333);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_7_ADDRESS, 0x44444444);
       // write mask for above 32 bytes 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, 0xFFFFFFFF);

   } else {

     // random data patterns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_0_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_1_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_2_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_3_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_4_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_5_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_6_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_7_ADDRESS, rand());
       // write mask for above 32 bytes 
       //hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, 0xFFFFFFFF);
   }

    if ( dont_program_training_regs_for_write == 0 ) {

       // enable write training order for training 0 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x3);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);

       hw_status("mtc_write_training_basic_test: Enable Write Training\n");

       enable_auto_refresh_with_alignment();

       poll_status_reg_for_write_training_completion(/*global_hold_ctrl*/0);

       if ( ! DDC_REGS_TRAINING_STATUS_WR_FAIL_GET(last_rd_val)) {
             hw_status("mtc_write_training_basic_test: MTC WRITE TRAINING COMPLETED!!! regval =%xH\n",last_rd_val);
       }  else {
             hw_status("mtc_write_training_basic_test: MTC WRITE FAILURE STATUS DETECTED!!! regval =%xH\n",last_rd_val);
       }

       // Disable all training 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);


       check_mtc_training_status_bits();

       // Dont allow any activity while training 
       simulation_wait_ns(5000);
    }

    hw_status("Exiting %s\n", __func__);
}

// --------------------------------------
// mtc_write_training_basic_test_no_enable() W R
// --------------------------------------
void mtc_write_training_basic_test_no_enable(uint32_t sample_cnt, uint32_t read_wr_num,uint32_t adj_step,uint32_t pass_criteria, uint32_t mov_window,
      uint32_t byte_cnt) {
  uint32_t rd_val;
  uint32_t expect_val;

    hw_status("mtc_test: Entering %s\n", __func__);
    hw_status("mtc_test:  sample_cnt = %xH \n",sample_cnt);
    hw_status("mtc_test:  read_wr_num = %xH \n",read_wr_num);
    hw_status("mtc_test:  adj_step = %xH \n",adj_step);
    hw_status("mtc_test:  pass_criteria = %xH \n",pass_criteria);
    hw_status("mtc_test:  mov_window = %xH \n",mov_window);
    hw_status("mtc_test:  byte_cnt = %xH \n",byte_cnt);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_CNT_ADDRESS, sample_cnt);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_READ_WRITE_NUM_ADDRESS, read_wr_num);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_BREAK_ADDRESS, 0);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MOV_WINDOW_ADDRESS, mov_window << DDC_REGS_MOV_WINDOW_WINDOW_WR_LSB);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_ADJ_STEP_ADDRESS, adj_step);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_BYTE_CNT_ADDRESS, byte_cnt);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_PASS_CRITERIA_ADDRESS, pass_criteria);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);

   if ( random_write_data == 0 ) {
       // Write data patterns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_0_ADDRESS, 0x55555555);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_1_ADDRESS, 0xAAAAAAAA);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_2_ADDRESS, 0xFFFFFFFF);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_3_ADDRESS, 0x00000000);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_4_ADDRESS, 0x11111111);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_5_ADDRESS, 0x22222222);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_6_ADDRESS, 0x33333333);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_7_ADDRESS, 0x44444444);
       // write mask for above 32 bytes 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, 0xFFFFFFFF);

   } else {

     // random data patterns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_0_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_1_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_2_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_3_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_4_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_5_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_6_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_7_ADDRESS, rand());
       // write mask for above 32 bytes 
       //hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, 0xFFFFFFFF);
   }

    hw_status("mtc_test: Exiting %s\n", __func__);
}

// --------------------------------------
// mtc_enable_for_write_training_basic_test() W R
// --------------------------------------
void mtc_enable_for_write_training_basic_test() {
  uint32_t rd_val;
  uint32_t expect_val;

    hw_status("mtc_test: Entering %s\n", __func__);

    // enable write training order for training 0 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x3);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);

    hw_status("mtc_test: mtc_write_training_basic_test_no_polling: Enable Write Training\n");
    hw_status("mtc_test: Exiting %s\n", __func__);
}


// --------------------------------------
// mtc_write_training_basic_test_no_polling() W R
// --------------------------------------
void mtc_write_training_basic_test_no_polling(uint32_t sample_cnt, uint32_t read_wr_num,uint32_t adj_step,uint32_t pass_criteria, uint32_t mov_window,
      uint32_t byte_cnt) {
  uint32_t rd_val;
  uint32_t expect_val;

    hw_status("mtc_test: Entering %s\n", __func__);
    hw_status("mtc_test:  sample_cnt = %xH \n",sample_cnt);
    hw_status("mtc_test:  read_wr_num = %xH \n",read_wr_num);
    hw_status("mtc_test:  adj_step = %xH \n",adj_step);
    hw_status("mtc_test:  pass_criteria = %xH \n",pass_criteria);
    hw_status("mtc_test:  mov_window = %xH \n",mov_window);
    hw_status("mtc_test:  byte_cnt = %xH \n",byte_cnt);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_CNT_ADDRESS, sample_cnt);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_READ_WRITE_NUM_ADDRESS, read_wr_num);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_BREAK_ADDRESS, 0);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MOV_WINDOW_ADDRESS, mov_window << DDC_REGS_MOV_WINDOW_WINDOW_WR_LSB);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_ADJ_STEP_ADDRESS, adj_step);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_BYTE_CNT_ADDRESS, byte_cnt);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_PASS_CRITERIA_ADDRESS, pass_criteria);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);

   if ( random_write_data == 0 ) {
       // Write data patterns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_0_ADDRESS, 0x55555555);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_1_ADDRESS, 0xAAAAAAAA);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_2_ADDRESS, 0xFFFFFFFF);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_3_ADDRESS, 0x00000000);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_4_ADDRESS, 0x11111111);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_5_ADDRESS, 0x22222222);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_6_ADDRESS, 0x33333333);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_7_ADDRESS, 0x44444444);
       // write mask for above 32 bytes 
       // sugopala: commented out to use default mask pattern setup by init.
       //hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, 0xFFFFFFFF);

   } else {

     // random data patterns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_0_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_1_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_2_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_3_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_4_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_5_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_6_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_7_ADDRESS, rand());
       // write mask for above 32 bytes 
       //sugopala: enabled rand mask rather than all-f mask.
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, rand());
       //hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, 0xFFFFFFFF);
   }

    if ( dont_program_training_regs_for_write == 0 ) {

       // enable write training order for training 0 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x3);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);

       hw_status("mtc_test: mtc_write_training_basic_test_no_polling: Enable Write Training\n");

       //sugopala: TODO: FIXME: Check if this is really needed
       //enable_auto_refresh_with_alignment();

       //poll_status_reg_for_write_training_completion(/*global_hold_ctrl*/0);

       //if ( ! DDC_REGS_TRAINING_STATUS_WR_FAIL_GET(last_rd_val)) {
       //      hw_status("mtc_write_training_basic_test_no_polling: MTC WRITE TRAINING COMPLETED!!! regval =%xH\n",rd_val);
       //} 

       // Disable all training 
       //hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);


       //check_mtc_training_status_bits();

       // Dont allow any activity while training 
       //simulation_wait_ns(5000);
    }

    hw_status("mtc_test: Exiting %s\n", __func__);
}


// -----------------------------------------------
// mtc_write_training_sample_break_enabled() 
// -----------------------------------------------
void mtc_write_training_sample_break_enabled(uint32_t sample_cnt, uint32_t read_wr_num,uint32_t adj_step,uint32_t pass_criteria, uint32_t mov_window,
      uint32_t byte_cnt) {
  uint32_t rd_val;
  uint32_t expect_val;

    hw_status("Entering %s\n", __func__);
    hw_status(" sample_cnt = %xH \n",sample_cnt);
    hw_status(" read_wr_num = %xH \n",read_wr_num);
    hw_status(" adj_step = %xH \n",adj_step);
    hw_status(" pass_criteria = %xH \n",pass_criteria);
    hw_status(" mov_window = %xH \n",mov_window);
    hw_status(" byte_cnt = %xH \n",byte_cnt);

    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_CNT_ADDRESS, sample_cnt);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_READ_WRITE_NUM_ADDRESS, read_wr_num);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_BREAK_ADDRESS, 1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MOV_WINDOW_ADDRESS, mov_window << DDC_REGS_MOV_WINDOW_WINDOW_WR_LSB);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_ADJ_STEP_ADDRESS, adj_step);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_BYTE_CNT_ADDRESS, byte_cnt);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_PASS_CRITERIA_ADDRESS, pass_criteria);

   if ( random_write_data == 0 ) {
       // Write data patterns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_0_ADDRESS, 0x55555555);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_1_ADDRESS, 0xAAAAAAAA);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_2_ADDRESS, 0xFFFFFFFF);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_3_ADDRESS, 0x00000000);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_4_ADDRESS, 0x11111111);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_5_ADDRESS, 0x22222222);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_6_ADDRESS, 0x33333333);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_7_ADDRESS, 0x44444444);
       // write mask for above 32 bytes 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, 0xFFFFFFFF);

   } else {

     // random data patterns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_0_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_1_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_2_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_3_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_4_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_5_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_6_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_7_ADDRESS, rand());
       // write mask for above 32 bytes 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, rand());
   }

    // enable write training order for training 0 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x3);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);

    hw_status("mtc_write_training_sample_break_enabled: Enable Write Training");

    enable_auto_refresh_with_alignment();

    poll_status_reg_for_write_training_completion(/*global_hold_ctrl*/0);

   if ( ! DDC_REGS_TRAINING_STATUS_WR_FAIL_GET(last_rd_val)) {
             hw_status("mtc_write_training_sample_break_enabled: MTC WRITE TRAINING COMPLETED!!! regval =%xH\n",rd_val);
   }

    // Disable all training 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);

    check_mtc_training_status_bits();

    // Dont allow any activity while training 
    simulation_wait_ns(5000);

    hw_status("Exiting %s\n", __func__);
}


// --------------------------------------------------
// poll_status_reg_for_read_training_completion()
// --------------------------------------------------
void poll_status_reg_for_read_training_completion(const int global_hold_ctrl) 
{

    uint32_t rd_val;
    uint32_t expect_val, expect_mask;
    int display_flag=0;

    // ------------ poll till design signals start of training -------------------
    rd_val = 0;
    //expect_val = DDC_REGS_TRAINING_STATUS_BUSY_SET(1) | DDC_REGS_TRAINING_STATUS_TRAINING_IN_PROGRESS_SET(4) |
    //       DDC_REGS_TRAINING_STATUS_UNFINISHED_RD_TRAINING_SET(1);
    expect_val = DDC_REGS_TRAINING_STATUS_BUSY_SET(0) | DDC_REGS_TRAINING_STATUS_UNFINISHED_RD_TRAINING_SET(1);

    while ((rd_val & DDC_REGS_TRAINING_STATUS_UNFINISHED_RD_TRAINING_FIELD_MASK)  != expect_val) 
    { 
        rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
        last_rd_val = rd_val; //sugopala-added
        if ( display_flag == 0 ) 
        {
            hw_status("%s: mtc_test: %s:  polling for busy training status regval = 0x%x expect_val = 0x%x\n",
                get_sim_time(), __func__, rd_val,expect_val);
            display_flag= 1;
        } 
        else 
        {
            hw_status(".");
        }

        if ( DDC_REGS_TRAINING_STATUS_RD_FAIL_GET(rd_val)) 
        {
            hw_status("\n%s: mtc_test: %s: WARNING : MTC RD TRAINING FAIL STATUS !! Not clearing at this time; regval = 0x%x\n", get_sim_time(), __func__, rd_val);
            // clear status fail bit 
            // sugopala:  Commented this clearing
            //hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS,  1 << DDC_REGS_TRAINING_STATUS_RD_FAIL_MSB);
            //last_rd_val = rd_val;
            break;
        }
    }

    if (global_hold_ctrl == 1) 
    {
        hw_status("%s: mtc_test: %s: Releasing global hold\n", get_sim_time(), __func__);
        release_global_hold_seq(0, 1, "sch");
    }

    // -------------- poll for status completion of training -------------------
    rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
    //sugopala: updated this status expectation
    expect_val =  DDC_REGS_TRAINING_STATUS_BUSY_SET(1) |  DDC_REGS_TRAINING_STATUS_TRAINING_IN_PROGRESS_SET(0) |
                  DDC_REGS_TRAINING_STATUS_UNFINISHED_RD_TRAINING_SET(0);
    expect_mask = DDC_REGS_TRAINING_STATUS_BUSY_FIELD_MASK | DDC_REGS_TRAINING_STATUS_TRAINING_IN_PROGRESS_FIELD_MASK |
                  DDC_REGS_TRAINING_STATUS_UNFINISHED_RD_TRAINING_FIELD_MASK;

    //hw_status("mtc_test: %s:  read training status regval = 0x%x expect_val = 0x%x\n",
    //    __func__, rd_val, expect_val);

    //last_rd_val = rd_val;
    display_flag = 0;
    //while ((rd_val & DDC_REGS_TRAINING_STATUS_UNFINISHED_RD_TRAINING_FIELD_MASK) != expect_val) 
    do
    { 
        rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
        last_rd_val = rd_val;
        if ( display_flag == 0 ) 
        {
            hw_status("%s: mtc_test: %s:  polling for done training status regval = 0x%x; expect_val = 0x%x; expect_mask = 0x%x\n",
                get_sim_time(), __func__, rd_val, expect_val, expect_mask);
            display_flag = 1;
        } 
        else 
        {
            hw_status("%");
        }

    }
    while ((rd_val & expect_mask) != expect_val);

    // check read training fail status 
    if (DDC_REGS_TRAINING_STATUS_RD_FAIL_GET(rd_val)) 
    {
         hw_status("\n%s: mtc_test: %s: WARNING : MTC RD TRAINING FAIL STATUS  !! Clearing this time; regval = 0x%x; saving to last_rd_val = 0x%x\n", get_sim_time(), __func__, rd_val, last_rd_val);
         // clear status fail bit 
         hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS,  1 << DDC_REGS_TRAINING_STATUS_RD_FAIL_MSB);
         //break;
    }

    hw_status("%s: mtc_test: %s: exiting function\n", get_sim_time(), __func__);
}

// --------------------------------------------------
// poll_status_reg_for_read_training_completion_phy_based()
// --------------------------------------------------
void poll_status_reg_for_read_training_completion_phy_based() {

  uint32_t rd_val;
  uint32_t expect_val;
  int display_flag=0;

    // ------------ poll till design signals start of training -------------------
    rd_val = 0;
    //expect_val = DDC_REGS_TRAINING_STATUS_BUSY_SET(1) | DDC_REGS_TRAINING_STATUS_TRAINING_IN_PROGRESS_SET(4) |
    //       DDC_REGS_TRAINING_STATUS_UNFINISHED_RD_TRAINING_SET(0);
    expect_val = DDC_REGS_TRAINING_STATUS_BUSY_SET(0) ;

    while (rd_val != expect_val) { 
         rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);

         if ( display_flag == 0 ) {
           hw_status("poll_status_reg_for_read_training_completion_phy_based:  polling for busy training status regval = %xH expect_val = %xH\n",
               rd_val,expect_val);
           display_flag= 1;
         } else {
            hw_status(".");
         }

          if ( DDC_REGS_TRAINING_STATUS_RD_FAIL_GET(rd_val)) {
             hw_status("\npoll_status_reg_for_read_training_completion_phy_based: WARNING : MTC RD TRAINING FAIL STATUS  !! regval =%xH\n",rd_val);
             // clear status fail bit 
             hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS,  1 << DDC_REGS_TRAINING_STATUS_RD_FAIL_MSB);
             last_rd_val = rd_val;
             break;
          }
    }

    // -------------- poll for status completion of training -------------------
    rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
     // expect_val =  DDC_REGS_TRAINING_STATUS_BUSY_SET(0) |  DDC_REGS_TRAINING_STATUS_TRAINING_IN_PROGRESS_SET(0);
      expect_val =  DDC_REGS_TRAINING_STATUS_BUSY_SET(1);

    hw_status("poll_status_reg_for_read_training_completion_phy_based:  read training status regval = %xH expect_val = %xH\n",
           rd_val,expect_val);

    display_flag = 0;
    while (rd_val != expect_val) { 

         rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
         if ( display_flag == 0 ) {
            hw_status("poll_status_reg_for_read_training_completion_phy_based:  polling for done training status regval = %xH expect_val = %xH\n",
              rd_val,expect_val);
            display_flag = 1;
         } else {
            hw_status("%");
         }

           // check phyupdate training fail status 
          if ( DDC_REGS_TRAINING_STATUS_RD_FAIL_GET(rd_val)) {
             hw_status("\npoll_status_reg_for_read_training_completion_phy_based: WARNING : MTC RD TRAINING FAIL STATUS  !! regval =%xH\n",rd_val);
             // clear status fail bit 
             hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS,  1 << DDC_REGS_TRAINING_STATUS_RD_FAIL_MSB);
             last_rd_val = rd_val;
             break;
          }
     }
}

// ---------------------------------------
// mtc_read_training_basic_test() R D
// ---------------------------------------
void mtc_read_training_basic_test(uint32_t sample_cnt, uint32_t read_wr_num,uint32_t adj_step,uint32_t pass_criteria, uint32_t mov_window,
      uint32_t byte_cnt) {
  uint32_t rd_val;
  uint32_t expect_val;


    hw_status("mtc_test: Entering %s\n", __func__);
    hw_status("mtc_test:  sample_cnt = %xH \n",sample_cnt);
    hw_status("mtc_test:  read_wr_num = %xH \n",read_wr_num);
    hw_status("mtc_test:  adj_step = %xH \n",adj_step);
    hw_status("mtc_test:  pass_criteria = %xH \n",pass_criteria);
    hw_status("mtc_test:  mov_window = %xH \n",mov_window);
    hw_status("mtc_test:  byte_cnt = %xH \n",byte_cnt);

    if ( dont_program_training_regs_for_read == 0) {
        //sugopala: Moved from after training-en to before training-en
        enable_auto_refresh_with_alignment();
    }

   if ( (dont_program_training_regs_for_read == 0) && (dont_program_training_regs_for_write == 0)) {
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_CNT_ADDRESS, sample_cnt);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_READ_WRITE_NUM_ADDRESS, read_wr_num);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_BREAK_ADDRESS, 0);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MOV_WINDOW_ADDRESS, mov_window << DDC_REGS_MOV_WINDOW_WINDOW_RD_LSB);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_ADJ_STEP_ADDRESS, adj_step);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_BYTE_CNT_ADDRESS, byte_cnt);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_PASS_CRITERIA_ADDRESS, pass_criteria);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);

     if ( random_write_data == 0 ) {
       // Write data patterns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_0_ADDRESS, 0x55555555);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_1_ADDRESS, 0xAAAAAAAA);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_2_ADDRESS, 0xFFFFFFFF);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_3_ADDRESS, 0x00000000);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_4_ADDRESS, 0x11111111);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_5_ADDRESS, 0x22222222);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_6_ADDRESS, 0x33333333);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_7_ADDRESS, 0x44444444);
       // write mask for above 32 bytes 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, 0xFFFFFFFF);

     } else {

     // random data patterns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_0_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_1_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_2_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_3_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_4_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_5_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_6_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_7_ADDRESS, rand());
       // write mask for above 32 bytes 
       //hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, 0xFFFFFFFF);
     }

    // enable read training order for training 0 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x2);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);
   }


  if ( dont_program_training_regs_for_read == 0) {
    hw_status("mtc_test: mtc_read_training_basic_test: Enable Read Training\n");

    //sugopala: Commented this and moved before training-en
    //enable_auto_refresh_with_alignment();

    poll_status_reg_for_read_training_completion(/*global_hold_ctrl*/0);

    // ---- Debug only pass cnt registers 
    //rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_RD_WR_TR_RIGHT_PASS_CNT_ADDRESS);
    //hw_status("check_write_training_stats: RD_TR_RIGHT_PASS_CNT regval = %xH\n", rd_val);
    //rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_RD_WR_TR_LEFT_PASS_CNT_ADDRESS);
    //hw_status("check_write_training_stats: RD_TR_LEFT_PASS_CNT regval = %xH\n", rd_val);


    // Disable all training 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);

    if ( ! DDC_REGS_TRAINING_STATUS_RD_FAIL_GET(last_rd_val)) {
             hw_status("mtc_test: mtc_read_training_basic_test: MTC READ TRAINING COMPLETED!!! regval =%xH\n",last_rd_val);
    }

    check_mtc_training_status_bits(rd_val);

    // Dont allow any activity while training 
    simulation_wait_ns(5000);
   }

    hw_status("mtc_test: Exiting %s\n", __func__);
}

// ---------------------------------------
// mtc_read_training_basic_test_no_polling() R D
// ---------------------------------------
void mtc_read_training_basic_test_no_polling(uint32_t sample_cnt, uint32_t read_wr_num,uint32_t adj_step,uint32_t pass_criteria, uint32_t mov_window,
      uint32_t byte_cnt) {
  uint32_t rd_val;
  uint32_t expect_val;


    hw_status("Entering %s\n", __func__);
    hw_status(" sample_cnt = %xH \n",sample_cnt);
    hw_status(" read_wr_num = %xH \n",read_wr_num);
    hw_status(" adj_step = %xH \n",adj_step);
    hw_status(" pass_criteria = %xH \n",pass_criteria);
    hw_status(" mov_window = %xH \n",mov_window);
    hw_status(" byte_cnt = %xH \n",byte_cnt);

   if ( (dont_program_training_regs_for_read == 0) && (dont_program_training_regs_for_write == 0)) {
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_CNT_ADDRESS, sample_cnt);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_READ_WRITE_NUM_ADDRESS, read_wr_num);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_BREAK_ADDRESS, 0);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MOV_WINDOW_ADDRESS, mov_window << DDC_REGS_MOV_WINDOW_WINDOW_RD_LSB);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_ADJ_STEP_ADDRESS, adj_step);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_BYTE_CNT_ADDRESS, byte_cnt);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_PASS_CRITERIA_ADDRESS, pass_criteria);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);

     if ( random_write_data == 0 ) {
       // Write data patterns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_0_ADDRESS, 0x55555555);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_1_ADDRESS, 0xAAAAAAAA);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_2_ADDRESS, 0xFFFFFFFF);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_3_ADDRESS, 0x00000000);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_4_ADDRESS, 0x11111111);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_5_ADDRESS, 0x22222222);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_6_ADDRESS, 0x33333333);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_7_ADDRESS, 0x44444444);
       // write mask for above 32 bytes 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, 0xFFFFFFFF);

     } else {

     // random data patterns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_0_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_1_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_2_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_3_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_4_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_5_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_6_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_7_ADDRESS, rand());
       // write mask for above 32 bytes 
       //hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, 0xFFFFFFFF);
     }

    // enable read training order for training 0 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x2);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);
   }

  if ( dont_program_training_regs_for_read == 0) {
    hw_status("mtc_read_training_basic_test_no_polling: Enable Read Training\n");

    enable_auto_refresh_with_alignment();

    //poll_status_reg_for_read_training_completion(/*global_hold_ctrl*/0);

    // ---- Debug only pass cnt registers 
    //rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_RD_WR_TR_RIGHT_PASS_CNT_ADDRESS);
    //hw_status("check_write_training_stats: RD_TR_RIGHT_PASS_CNT regval = %xH\n", rd_val);
    //rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_RD_WR_TR_LEFT_PASS_CNT_ADDRESS);
    //hw_status("check_write_training_stats: RD_TR_LEFT_PASS_CNT regval = %xH\n", rd_val);


    // Disable all training 
    //hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);

    //if ( ! DDC_REGS_TRAINING_STATUS_RD_FAIL_GET(last_rd_val)) {
             hw_status("mtc_read_training_basic_test_no_polling: MTC READ TRAINING COMPLETED!!! regval =%xH\n",last_rd_val);
    //}

    //check_mtc_training_status_bits(rd_val);

    // Dont allow any activity while training 
    //simulation_wait_ns(5000);
   }

    hw_status("Exiting %s\n", __func__);
}

// ---------------------------------------
// mtc_read_training_basic_test_phy_based() R D
// ---------------------------------------
void mtc_read_training_basic_test_phy_based(uint32_t sample_cnt, uint32_t read_wr_num,uint32_t adj_step,uint32_t pass_criteria, uint32_t mov_window,
      uint32_t byte_cnt) {
  uint32_t rd_val;
  uint32_t expect_val;


    hw_status("Entering %s\n", __func__);
    hw_status(" sample_cnt = %xH \n",sample_cnt);
    hw_status(" read_wr_num = %xH \n",read_wr_num);
    hw_status(" adj_step = %xH \n",adj_step);
    hw_status(" pass_criteria = %xH \n",pass_criteria);
    hw_status(" mov_window = %xH \n",mov_window);
    hw_status(" byte_cnt = %xH \n",byte_cnt);

   if ( (dont_program_training_regs_for_read == 0) && (dont_program_training_regs_for_write == 0)) {
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_CNT_ADDRESS, sample_cnt);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_READ_WRITE_NUM_ADDRESS, read_wr_num);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_BREAK_ADDRESS, 0);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MOV_WINDOW_ADDRESS, mov_window << DDC_REGS_MOV_WINDOW_WINDOW_RD_LSB);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_ADJ_STEP_ADDRESS, adj_step);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_BYTE_CNT_ADDRESS, byte_cnt);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_PASS_CRITERIA_ADDRESS, pass_criteria);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);

     if ( random_write_data == 0 ) {
       // Write data patterns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_0_ADDRESS, 0x55555555);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_1_ADDRESS, 0xAAAAAAAA);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_2_ADDRESS, 0xFFFFFFFF);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_3_ADDRESS, 0x00000000);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_4_ADDRESS, 0x11111111);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_5_ADDRESS, 0x22222222);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_6_ADDRESS, 0x33333333);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_7_ADDRESS, 0x44444444);
       // write mask for above 32 bytes 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, 0xFFFFFFFF);

     } else {

     // random data patterns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_0_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_1_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_2_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_3_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_4_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_5_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_6_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_7_ADDRESS, rand());
       // write mask for above 32 bytes 
       //hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, 0xFFFFFFFF);
     }

    // enable read training order for training 0 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x2);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);
   }

  if ( dont_program_training_regs_for_read == 0) {
    hw_status("mtc_read_training_basic_test_phy_based: Enable PHY basd Read Training\n");

    enable_auto_refresh_with_alignment();

    poll_status_reg_for_read_training_completion_phy_based();

    // ---- Debug only pass cnt registers 
    //rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_RD_WR_TR_RIGHT_PASS_CNT_ADDRESS);
    //hw_status("check_write_training_stats: RD_TR_RIGHT_PASS_CNT regval = %xH\n", rd_val);
    //rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_RD_WR_TR_LEFT_PASS_CNT_ADDRESS);
    //hw_status("check_write_training_stats: RD_TR_LEFT_PASS_CNT regval = %xH\n", rd_val);


    // Disable all training 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);

    if ( ! DDC_REGS_TRAINING_STATUS_RD_FAIL_GET(last_rd_val)) {
             hw_status("mtc_read_training_basic_test_phy_based: MTC READ TRAINING COMPLETED!!! regval =%xH\n",last_rd_val);
    }

    check_mtc_training_status_bits(rd_val);

    // Dont allow any activity while training 
    simulation_wait_ns(5000);
   }

    hw_status("Exiting %s\n", __func__);
}
// ---------------------------------------
// mtc_read_training_basic_test_phy_based_no_polling() R D
// ---------------------------------------
void mtc_read_training_basic_test_phy_based_no_polling(uint32_t sample_cnt, uint32_t read_wr_num,uint32_t adj_step,uint32_t pass_criteria, uint32_t mov_window,
      uint32_t byte_cnt) {
  uint32_t rd_val;
  uint32_t expect_val;


    hw_status("Entering %s\n", __func__);
    hw_status(" sample_cnt = %xH \n",sample_cnt);
    hw_status(" read_wr_num = %xH \n",read_wr_num);
    hw_status(" adj_step = %xH \n",adj_step);
    hw_status(" pass_criteria = %xH \n",pass_criteria);
    hw_status(" mov_window = %xH \n",mov_window);
    hw_status(" byte_cnt = %xH \n",byte_cnt);

   if ( (dont_program_training_regs_for_read == 0) && (dont_program_training_regs_for_write == 0)) {
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_CNT_ADDRESS, sample_cnt);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_READ_WRITE_NUM_ADDRESS, read_wr_num);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_BREAK_ADDRESS, 0);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MOV_WINDOW_ADDRESS, mov_window << DDC_REGS_MOV_WINDOW_WINDOW_RD_LSB);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_ADJ_STEP_ADDRESS, adj_step);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_BYTE_CNT_ADDRESS, byte_cnt);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_PASS_CRITERIA_ADDRESS, pass_criteria);
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);

     if ( random_write_data == 0 ) {
       // Write data patterns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_0_ADDRESS, 0x55555555);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_1_ADDRESS, 0xAAAAAAAA);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_2_ADDRESS, 0xFFFFFFFF);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_3_ADDRESS, 0x00000000);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_4_ADDRESS, 0x11111111);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_5_ADDRESS, 0x22222222);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_6_ADDRESS, 0x33333333);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_7_ADDRESS, 0x44444444);
       // write mask for above 32 bytes 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, 0xFFFFFFFF);

     } else {

     // random data patterns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_0_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_1_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_2_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_3_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_4_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_5_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_6_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_7_ADDRESS, rand());
       // write mask for above 32 bytes 
       //hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, 0xFFFFFFFF);
     }

    // enable read training order for training 0 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x2);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);
   }

  if ( dont_program_training_regs_for_read == 0) {
    hw_status("mtc_read_training_basic_test_phy_based_no_polling: Enable PHY based Read Training\n");

    enable_auto_refresh_with_alignment();

   }
    hw_status("Exiting %s\n", __func__);
}

// ---------------------------------------
// mtc_read_training_sample_break_enabled() 
// ---------------------------------------
void mtc_read_training_sample_break_enabled(uint32_t sample_cnt, uint32_t read_wr_num,uint32_t adj_step,uint32_t pass_criteria, uint32_t mov_window,
      uint32_t byte_cnt) {
  uint32_t rd_val;
  uint32_t expect_val;

    hw_status("Entering %s\n", __func__);
    hw_status(" sample_cnt = %xH \n",sample_cnt);
    hw_status(" read_wr_num = %xH \n",read_wr_num);
    hw_status(" adj_step = %xH \n",adj_step);
    hw_status(" pass_criteria = %xH \n",pass_criteria);
    hw_status(" mov_window = %xH \n",mov_window);
    hw_status(" byte_cnt = %xH \n",byte_cnt);

    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_CNT_ADDRESS, sample_cnt);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_READ_WRITE_NUM_ADDRESS, read_wr_num);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_BREAK_ADDRESS, 1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MOV_WINDOW_ADDRESS, mov_window << DDC_REGS_MOV_WINDOW_WINDOW_RD_LSB);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_ADJ_STEP_ADDRESS, adj_step);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_BYTE_CNT_ADDRESS, byte_cnt);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_PASS_CRITERIA_ADDRESS, pass_criteria);

   if ( random_write_data == 0 ) {
       // Write data patterns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_0_ADDRESS, 0x55555555);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_1_ADDRESS, 0xAAAAAAAA);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_2_ADDRESS, 0xFFFFFFFF);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_3_ADDRESS, 0x00000000);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_4_ADDRESS, 0x11111111);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_5_ADDRESS, 0x22222222);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_6_ADDRESS, 0x33333333);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_7_ADDRESS, 0x44444444);
       // write mask for above 32 bytes 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, 0xFFFFFFFF);

   } else {

     // random data patterns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_0_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_1_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_2_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_3_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_4_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_5_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_6_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_7_ADDRESS, rand());
       // write mask for above 32 bytes 
       //hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, rand());
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, 0xFFFFFFFF);
   }

    // enable read training order for training 0 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x2);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);

    hw_status("mtc_read_training_sample_break_enabled: Enable Read Training\n");

    enable_auto_refresh_with_alignment();

    poll_status_reg_for_read_training_completion(/*global_hold_ctrl*/0);

    // Disable all training 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);

    if ( ! DDC_REGS_TRAINING_STATUS_RD_FAIL_GET(last_rd_val)) {
             hw_status("mtc_read_training_sample_break_enabled: MTC READ TRAINING COMPLETED!!! regval =%xH\n",rd_val);
    }


    check_mtc_training_status_bits(rd_val);

    // Dont allow any activity while training 
    simulation_wait_ns(5000);

    hw_status("Exiting %s\n", __func__);
}

// ---------------------------------------
// mtc_zqcal_training_test_lpddr4()  ZQ4 
// ---------------------------------------
void mtc_zqcal_training_test_lpddr4(int break_type,int zqcal_mode) {
  uint32_t rd_val;
  uint32_t expect_val;

    hw_status("Entering %s break_type - %xH, mode - %xH\n", __func__,break_type,zqcal_mode);

     // enable  zq cal 
    /// Enable LPDDR4 ZQ Cal 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_ZQ_CAL_MODE_ADDRESS, break_type << 2 | zqcal_mode);  // lpddr4 + break = rank one/rank_all enabled 

    
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MPC_VALUE1_ADDRESS, 0x51); // from lpddr4 jedec spec
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MPC_VALUE2_ADDRESS, 0x4f); // from lpddr4 jedec spec
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT1_ADDRESS, 30/1.25); // from lpddr4 jedec spec  1us/2.5ns
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT2_ADDRESS, 0); // from lpddr4 jedec spec 30ns/2.5ns

       hw_status("mtc_zqcal_training_test_lpddr4: Enable MTC ZQ Cal MPC Training\n");
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x1);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);

       enable_auto_refresh_with_alignment();
   
       simulation_wait_ns(90); // wait for tzcl 90 ns min time 

       // rank 0 polling 
       poll_status_reg_for_zqcal_training_completion();

       // rank 1 polling 
       poll_status_reg_for_zqcal_training_completion();
       
      // Disable all training
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);

      check_mtc_training_status_bits();

      // wait for tzqinit min time 1us 
      // Dont allow any activity
      //simulation_tb_wait(10000,"sch");


    hw_status("Exiting %s\n", __func__);

}

// ------------------------------------------------------
// mtc_zqcal_training_test_lpddr4_new_requirement()  ZQ4 
// ------------------------------------------------------
void mtc_zqcal_training_test_lpddr4_new_requirement(int break_type,int zqcal_mode) {
  uint32_t rd_val;
  uint32_t expect_val;

    hw_status("Entering %s break_type - %xH, mode - %xH\n", __func__,break_type,zqcal_mode);

     // enable  zq cal 
    /// Enable LPDDR4 ZQ Cal 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_ZQ_CAL_MODE_ADDRESS, break_type << 2 | zqcal_mode);  // lpddr4 + break = rank one/rank_all enabled 

    
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MPC_VALUE1_ADDRESS, 0x51); // from lpddr4 jedec spec
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MPC_VALUE2_ADDRESS, 0x4f); // from lpddr4 jedec spec
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT1_ADDRESS, 30/1.25); // from lpddr4 jedec spec ZQLAT
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT2_ADDRESS, 0); 

       hw_status("mtc_zqcal_training_test_lpddr4_new_requirement: Enable MTC ZQ Cal MPC Training\n");
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x1);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);

       enable_auto_refresh_with_alignment();
   
       simulation_wait_ns(90); // wait for tzcl 90 ns min time 

       // rank 0 polling 
       poll_status_reg_for_zqcal_training_completion();

       // rank 1 polling 
       poll_status_reg_for_zqcal_training_completion();
       
      // Disable all training
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);

      check_mtc_training_status_bits();

      // wait for tzqinit min time 1us 
      // Dont allow any activity
      //simulation_tb_wait(10000,"sch");


    hw_status("Exiting %s\n", __func__);

}
// ---------------------------------------
// mtc_zqcal_training_test_lpddr4_rank0_only()  ZQ4 
// ---------------------------------------
void mtc_zqcal_training_test_lpddr4_rank0_only(int break_type,int zqcal_mode) {
  uint32_t rd_val;
  uint32_t expect_val;

    hw_status("Entering %s break_type - %xH, mode - %xH\n", __func__,break_type,zqcal_mode);

     // enable  zq cal 
    /// Enable LPDDR4 ZQ Cal 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_ZQ_CAL_MODE_ADDRESS, break_type << 2 | zqcal_mode);  // lpddr4 + break = rank one/rank_all enabled 

    
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MPC_VALUE1_ADDRESS, 0x51); // from lpddr4 jedec spec
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MPC_VALUE2_ADDRESS, 0x4f); // from lpddr4 jedec spec
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT1_ADDRESS, 30/1.25); // from lpddr4 jedec spec  1us/2.5ns
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT2_ADDRESS, 0); // from lpddr4 jedec spec 30ns/2.5ns

       hw_status("mtc_zqcal_training_test_lpddr4_rank0_only: Enable MTC ZQ Cal MPC Training\n");
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x1);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);

       enable_auto_refresh_with_alignment();
   
       simulation_wait_ns(90); // wait for tzcl 90 ns min time 

       // rank 0 polling 
       poll_status_reg_for_zqcal_training_completion();

       // rank 1 polling 
      // poll_status_reg_for_zqcal_training_completion();
       
      // Disable all training
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);

      check_mtc_training_status_bits();

      // wait for tzqinit min time 1us 
      // Dont allow any activity
      //simulation_tb_wait(10000,"sch");


    hw_status("Exiting %s\n", __func__);

}


// --------------------------------------------
// C O M B 0   T R A I N I N G  T E S T S 
// --------------------------------------------
// ------------------------------------------------------
// mtc_phyupdate_zqcal_combo_training_test_lpddr3()  ZQ 
// ------------------------------------------------------
// cal_type = 0 (short) or 1 (long), zqcal init(2)
void mtc_phyupdate_zqcal_combo_training_test_lpddr3(int cal_type) {
  uint32_t rd_val;
  uint32_t expect_val;

    hw_status("Entering %s cal_type - %xH\n", __func__,cal_type);

    // Setup parameters for PHY update test
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_CNT_ADDRESS, 1);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_DFI_TP_CTRLUPD_MAX_ADDRESS, 200);

     // enable  zq cal 
    /// Enable LPDDR3 ZQ Cal 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_ZQ_CAL_MODE_ADDRESS, 0x1);

    // ---- MR10 Write for ZQ Cal for LPDDR3 ---
    // 0xFF: Calibration command after initialization 
    // 0xAB: Long calibration
    // 0x56: Short calibration
    // 0xC3: ZQ Reset
 
    if ( cal_type == 1) {  // Long training 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MR_VALUE_ADDRESS, 0xAB);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT1_ADDRESS, 144);   // 360 ns / 2.5 ns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MR_ADDR_ADDRESS, 0xa);
       //simulation_wait_ns(1000); // wait for tzqinit min time 1us 

       hw_status("mtc_zqcal_training_test_lpddr3: Enable MTC ZQ Cal Long Training\n");
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x1);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);

       enable_auto_refresh_with_alignment();

    }
    else if(cal_type == 0) {  // Short training 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MR_VALUE_ADDRESS, 0x56);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT1_ADDRESS, 36); // 90ns / 2.5 ns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MR_ADDR_ADDRESS, 0xa);

       hw_status("mtc_zqcal_training_test_lpddr3: Enable PHY update + MTC ZQ Cal Short Training\n");
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x4);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x3);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR1_FREQUENCY_ADDRESS, trn_frq);

       enable_auto_refresh_with_alignment();

       
    } else if ( cal_type == 2) { // ZQ cal after init 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MR_VALUE_ADDRESS, 0xFF);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT1_ADDRESS, 400); // 1us / 2.5 ns 
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MR_ADDR_ADDRESS, 0xa);

       hw_status("mtc_zqcal_training_test_lpddr3: Enable MTC ZQ Cal Long Training\n");
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x1);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x1);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, trn_frq);

       enable_auto_refresh_with_alignment();

    }

      // ------------ poll till design signals start of phy update training -------------------
      rd_val = 0;
      expect_val = DDC_REGS_TRAINING_STATUS_BUSY_SET(1) | DDC_REGS_TRAINING_STATUS_TRAINING_IN_PROGRESS_SET(1);

      while (rd_val != expect_val) { 
         rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
         hw_status("mtc_zqcal_training_test_lpddr3:  polling for busy training status regval = %xH expect_val = %xH\n",
           rd_val,expect_val);
      }

      // -------------- poll for status completion of training -------------------
      rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
      expect_val =  DDC_REGS_TRAINING_STATUS_BUSY_SET(0) |  DDC_REGS_TRAINING_STATUS_TRAINING_IN_PROGRESS_SET(0);

      hw_status("mtc_zqcal_training_test_lpddr3:  zqcal training status regval = %xH expect_val = %xH\n",
           rd_val,expect_val);

      while (rd_val != expect_val) { 

         rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
         hw_status("mtc_zqcal_training_test_lpddr3:  polling for done training status regval = %xH expect_val = %xH\n",
           rd_val,expect_val);
           // check phy update and zqcal training fail status 

          if ( DDC_REGS_TRAINING_STATUS_PHYUPD_FAIL_GET(rd_val)) {
             hw_status("mtc_phyupdate_training_test: ERROR : MTC PHY UPDATE FAILED !! regval =%xH\n",rd_val);
             break;
          }

      }
      if ( ! DDC_REGS_TRAINING_STATUS_PHYUPD_FAIL_GET(rd_val)) {
             hw_status("mtc_phyupdate_training_test: MTC PHY UPDATE TRAINING COMPLETED!!! regval =%xH\n",rd_val);
      }

      // ------------ poll till design signals start of zq cal training -------------------
      rd_val = 0;
      expect_val = DDC_REGS_TRAINING_STATUS_BUSY_SET(1) | DDC_REGS_TRAINING_STATUS_TRAINING_IN_PROGRESS_SET(2);

      while (rd_val != expect_val) { 
         rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
         hw_status("mtc_zqcal_training_test_lpddr3:  polling for busy training status regval = %xH expect_val = %xH\n",
           rd_val,expect_val);
      }

      // -------------- poll for status completion of training -------------------
      rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
      expect_val =  DDC_REGS_TRAINING_STATUS_BUSY_SET(0) |  DDC_REGS_TRAINING_STATUS_TRAINING_IN_PROGRESS_SET(0);

      hw_status("mtc_zqcal_training_test_lpddr3:  zqcal training status regval = %xH expect_val = %xH\n",
           rd_val,expect_val);

      while (rd_val != expect_val) { 

         rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_TRAINING_STATUS_ADDRESS);
         hw_status("mtc_zqcal_training_test_lpddr3:  polling for done training status regval = %xH expect_val = %xH\n",
           rd_val,expect_val);
           // check phy update and zqcal training fail status 


          //if ( DDC_REGS_TRAINING_STATUS_ZQCAL_FAIL_GET(rd_val)) {
          //   hw_status("mtc_zqcal_training_test_lpddr3: ERROR : MTC ZQ CAL FAILED !! regval =%xH\n",rd_val);
          //   break;
          //}
      }

      //if ( ! DDC_REGS_TRAINING_STATUS_ZQCAL_FAIL_GET(rd_val)) {
      //      hw_status("mtc_zqcal_training_test_lpddr3: : MTC ZQ CAL COMPLETED!! regval =%xH\n",rd_val);
      //}

      // Disable all training 
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);

      // wait for tzqinit min time 1us 
      // Dont allow any activity
      simulation_tb_wait(10000,"sch");

    hw_status("Exiting %s\n", __func__);

}

// --------------------------------------------------------
// mtc_zqcal_wr_rd_phy_combo_training_test_lpddr4()  ZQ+WR+RD
// --------------------------------------------------------
void mtc_zqcal_wr_rd_phy_combo_training_test_lpddr4(int break_type,int zqcal_mode) {
  uint32_t rd_val;
  uint32_t expect_val;
  int i,initial_delay[NUM_SLICES],j;
  int mov_window;
  int write_val;

    hw_status("Entering %s break_type - %xH, mode - %xH\n", __func__,break_type,zqcal_mode);

     // enable  zq cal 
    /// Enable LPDDR4 ZQ Cal 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_ZQ_CAL_MODE_ADDRESS, break_type << 2 | zqcal_mode);  // lpddr4 + break = rank one/rank_all enabled 
    
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MPC_VALUE1_ADDRESS, 0x51); // from lpddr4 jedec spec
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MPC_VALUE2_ADDRESS, 0x4f); // from lpddr4 jedec spec
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT1_ADDRESS, 30/1.25); // from lpddr4 jedec spec  1us/2.5ns
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT2_ADDRESS, 0); // from lpddr4 jedec spec 50ns/2.5ns

       // ----- setup stage for write training  ------
#ifdef CONF_HAS___PHY_RTL
       initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+get_freq_based_delay(MVM_DELTA);
       initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+get_freq_based_delay(MVM_DELTA);
#else 
       initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+4;
       initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+4;
#endif 
       for (i=0;i < NUM_SLICES;i++) {
         set_phy_delay_write_reg(i,initial_delay[i],0); // rank 0 
       }
#ifdef CONF_HAS___PHY_RTL
       initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+MVM_DELTA+5;
       initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+MVM_DELTA+5;
       mov_window=get_freq_based_delay(MVM_DELTA);
#else 
       initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+4;
       initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+4;
       mov_window=2;
#endif 
       for (i=0;i < NUM_SLICES;i++) {
         set_phy_delay_write_reg(i,initial_delay[i],1); // rank 1 
       }
       dont_program_training_regs_for_write = 1;
       dont_program_training_regs_for_read = 1;
       //sample_cnt, read_wr_num,adj_step,pass_criteria, mov_window,byte_cnt
       mtc_write_training_basic_test(3,2,1,2,mov_window,1); // LPDDR4

       // ----- setup stage for read training  ------
       initial_delay[0] = get_freq_based_golden_read_delay()+5;
       initial_delay[1] = get_freq_based_golden_read_delay()+5;
       for (i=0;i < NUM_READ_DELAYS ;i++) {
            for (j=0;j < NUM_SLICES ;j++) {
              set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0); // rank0
              set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],1); // rank1 
            }
       }

       // Since dont_program_training_regs_for_write =1 , program the mov window correctly again 
       write_val = DDC_REGS_MOV_WINDOW_WINDOW_WR_SET(mov_window) | DDC_REGS_MOV_WINDOW_WINDOW_RD_SET(mov_window);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MOV_WINDOW_ADDRESS, write_val);

       mtc_read_training_basic_test(3,2,1,2,2,1); // LPDDR4

       // ------ setup for phy update ------------
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_DFI_TP_CTRLUPD_MAX_ADDRESS, 200);
       
       hw_status("mtc_zqcal_training_test_lpddr4: Enable MTC ZQ Cal MPC Training + Write Training + Read Training + PHY training \n");
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x2d);  // ZQCAL,Write,Read Training,PHY training  
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0xf);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, 3);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR1_FREQUENCY_ADDRESS, 3);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR2_FREQUENCY_ADDRESS, 3);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR3_FREQUENCY_ADDRESS, 3);

       enable_auto_refresh_with_alignment();
   
       simulation_wait_ns(90); // wait for tzcl 90 ns min time 

       // rank 0  polling 
       poll_status_reg_for_zqcal_training_completion();

       // rank 1 polling 
       poll_status_reg_for_zqcal_training_completion();

      ///  ---------------- Write training polling -----------------------

      // rank 0 polling 
      poll_status_reg_for_write_training_completion(/*global_hold_ctrl*/0);
      if ( ! DDC_REGS_TRAINING_STATUS_WR_FAIL_GET(last_rd_val)) {
             hw_status("mtc_write_training_basic_test: MTC WRITE TRAINING COMPLETED RANK 0!!! regval =%xH\n",rd_val);
      }


      // rank 1 polling 
      poll_status_reg_for_write_training_completion(/*global_hold_ctrl*/0);
      if ( ! DDC_REGS_TRAINING_STATUS_WR_FAIL_GET(last_rd_val)) {
             hw_status("mtc_write_training_basic_test: MTC WRITE TRAINING COMPLETED RANK 1!!! regval =%xH\n",rd_val);
      }

      ///  ---------------- Read training polling -----------------------

      // rank 0 polling 
      poll_status_reg_for_read_training_completion(/*global_hold_ctrl*/0);

      // rank 1 polling 
      poll_status_reg_for_read_training_completion(/*global_hold_ctrl*/0);


      // Start checking results 
#ifdef CONF_HAS___PHY_RTL
      initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+get_freq_based_delay(MVM_DELTA);
      initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+get_freq_based_delay(MVM_DELTA);
      mov_window=get_freq_based_delay(MVM_DELTA);
#else 
      initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+4;
      initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+4;
      mov_window = 2;
#endif 
      //sample_cnt, read_wr_num,adj_step,pass_criteria, mov_window,byte_cnt
      //mtc_write_training_basic_test(3,2,1,2,2,1); 

      // sample_cnt, initial_delay_val, mov_window,adj_step,pass_criteria,num_writes_reads,sample_break,no_of_ranks_polled,rankid
      check_write_training_stats(3,initial_delay,mov_window,1,2,2,0,2,0,force_fail);
#ifdef CONF_HAS___PHY_RTL
      initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+MVM_DELTA+5;
      initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+MVM_DELTA+5;
#else 
      initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+4;
      initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+4;
#endif 
      check_write_training_stats(3,initial_delay,mov_window,1,2,2,0,2,1,force_fail);

      initial_delay[0] = get_freq_based_golden_read_delay()+5;
      initial_delay[1] = get_freq_based_golden_read_delay()+5;
      check_read_training_stats(3,initial_delay,2,1,2,2,0,2,0, /*force_fail*/force_fail);
      check_read_training_stats(3,initial_delay,2,1,2,2,0,2,1, force_fail);

      ///  ---------------- PHY update training polling -----------------------
      //poll_status_reg_for_phy_update_completion();
      // cant detect phy_update status change so lets wait for next round of trainings 
      poll_status_reg_for_read_training_completion(/*global_hold_ctrl*/0);

      // Disable all training 
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);

       
      dont_program_training_regs_for_write = 0;
      dont_program_training_regs_for_read = 0;
      //simulation_tb_wait(10000,"sch");

    hw_status("Exiting %s\n", __func__);

}

// -------------------------------------------------------------
// mtc_zqcal_wr_rd_phy_combo_training_test_lpddr4_rank0_only()  ZRR
// -------------------------------------------------------------
void mtc_zqcal_wr_rd_phy_combo_training_test_lpddr4_rank0_only(int break_type,int zqcal_mode) {
  uint32_t rd_val;
  uint32_t expect_val;
  int i,initial_delay[NUM_SLICES],j;
  int mov_window;
  int write_val;

    hw_status("Entering %s break_type - %xH, mode - %xH\n", __func__,break_type,zqcal_mode);

     // enable  zq cal 
    /// Enable LPDDR4 ZQ Cal 
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_ZQ_CAL_MODE_ADDRESS, break_type << 2 | zqcal_mode);  // lpddr4 + break = rank one/rank_all enabled 
    
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MPC_VALUE1_ADDRESS, 0x51); // from lpddr4 jedec spec
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MPC_VALUE2_ADDRESS, 0x4f); // from lpddr4 jedec spec
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT1_ADDRESS, 30/1.25); // from lpddr4 jedec spec  1us/2.5ns
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT2_ADDRESS, 0); // from lpddr4 jedec spec 50ns/2.5ns

       // ----- setup stage for write training  ------
#ifdef CONF_HAS___PHY_RTL
       initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+get_freq_based_delay(MVM_DELTA);
       initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+get_freq_based_delay(MVM_DELTA);
#else 
       initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+4;
       initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+4;
#endif 
       for (i=0;i < NUM_SLICES;i++) {
         set_phy_delay_write_reg(i,initial_delay[i],0); // rank 0 
       }
#ifdef CONF_HAS___PHY_RTL
       initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+MVM_DELTA+5;
       initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+MVM_DELTA+5;
       mov_window=get_freq_based_delay(MVM_DELTA);
#else 
       initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+4;
       initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+4;
       mov_window=2;
#endif 
//       for (i=0;i < NUM_SLICES;i++) {
//         set_phy_delay_write_reg(i,initial_delay[i],1); // rank 1 
//       }
       dont_program_training_regs_for_write = 1;
       dont_program_training_regs_for_read = 1;
       //sample_cnt, read_wr_num,adj_step,pass_criteria, mov_window,byte_cnt
       mtc_write_training_basic_test(3,2,1,2,mov_window,1); // LPDDR4

       // ----- setup stage for read training  ------
       initial_delay[0] = get_freq_based_golden_read_delay()+5;
       initial_delay[1] = get_freq_based_golden_read_delay()+5;
       for (i=0;i < NUM_READ_DELAYS ;i++) {
            for (j=0;j < NUM_SLICES ;j++) {
              set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0); // rank0
//              set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],1); // rank1 
            }
       }

       // Since dont_program_training_regs_for_write =1 , program the mov window correctly again 
       write_val = DDC_REGS_MOV_WINDOW_WINDOW_WR_SET(mov_window) | DDC_REGS_MOV_WINDOW_WINDOW_RD_SET(mov_window);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MOV_WINDOW_ADDRESS, write_val);

       mtc_read_training_basic_test(3,2,1,2,2,1); // LPDDR4

       // ------ setup for phy update ------------
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_DFI_TP_CTRLUPD_MAX_ADDRESS, 200);
       
       hw_status("mtc_zqcal_training_test_lpddr4: Enable MTC ZQ Cal MPC Training + Write Training + Read Training + PHY training \n");
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ORDER_ADDRESS, 0x2d);  // ZQCAL,Write,Read Training,PHY training  
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0xf);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, 3);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR1_FREQUENCY_ADDRESS, 3);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR2_FREQUENCY_ADDRESS, 3);
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR3_FREQUENCY_ADDRESS, 3);

       enable_auto_refresh_with_alignment();
   
       simulation_wait_ns(90); // wait for tzcl 90 ns min time 

       // rank 0  polling 
       poll_status_reg_for_zqcal_training_completion();

       // rank 1 polling 
       //poll_status_reg_for_zqcal_training_completion();

      ///  ---------------- Write training polling -----------------------

      // rank 0 polling 
      poll_status_reg_for_write_training_completion(/*global_hold_ctrl*/0);
      if ( ! DDC_REGS_TRAINING_STATUS_WR_FAIL_GET(last_rd_val)) {
             hw_status("mtc_write_training_basic_test: MTC WRITE TRAINING COMPLETED RANK 0!!! regval =%xH\n",rd_val);
      }


      // rank 1 polling 
      //poll_status_reg_for_write_training_completion(/*global_hold_ctrl*/0);
      //if ( ! DDC_REGS_TRAINING_STATUS_WR_FAIL_GET(last_rd_val)) {
      //       hw_status("mtc_write_training_basic_test: MTC WRITE TRAINING COMPLETED RANK 1!!! regval =%xH\n",rd_val);
     // }
      //hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MOV_WINDOW_ADDRESS,2 << DDC_REGS_MOV_WINDOW_WINDOW_WR_LSB);

      ///  ---------------- Read training polling -----------------------

      // rank 0 polling 
      poll_status_reg_for_read_training_completion(/*global_hold_ctrl*/0);

      // rank 1 polling 
      //poll_status_reg_for_read_training_completion(/*global_hold_ctrl*/0);
 
      ///  ---------------- PHY update training polling -----------------------
      //poll_status_reg_for_phy_update_completion();


      // Start checking results 
#ifdef CONF_HAS___PHY_RTL
      initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+get_freq_based_delay(MVM_DELTA);
      initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+get_freq_based_delay(MVM_DELTA);
      mov_window=get_freq_based_delay(MVM_DELTA);
#else 
      initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+4;
      initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+4;
      mov_window = 2;
#endif 
      //sample_cnt, read_wr_num,adj_step,pass_criteria, mov_window,byte_cnt
      //mtc_write_training_basic_test(3,2,1,2,2,1); 

      // sample_cnt, initial_delay_val, mov_window,adj_step,pass_criteria,num_writes_reads,sample_break,no_of_ranks_polled,rankid
      check_write_training_stats(3,initial_delay,mov_window,1,2,2,0,2,0,force_fail);
#ifdef CONF_HAS___PHY_RTL
      initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+MVM_DELTA+5;
      initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+MVM_DELTA+5;
#else 
      initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+4;
      initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+4;
#endif 
      //check_write_training_stats(3,initial_delay,mov_window,1,2,2,0,2,1,force_fail);

      initial_delay[0] = get_freq_based_golden_read_delay()+5;
      initial_delay[1] = get_freq_based_golden_read_delay()+5;
      check_read_training_stats(3,initial_delay,2,1,2,2,0,2,0, force_fail);
      //check_read_training_stats(3,initial_delay,2,1,2,2,0,2,1, force_fail);
       
      // Wait till second round of read training 
      poll_status_reg_for_read_training_completion(/*global_hold_ctrl*/0);

      // Disable all training 
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);

      dont_program_training_regs_for_write = 0;
      dont_program_training_regs_for_read = 0;
      //simulation_tb_wait(10000,"sch");

    hw_status("Exiting %s\n", __func__);

}

// --------------------------------------------------------------------------
// mtc_zqcal_wr_rd_phy_combo_training_test_lpddr4_default_regs()  ZQ++WR+RD
// --------------------------------------------------------------------------
void mtc_zqcal_wr_rd_phy_combo_training_test_lpddr4_default_regs(int break_type,int zqcal_mode) {
  uint32_t rd_val;
  uint32_t expect_val;
  int i,initial_delay[NUM_SLICES],j;

    hw_status("Entering %s break_type - %xH, mode - %xH\n", __func__,break_type,zqcal_mode);

    // enable  zq cal 
    /// Enable LPDDR4 ZQ Cal 
    
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MPC_VALUE1_ADDRESS, 0x51); // from lpddr4 jedec spec
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MPC_VALUE2_ADDRESS, 0x4f); // from lpddr4 jedec spec
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT1_ADDRESS, 30/1.25); // from lpddr4 jedec spec  1us/2.5ns
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT2_ADDRESS, 0); // from lpddr4 jedec spec 30ns/2.5ns

    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, 6);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR1_FREQUENCY_ADDRESS, 6);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR2_FREQUENCY_ADDRESS, 3);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR3_FREQUENCY_ADDRESS, 3);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0xf);

   // ----- setup stage for read training  ------
   //initial_delay[0] = 0;
   //initial_delay[1] = 0;
   //for (i=0;i < NUM_READ_DELAYS ;i++) {
   //    for (j=0;j < NUM_SLICES ;j++) {
   //      set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0); // rank0
   //      set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],1); // rank1 
   //    }
  // }

    // ----- setup stage for write training  ------
    initial_delay[0] = 16;
    initial_delay[1] = 16;
    for (i=0;i < NUM_SLICES;i++) {
         set_phy_delay_write_reg(i,initial_delay[i],0); // rank 0 
    }
   initial_delay[0] = 16;
   initial_delay[1] = 16;
   for (i=0;i < NUM_SLICES;i++) {
         set_phy_delay_write_reg(i,initial_delay[i],1); // rank 1 
   }

    hw_status("mtc_zqcal_training_test_lpddr4: Enable MTC ZQ Cal MPC Training + Write Training + Read Training + PHY training \n");

    enable_auto_refresh_with_alignment();
    ///  ---------------- PHY update training polling -----------------------

    dont_program_training_regs_for_write = 1;
    dont_program_training_regs_for_read = 1;


       //sample_cnt, read_wr_num,adj_step,pass_criteria, mov_window,byte_cnt
       //mtc_read_training_basic_test_default_reg(0xf,5,1,0x4b,0xf,0x1); // LPDDR4
       //mtc_write_training_basic_test_default_reg(0xf,5,1,0x4b,0xf,0x1); // LPDDR4

      ///  ---------------- Write training polling -----------------------

      // rank 0 polling 
      poll_status_reg_for_write_training_completion(/*global_hold_ctrl*/0);
      if ( ! DDC_REGS_TRAINING_STATUS_WR_FAIL_GET(last_rd_val)) {
             hw_status("mtc_write_training_basic_test: MTC WRITE TRAINING COMPLETED RANK 0!!! regval =%xH\n",rd_val);
      }

      // rank 1 polling 
      poll_status_reg_for_write_training_completion(/*global_hold_ctrl*/0);
      if ( ! DDC_REGS_TRAINING_STATUS_WR_FAIL_GET(last_rd_val)) {
             hw_status("mtc_write_training_basic_test: MTC WRITE TRAINING COMPLETED RANK 1!!! regval =%xH\n",rd_val);
      }

      //poll_status_reg_for_phy_update_completion();
      // rank 0  polling 

      //poll_status_reg_for_zqcal_training_completion();

      ///  ---------------- Read training polling -----------------------
      // rank 0 polling 
      poll_status_reg_for_read_training_completion(/*global_hold_ctrl*/0);

      // rank 1 polling 
      poll_status_reg_for_read_training_completion(/*global_hold_ctrl*/0);

      // Disable all training 
      hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x0);

      //hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MOV_WINDOW_ADDRESS,2 << DDC_REGS_MOV_WINDOW_WINDOW_WR_LSB);

      /*

      // Start checking results 
      initial_delay[0] = 2;
      initial_delay[1] = 2;
      //sample_cnt, read_wr_num,adj_step,pass_criteria, mov_window,byte_cnt
      //mtc_write_training_basic_test(3,2,1,2,2,1); 
      // sample_cnt, initial_delay_val, mov_window,adj_step,pass_criteria,num_writes_reads,sample_break,no_of_ranks_polled,rankid
      check_write_training_stats(3,initial_delay,2,1,2,2,0,2,0);
      initial_delay[0] = 5;
      initial_delay[1] = 5;
      check_write_training_stats(3,initial_delay,2,1,2,2,0,2,1);

      initial_delay[0] = 0;
      initial_delay[1] = 0;
      check_read_training_stats(3,initial_delay,2,1,2,2,0,2,0, force_fail);
      check_read_training_stats(3,initial_delay,2,1,2,2,0,2,1, force_fail);

      */
       
      dont_program_training_regs_for_write = 0;
      dont_program_training_regs_for_read = 0;
      //simulation_tb_wait(10000,"sch");

    hw_status("Exiting %s\n", __func__);

}

// ---------------------
// Enabling_PUB_mode
// ---------------------
void Enabling_PUB_mode(){
#ifdef CONF_HAS___PHY_RTL
 int rd_val;
     //hw_write32((uint32_t)0x2005, 0x02004640);

    hw_status("ENABLING PUB\n");
    rd_val = hw_phy_read32((uint32_t *) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR1_ADDRESS);
    hw_status("OLD PGCR1 = %xH\n",rd_val);

    hw_phy_write32((uint32_t)MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR1_ADDRESS,
          rd_val | PHY_REGS_DWC_DDRPHY_PUB_PGCR1_PUBMODE_FIELD_MASK );
    hw_status("NEW PGCR1 = %xH\n",rd_val | PHY_REGS_DWC_DDRPHY_PUB_PGCR1_PUBMODE_FIELD_MASK);
#endif
}

// ---------------------
// Disabling_PUB_mode
// ---------------------
void Disabling_PUB_mode(){
#ifdef CONF_HAS___PHY_RTL
  int rd_val;
     hw_status("DISABLING PUB\n");
     hw_phy_write32((uint32_t)0x2005, 0x02004640);

    rd_val = hw_phy_read32((uint32_t *) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR1_ADDRESS);
    hw_status("OLD PGCR1 = %xH\n",rd_val);
    hw_phy_write32((uint32_t)MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR1_ADDRESS,
          rd_val & ~PHY_REGS_DWC_DDRPHY_PUB_PGCR1_PUBMODE_FIELD_MASK );
    hw_status("NEW PGCR1 = %xH\n",rd_val & ~PHY_REGS_DWC_DDRPHY_PUB_PGCR1_PUBMODE_FIELD_MASK);
#endif
}


// --------------------
// mtc_write_train_init()
// --------------------
void mtc_write_train_init(active_ranks) {
    int i,j;
    uint32_t initial_delay[NUM_SLICES];

     mtc_write_init(/*default_mtc_setup*/0, /*min_tr_freq*/0);
     mtc_common_init_setup(active_ranks);
#ifdef CONF_HAS___PHY_RTL

     initial_delay[0] = get_freq_based_golden_write_delay(0, 0);
     initial_delay[1] = get_freq_based_golden_write_delay(0, 1);
     for (i=0;i < NUM_SLICES;i++) {
        set_phy_delay_write_reg(i,initial_delay[i],0);
     }
#endif

}

void mtc_init_basic_only() {

    int i,j;
    int rd_val;
    int initial_delay[NUM_SLICES];

     gate_adj_disable  = 1;
     phy_based_delay_adj = 0;
     wdlvt_enable = 0;

#ifdef CONF_HAS___PHY_RTL

     // Enabling Controller PHY update ack 
     rd_val = hw_phy_read32((uint32_t *)  MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DSGCR_ADDRESS);
     hw_status("DSGCR = %xH\n",rd_val);
     rd_val = rd_val | PHY_REGS_DWC_DDRPHY_PUB_DSGCR_CUAEN_SET(1);
     hw_phy_write32((uint32_t) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DSGCR_ADDRESS, rd_val);
     hw_status("NEW DSGCR = %xH\n",rd_val);
     hw_status("\n");
     hw_status("PUB_PGCR3_GATEACCTLCLK = %xH\n",get_pub_gcr3_gateacctlclk());

    /*
    for (i=0;i < NUM_SLICES;i++) {
       global_wdqdprd[i] = get_WDQPRD(i,0);
    }
    */

     // Program GCR3.WDLVT 
     for (i=0;i < NUM_RANKS;i++) {
      for (j=0;j < NUM_SLICES;j++) {
        set_dxngcr3_wdlvt(j,i,wdlvt_enable);
        //dxngsr0_gdqsprd[i][j] = get_phy_dxngsr0_gdqsprd(j,i);
        //dxngtr0_dgsl[i][j] = get_phy_dxngtr0_dgsl(j,i);
      }
     }
     // Program WDLVT = 0 for MTC WRITE_TRAINING_CONFIG
     //hw_write32((uint32_t) MCU_REGS_DDC_REGS_WRITE_TRAINING_CONFIG_ADDRESS,DDC_REGS_WRITE_TRAINING_CONFIG_VALUE_SET(wdlvt_enable));

     /*
     initial_delay[0] = get_freq_based_golden_write_delay(0, 0);
     initial_delay[1] = get_freq_based_golden_write_delay(0, 1);
     for (i=0;i < NUM_SLICES;i++) {
        set_phy_delay_write_reg(i,initial_delay[i],0);
     }
     */
#endif

    trn_frq = rand_interval(1,5);
    hw_status("trn_frq = %xH\n",trn_frq);

}

void mtc_init() {

    int i,j;
    int rd_val;
    int initial_delay[NUM_SLICES];

     gate_adj_disable  = 1;
     phy_based_delay_adj = 0;
     wdlvt_enable = 0;

#ifdef CONF_HAS___PHY_RTL

     // Enabling Controller PHY update ack 
     rd_val = hw_phy_read32((uint32_t *)  MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DSGCR_ADDRESS);
     hw_status("DSGCR = %xH\n",rd_val);
     rd_val = rd_val | PHY_REGS_DWC_DDRPHY_PUB_DSGCR_CUAEN_SET(1);
     hw_phy_write32((uint32_t) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DSGCR_ADDRESS, rd_val);
     hw_status("NEW DSGCR = %xH\n",rd_val);
     hw_status("\n");
     hw_status("PUB_PGCR3_GATEACCTLCLK = %xH\n",get_pub_gcr3_gateacctlclk());

    for (i=0;i < NUM_SLICES;i++) {
       global_wdqdprd[i] = get_WDQPRD(i,0);
    }

     // Program GCR3.WDLVT 
     for (i=0;i < NUM_RANKS;i++) {
      for (j=0;j < NUM_SLICES;j++) {
        set_dxngcr3_wdlvt(j,i,wdlvt_enable);
        dxngsr0_gdqsprd[i][j] = get_phy_dxngsr0_gdqsprd(j,i);
        dxngtr0_dgsl[i][j] = get_phy_dxngtr0_dgsl(j,i);
      }
     }
     // Program WDLVT = 0 for MTC WRITE_TRAINING_CONFIG
     //hw_write32((uint32_t) MCU_REGS_DDC_REGS_WRITE_TRAINING_CONFIG_ADDRESS,DDC_REGS_WRITE_TRAINING_CONFIG_VALUE_SET(wdlvt_enable));

     initial_delay[0] = get_freq_based_golden_write_delay(0, 0);
     initial_delay[1] = get_freq_based_golden_write_delay(0, 1);
     for (i=0;i < NUM_SLICES;i++) {
        set_phy_delay_write_reg(i,initial_delay[i],0);
     }

#else 
     // Program WDLVT = 0 for MTC WRITE_TRAINING_CONFIG
     //hw_write32((uint32_t) MCU_REGS_DDC_REGS_WRITE_TRAINING_CONFIG_ADDRESS,DDC_REGS_WRITE_TRAINING_CONFIG_VALUE_SET(wdlvt_enable));

/*  --- Removing the setting of phy write registers for PHY bfm tests ------ 
     initial_delay[0] = get_freq_based_golden_write_delay(0, 0);
     initial_delay[1] = get_freq_based_golden_write_delay(0, 1);
     for (i=0;i < NUM_SLICES;i++) {
        set_phy_delay_write_reg(i,initial_delay[i],0);
     }
*/
     for (i=0;i < NUM_RANKS;i++) {
      for (j=0;j < NUM_SLICES;j++) {
        dxngtr0_dgsl[i][j] = get_phy_dxngtr0_dgsl(j,i);
        dxngsr0_gdqsprd[i][j] = get_phy_dxngsr0_gdqsprd(j,i);
      }
     }

     
#endif

     hw_write32((uint32_t) MCU_REGS_DDC_REGS_READ_TRAINING_CONFIGURATION_ADDRESS, 
                         DDC_REGS_READ_TRAINING_CONFIGURATION_PHY_BASED_SET(phy_based_delay_adj) |
                         DDC_REGS_READ_TRAINING_CONFIGURATION_GATE_ADJ_DISABLE_SET(gate_adj_disable));

    trn_frq = rand_interval(1,5);
    hw_status("trn_frq = %xH\n",trn_frq);

}

// ---------------------------
// Entry function 
// ---------------------------
// mtc_training_tests()
// ---------------------------
void mtc_training_tests(int mtc_test_num,int dram_mode,int autoref_int,int delay_model, int dram_frequency, int active_ranks) {

    int i,j;
    int rd_val;
    uint32_t sample_cnt, read_wr_num,adj_step,pass_criteria, mov_window, byte_cnt, sample_break;
    uint32_t initial_delay[NUM_SLICES];
    int train_count;
    int gap_set;

    hw_status("Entering %s\n", __func__);
    hw_status("Delay model = %xH\n", delay_model);
    global_delay_model = delay_model;
    global_dram_mode = dram_mode;
    global_dram_frequency = dram_frequency;
    hw_status("Freq = %xH\n", dram_frequency);

    //mtc_common_init_setup(active_ranks);

    mtc_write_train_init(active_ranks);

    switch(mtc_test_num) {

       case 0 : 
           mtc_phy_update_training_test();
           break;

       case 1 : 
           mtc_zqcal_training_test_lpddr3(0); // short_training LPDDR3
           break;

       case 2 : 
           mtc_zqcal_training_test_lpddr3(1); // long_training LPDDR3
           break;

       case 3 : 
           mtc_zqcal_training_test_lpddr3(2); // init training  LPDDR3
           break;

       case 4 : 
           //sample_cnt, read_wr_num,adj_step,pass_criteria, mov_window,byte_cnt
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+1;
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+1;
           mov_window = 1;
#else 
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1);
           mov_window = 1;
#endif 
           for (i=0;i < NUM_SLICES;i++) {
             set_phy_delay_write_reg(i,initial_delay[i],0);
           }
           mtc_write_training_basic_test(3,2,1,2,mov_window,1); // LPDDR4
           check_write_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0,force_fail);
           break;

       case 5 :  // RDATA
           read_methodA_init();

#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           mov_window=MVM_DELTA+10;
#else 
           initial_delay[0] = get_freq_based_golden_read_delay()+1;
           initial_delay[1] = get_freq_based_golden_read_delay()+1;
           mov_window = 1;
#endif 
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
               set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
             }
           }
           mtc_read_training_basic_test(3,2,1,2,mov_window,1);  // LPDDR4
           check_read_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0, force_fail);
           break;

       case 6 : 
           Disabling_PUB_mode();
           mtc_zqcal_training_test_lpddr4(0,2); // MPC 1 step
           break;

       case 7 : 
           Disabling_PUB_mode();
           read_methodA_init();
           mtc_phyupdate_zqcal_combo_training_test_lpddr3(0);
           break;

       case 8 : 
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA);
#else 
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1);
           mov_window = 1;
#endif 
           for (i=0;i < NUM_SLICES;i++) {
             set_phy_delay_write_reg(i,initial_delay[i],0);
           }
           sample_break = 1;
           //sample_cnt, read_wr_num,adj_step,pass_criteria, mov_window,byte_cnt
           mtc_write_training_sample_break_enabled(3,2,1,2,mov_window,1); // LPDDR4
           check_write_training_stats(3,initial_delay,mov_window,1,2,2,sample_break,1,0,force_fail);
           break;

       case 9 : 
           // Randomize parameters
           random_write_data = 1;
           sample_break= 0;
           sample_cnt = rand_interval(1,15);  // 4 bits 
           read_wr_num =   rand_interval(1,7); //  3 bits
           mov_window =  rand_interval(1,10); //  4 bits
           pass_criteria = rand_interval(read_wr_num*(sample_cnt/2),read_wr_num*sample_cnt); // 7 bits 
           adj_step = rand_interval(1,mov_window) & 0x7; //  3 bits 
           byte_cnt = 1;
           initial_delay[0] = rand_interval(1,3); 
           
           if (initial_delay[0] < mov_window) { // avoid negative left movements
              if (  (initial_delay[0] + mov_window) >= 10 ) {  // avoid more than 1/2 clock cycle movements 
                 mov_window /= 2;
                 initial_delay[0] = initial_delay[0] + mov_window;
              } else {
                 initial_delay[0] = initial_delay[0] + mov_window;// avoid more than 1/2 clock cycle movements 
                 if ((initial_delay[0] + mov_window) >= 10 ) {
                    mov_window /= 2;
                    initial_delay[0] = rand_interval(1,3) + mov_window;
                 }
              }
           }
           initial_delay[1] = initial_delay[0];

           for (i=0;i < NUM_SLICES;i++) {
             set_phy_delay_write_reg(i,initial_delay[i],0);
           }

           mtc_write_training_basic_test(sample_cnt,read_wr_num,adj_step,pass_criteria,mov_window,byte_cnt); // LPDDR4
           check_write_training_stats(sample_cnt,initial_delay,mov_window,adj_step,pass_criteria,read_wr_num,sample_break,1,0,force_fail);
           random_write_data = 0;
           break;

 
       case 10 :   // Directed testcase for dbg only 
           /*
           sample_cnt = 1;  
           sample_break = 0;
           read_wr_num = 2; 
           adj_step = 1; 
           pass_criteria = 2; 
           mov_window=get_freq_based_delay(MVM_DELTA); 
           byte_cnt = 1;
           initial_delay[0]= 6;
           */
           sample_cnt = 14;  
           sample_break = 0;
           read_wr_num = 7; 
           adj_step = 5; 
           pass_criteria = 14; 
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0]= get_freq_based_golden_write_delay(0, 0)+get_freq_based_delay(MVM_DELTA);
           initial_delay[1]= get_freq_based_golden_write_delay(0, 1)+get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA); 
#else 
           mov_window =  3; 
           initial_delay[0]= get_freq_based_golden_write_delay(0, 0)+6;;
           initial_delay[1]= get_freq_based_golden_write_delay(0, 1)+6;;
#endif 
           byte_cnt = 1;
           for (i=0;i < NUM_SLICES;i++) {
             set_phy_delay_write_reg(i,initial_delay[i],0);
           }
           mtc_write_training_basic_test(sample_cnt,read_wr_num,adj_step,pass_criteria,mov_window,byte_cnt); // LPDDR4
           check_write_training_stats(sample_cnt,initial_delay,mov_window,adj_step,pass_criteria,read_wr_num,sample_break,1,0,force_fail);
           // ---- Debug only pass cnt registers 
           //rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_RD_WR_TR_RIGHT_PASS_CNT_ADDRESS);
           //hw_status("check_write_training_stats: WR_TR_RIGHT_PASS_CNT regval = %xH\n", rd_val);
           //rd_val = hw_read32((uint32_t *) MCU_REGS_DDC_REGS_RD_WR_TR_LEFT_PASS_CNT_ADDRESS);
           //hw_status("check_write_training_stats: WR_TR_LEFT_PASS_CNT regval = %xH\n", rd_val);
           break;

       case 11 : 
           read_methodA_init();
           // Randomize parameters for read training test
           random_write_data = 1;
           sample_cnt = rand_interval(1,15);  // 4 bits 
           //sample_cnt = 1;
           hw_status("mtc_test: sample_cnt = %0d\n", sample_cnt);
           read_wr_num =   rand_interval(1,7); //  3 bits
           //read_wr_num =   1;
           hw_status("mtc_test: read_wr_num = %0d\n", read_wr_num);
           //U pass_criteria = rand_interval(1,read_wr_num*2); // 7 bits 
           pass_criteria = rand_interval(read_wr_num*(sample_cnt/2),read_wr_num*sample_cnt); // 7 bits
           hw_status("mtc_test: pass_criteria = %0d\n", pass_criteria);
           mov_window =  rand_interval(1,15); //  4 bits
           //mov_window = 4;
           hw_status("mtc_test: mov_window = %0d\n", mov_window);
           adj_step = rand_interval(1,mov_window) & 0x7; //  3 bits 
           if ( adj_step == 0 ) adj_step++;
           //adj_step = 1;
           hw_status("mtc_test: adj_step = %0d\n", adj_step);
           byte_cnt = 1;
           sample_break = 0;
           //U initial_delay[0]=  rand_interval(1,10);
           initial_delay[0] = rand_interval(1,3);
           hw_status("mtc_test: rand initial_delay = %0d\n", initial_delay[0]);
           if (initial_delay[0] < mov_window) { // avoid negative left movements
              if (  (initial_delay[0] + mov_window) >= 15 ) {  // avoid more than 1/2 clock cycle movements 
                 mov_window /= 2;
                 initial_delay[0] = initial_delay[0] + mov_window;
              } else {
                 initial_delay[0] = initial_delay[0] + mov_window;// avoid more than 1/2 clock cycle movements 
                 if ((initial_delay[0] + mov_window) >= 15 ) {
                    mov_window /= 2;
                    initial_delay[0] = rand_interval(1,3) + mov_window;
                 }
              }
           }
           //initial_delay[0] = 5;
           hw_status("mtc_test: after adjust initial_delay = %0d\n", initial_delay[0]);
           initial_delay[1] = initial_delay[0];
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
               set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
             }
           }
           mtc_read_training_basic_test(sample_cnt,read_wr_num,adj_step,pass_criteria,mov_window,byte_cnt); // LPDDR4
           check_read_training_stats(sample_cnt,initial_delay,mov_window,adj_step,pass_criteria,read_wr_num,sample_break,1,0, force_fail);
           random_write_data = 0;
             
           break;

       case 12 :   // Directed testcase for dbg only 
           read_methodA_init();
           sample_cnt = 1;  
           read_wr_num = 2; 
           adj_step = 1; 
           pass_criteria = 2; 
           byte_cnt = 1;
           sample_break = 0;
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA); 
#else 
           initial_delay[0] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA); 
           //initial_delay[0] = get_freq_based_golden_read_delay()+1;
           //initial_delay[1] = get_freq_based_golden_read_delay();
           //mov_window =  1; 
#endif 
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
             set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
             }
           }
           mtc_read_training_basic_test(sample_cnt,read_wr_num,adj_step,pass_criteria,mov_window,byte_cnt); // LPDDR4
           check_read_training_stats(sample_cnt,initial_delay,mov_window,adj_step,pass_criteria,read_wr_num,sample_break,1,0, force_fail);
           break;

       case 13 : 
           read_methodA_init();
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA);
#else 
           initial_delay[0] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           mov_window = 1;
#endif 
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
               set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
             }
           }
           sample_break = 1;
           mtc_read_training_sample_break_enabled(3,2,1,2,mov_window,1); // LPDDR4
           check_read_training_stats(3,initial_delay,mov_window,1,2,2,sample_break,1,0, force_fail);
           break;

       // ------------------ Movement tests -------------------
   
       case 14 :   // Write training with left movement 
           sample_cnt = 1;  
           sample_break = 0;
           read_wr_num = 2; 
           adj_step = 5; 
           pass_criteria = 2; 
           byte_cnt = 1;
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA); 
#else 
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA); 
#endif 

          // This will cause right comparisons to fail and left comparisons to pass, causing left movement 
           for (i=0;i < NUM_SLICES;i++) {
             set_phy_delay_write_reg(i,initial_delay[i],0);
           }
           mtc_write_training_basic_test(sample_cnt,read_wr_num,adj_step,pass_criteria,mov_window,byte_cnt); // LPDDR4
           check_write_training_stats(sample_cnt,initial_delay,mov_window,adj_step,pass_criteria,read_wr_num,sample_break,1,0,force_fail);
           break;

       case 15 :   // Write training with right movement (PR)
           sample_cnt = 1;  
           read_wr_num = 2; 
           adj_step = 1; 
           pass_criteria = 2; 
           mov_window =  2; 
           byte_cnt = 1;
           sample_break = 0;
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0)-get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)-get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA); 
#else 
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+2;
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+2;
           mov_window =  2; 
#endif

           for (i=0;i < NUM_SLICES;i++) {
             set_phy_delay_write_reg(i,initial_delay[i],0);
           }
           mtc_write_training_basic_test(sample_cnt,read_wr_num,adj_step,pass_criteria,mov_window,byte_cnt); // LPDDR4
           check_write_training_stats(sample_cnt,initial_delay,mov_window,adj_step,pass_criteria,read_wr_num,sample_break,1,0,force_fail);
           break;

       case 16 :   // Read training with left movement 
           read_methodA_init();
           sample_cnt = 1;  
           read_wr_num = 2; 
           adj_step = 1; 
           pass_criteria = 2; 
           mov_window=get_freq_based_delay(MVM_DELTA); 
           byte_cnt = 1;
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA); 
#else 
           initial_delay[0] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA); 
#endif 
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
               set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
             }
           }
           sample_break= 0;
           simulation_wait_ns(100);                    
           mtc_read_training_basic_test(sample_cnt,read_wr_num,adj_step,pass_criteria,mov_window,byte_cnt); // LPDDR4
           check_read_training_stats(sample_cnt,initial_delay,mov_window,adj_step,pass_criteria,read_wr_num,sample_break,1,0, force_fail);
           break;

       case 17 :   // Read training with right movement 
           read_methodA_init();
           sample_cnt = 2;  
           read_wr_num = 2; 
           adj_step = 1; 
           pass_criteria = 2; 
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay()-get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay()-get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA); 
#else 
           initial_delay[0] = get_freq_based_golden_read_delay();
           initial_delay[1] = get_freq_based_golden_read_delay();
           mov_window = 1; 
#endif 
           sample_break = 0;
           byte_cnt = 1;
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
                set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
             } 
           }
           simulation_wait_ns(100);                    
           mtc_read_training_basic_test(sample_cnt,read_wr_num,adj_step,pass_criteria,mov_window,byte_cnt); // LPDDR4
           check_read_training_stats(sample_cnt,initial_delay,mov_window,adj_step,pass_criteria,read_wr_num,sample_break,1,0, force_fail);
           break;

       case 18 :  
          // read training failing test 
          // Training fails but test will signal pass because the auto checker has verified this scenario

           sample_cnt = 3;
           read_wr_num = 2;
           adj_step = 1;
           pass_criteria = 2;
           byte_cnt = 1;
           read_methodA_init();
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay();
           initial_delay[1] = get_freq_based_golden_read_delay();
           mov_window =  MVM_DELTA*3; 
#else 
           initial_delay[0] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           mov_window =  2;
#endif
           sample_break = 0;

           for( train_count=0; train_count < 2; train_count++) {
             for (i=0;i <NUM_READ_DELAYS;i++) {
               for (j=0;j <NUM_SLICES;j++) {
                 set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);  // rank 0 regs 
                 set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],1);  // rank 1 regs 
               }
             }
             mtc_read_training_basic_test(sample_cnt,read_wr_num,adj_step,pass_criteria,mov_window,byte_cnt); // LPDDR4
             simulation_wait_ns(500);                    
             check_read_training_stats(sample_cnt,initial_delay,mov_window,adj_step,pass_criteria,read_wr_num,sample_break,1,train_count % 2, force_fail);
             non_fatal_status_handler();
             fatal_status_handler();
             intr_status_handler();
           }

           break;
 
       case 19 :   // write training failing test

          if (active_ranks == 1)
          {   
              program_single_rank_config();
              set_rank_id(0);
          }
          // Training fails but test will signal pass because the auto checker has verified this scenario
           sample_cnt = 3;  
           sample_break = 0;
           read_wr_num = 7; 
           adj_step = 3; 
           pass_criteria = 7; 
           byte_cnt = 1;
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1);
           mov_window =  2 * get_freq_based_delay(MVM_DELTA); 
#else 
           initial_delay[0]= get_freq_based_golden_write_delay(0, 0)+4;
           initial_delay[1]= get_freq_based_golden_write_delay(0, 1)+4;
           mov_window =  3; 
#endif

             for (i=0;i < NUM_SLICES;i++) {
               set_phy_delay_write_reg(i,initial_delay[i],0);
               if (active_ranks == 3)
               {
                 set_phy_delay_write_reg(i,initial_delay[i],1);
               }
             }
           for( train_count=0; train_count < 2; train_count++) {
             mtc_write_training_basic_test(sample_cnt,read_wr_num,adj_step,pass_criteria,mov_window,byte_cnt); // LPDDR4
             simulation_wait_ns(500);                    

             if (active_ranks == 3)
             {
               check_write_training_stats(sample_cnt,initial_delay,mov_window,adj_step,pass_criteria,read_wr_num,sample_break,1,train_count % 2,force_fail);
             }
             else
             {
               check_write_training_stats(sample_cnt,initial_delay,mov_window,adj_step,pass_criteria,read_wr_num,sample_break,1,0,force_fail);
             }
             non_fatal_status_handler();
             fatal_status_handler();
             intr_status_handler();
           }
           break;

       case 20 :  // single rank break test
           Disabling_PUB_mode();
           mtc_zqcal_training_test_lpddr4(1,2); // MPC 1 step
           break;

       case 21 :  
           if (active_ranks == 1)
           {
             program_single_rank_config();
           }
           Disabling_PUB_mode();
           read_methodA_init();
           if (active_ranks == 1)
           {
             mtc_zqcal_wr_rd_phy_combo_training_test_lpddr4_rank0_only(0,2);
           }
           else
           {
             mtc_zqcal_wr_rd_phy_combo_training_test_lpddr4(1,2);
           }
           break;

       case 22 :  // 2 step zq cal 
           Disabling_PUB_mode();
           mtc_zqcal_training_test_lpddr4_new_requirement(0,3); // MPC 2 step
           break;

       case 23 : 
           Disabling_PUB_mode();
           read_methodA_init();
           mtc_zqcal_wr_rd_phy_combo_training_test_lpddr4_default_regs(0,3);
           break;

       case 24 : 
           Disabling_PUB_mode();
           mtc_phy_update_training_test_wait_expired();
           non_fatal_status_handler();
           fatal_status_handler();
           intr_status_handler();
           break;

       case 25 : 
           Disabling_PUB_mode();
           mtc_phy_update_training_test_wait_expired_with_ack();
           break;

       case 26 : 
           Disabling_PUB_mode();
           mtc_phy_update_training_test_wait_expired_with_req();
           break;

       case 27 : 
           Disabling_PUB_mode();
           mtc_phy_update_training_test_wait_timeout_max();
           break;

       case 28 : 
           Disabling_PUB_mode();
           mtc_phy_update_training_test_random_CTRLUPD();
           break;

       case 29 : // slice0 - left , slice 1 fail write training 
           //sample_cnt, read_wr_num,adj_step,pass_criteria, mov_window,byte_cnt

#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+40;
           mov_window=get_freq_based_delay(MVM_DELTA);
#else
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+1;
#endif 
           for (i=0;i < NUM_SLICES;i++) {
             set_phy_delay_write_reg(i,initial_delay[i],0);
           }
           mtc_write_training_basic_test(3,2,1,2,1,1);  
           check_write_training_stats(3,initial_delay,1,1,2,2,0,1,0,force_fail);
           break;

       case 30 :  // slice0 - left , slice 1 fail read training 

           read_methodA_init();
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay()+40;
           mov_window=get_freq_based_delay(MVM_DELTA); 
#else 
           initial_delay[0] = get_freq_based_golden_read_delay()+2;
           initial_delay[1] = get_freq_based_golden_read_delay()+3;
#endif 
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
               set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
             }
           }
           mtc_read_training_basic_test(3,2,1,2,2,1);   
           check_read_training_stats(3,initial_delay,2,1,2,2,0,1,0, force_fail);
           break;

       case 31 :  // slice0  left, slice1 none  write training 

#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1);
           mov_window=get_freq_based_delay(MVM_DELTA);
#else
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+2;
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+4;
           mov_window = 2;
#endif 
           for (i=0;i < NUM_SLICES;i++) {
             set_phy_delay_write_reg(i,initial_delay[i],0);
           }
           mtc_write_training_basic_test(3,2,1,2,mov_window,1);  
           check_write_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0,force_fail);
           break;

       case 32 : // slice0  left, slice1 none read training 

           read_methodA_init();
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay();
           mov_window=get_freq_based_delay(MVM_DELTA); 
#else 
           initial_delay[0] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay()+2;
           mov_window=get_freq_based_delay(MVM_DELTA);
#endif 
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
               set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
             }
           }
           mtc_read_training_basic_test(3,2,1,2,mov_window,1);   
           check_read_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0, force_fail);
           break;

       case 33 :  // slice0  none, slice1 left  write training 

#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA);
#else
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+1;
           mov_window = 1;
#endif 
           for (i=0;i < NUM_SLICES;i++) {
             set_phy_delay_write_reg(i,initial_delay[i],0);
           }
           mtc_write_training_basic_test(3,2,1,2,mov_window,1);  
           check_write_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0,force_fail);
           break;

       case 34 : // slice0  none, slice1 left read training 

           read_methodA_init();
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay();
           initial_delay[1] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA); 
#else 
           initial_delay[0] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay()+2;
            mov_window = 2;
#endif 
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
               set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
             }
           }
           mtc_read_training_basic_test(3,2,1,2,mov_window,1);   
           check_read_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0, force_fail);
           break;


       case 35 :  // slice0  none, slice1 right  write training 

#ifdef CONF_HAS___PHY_RTL
           //sample_cnt, read_wr_num,adj_step,pass_criteria, mov_window,byte_cnt
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)-get_freq_based_delay(MVM_DELTA);
           mov_window =  get_freq_based_delay(MVM_DELTA);
#else
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1);
           mov_window = 1;
#endif 
           for (i=0;i < NUM_SLICES;i++) {
             set_phy_delay_write_reg(i,initial_delay[i],0);
           }
           mtc_write_training_basic_test(3,2,1,2*3,mov_window,1);  
           check_write_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0,force_fail);
           break;

       case 36 : // slice0  none, slice1 right read training 

           read_methodA_init();
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay();
           initial_delay[1] = get_freq_based_golden_read_delay()-get_freq_based_delay(MVM_DELTA);
           mov_window =  get_freq_based_delay(MVM_DELTA); 
#else 
           initial_delay[0] = get_freq_based_golden_read_delay()+10;
           initial_delay[1] = get_freq_based_golden_read_delay()-get_freq_based_delay(MVM_DELTA);
            mov_window=get_freq_based_delay(MVM_DELTA);
#endif 
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
               set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
             }
           }
           mtc_read_training_basic_test(3,2,1,2,mov_window,1);   
           check_read_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0, force_fail);
           break;

       case 37 :  // slice0  right, slice1 none  write training 

#ifdef CONF_HAS___PHY_RTL
           //sample_cnt, read_wr_num,adj_step,pass_criteria, mov_window,byte_cnt
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0)-get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1);
           mov_window = get_freq_based_delay(MVM_DELTA);
#else
           initial_delay[1] = get_freq_based_golden_write_delay(0, 0)+get_freq_based_delay(MVM_DELTA);
           initial_delay[0] = get_freq_based_golden_write_delay(0, 1);
           mov_window = 1;
#endif 
           for (i=0;i < NUM_SLICES;i++) {
             set_phy_delay_write_reg(i,initial_delay[i],0);
           }
           mtc_write_training_basic_test(3,2,1,2,mov_window,1);  
           check_write_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0,force_fail);
           break;

       case 38 : // slice0  right, slice1 none read training 
           read_methodA_init();

#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay()-get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay();
           mov_window =  get_freq_based_delay(MVM_DELTA); 
#else 
           initial_delay[1] = get_freq_based_golden_read_delay()+10;
           initial_delay[0] = get_freq_based_golden_read_delay()-get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA);
#endif 
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
               set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
             }
           }
           mtc_read_training_basic_test(3,2,1,2,mov_window,1);   
           check_read_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0, force_fail);
           break;

       case 39 :   // Directed test for typical DELAY=2 model 
           read_methodA_init();
           sample_cnt = 1;  
           read_wr_num = 2; 
           adj_step = 1; 
           pass_criteria = 2; 
           byte_cnt = 1;
           sample_break = 0;
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay();
           initial_delay[1] = get_freq_based_golden_read_delay();
           mov_window=get_freq_based_delay(MVM_DELTA); 
#else 
           //initial_delay[0] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           //initial_delay[1] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           //mov_window=get_freq_based_delay(MVM_DELTA); 
           // Passed @ 2,1
           initial_delay[0] = get_freq_based_golden_read_delay()+2; 
           initial_delay[1] = get_freq_based_golden_read_delay()+1;
           mov_window =  1; 
#endif 
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
             set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
             }
           }
           mtc_read_training_basic_test(sample_cnt,read_wr_num,adj_step,pass_criteria,mov_window,byte_cnt); // LPDDR4
           check_read_training_stats(sample_cnt,initial_delay,mov_window,adj_step,pass_criteria,read_wr_num,sample_break,1,0, force_fail);
           break;

      case 40 :

           read_methodA_init();
           gate_adj_disable  = 1;
           phy_based_delay_adj = 0;
           hw_write32((uint32_t) MCU_REGS_DDC_REGS_READ_TRAINING_CONFIGURATION_ADDRESS, DDC_REGS_READ_TRAINING_CONFIGURATION_PHY_BASED_SET(phy_based_delay_adj) |
                   DDC_REGS_READ_TRAINING_CONFIGURATION_GATE_ADJ_DISABLE_SET(gate_adj_disable));
           sample_cnt = 2;  
           read_wr_num = 2; 
           adj_step = 1; 
           pass_criteria = 2; 
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay()-get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay()-get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA); 
#else 
           initial_delay[0] = get_freq_based_golden_read_delay();
           initial_delay[1] = get_freq_based_golden_read_delay();
           mov_window = 1; 
#endif 
           sample_break = 0;
           byte_cnt = 1;
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
                set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
             } 
           }
           simulation_wait_ns(100);                    
           mtc_read_training_basic_test(sample_cnt,read_wr_num,adj_step,pass_criteria,mov_window,byte_cnt); // LPDDR4
           check_read_training_stats(sample_cnt,initial_delay,mov_window,adj_step,pass_criteria,read_wr_num,sample_break,1,0, force_fail);
           break;

      case 41 :

           gate_adj_disable  = 1;
           phy_based_delay_adj = 1;
           read_methodB_init(/*default_mtc_setup*/0);
           global_drift_enabled_for_phy_based_training = 1;

           sample_cnt = 2;  
           read_wr_num = 31; 
           adj_step = 1; 
           pass_criteria = 2; 
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay();
           initial_delay[1] = get_freq_based_golden_read_delay();
           mov_window=get_freq_based_delay(MVM_DELTA); 
#else 
           initial_delay[0] = get_freq_based_golden_read_delay();
           initial_delay[1] = get_freq_based_golden_read_delay();
           mov_window = 1; 
#endif 
           sample_break = 0;
           byte_cnt = 1;
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
                set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
             } 
           }
           mtc_read_training_basic_test_phy_based(sample_cnt,read_wr_num,adj_step,pass_criteria,mov_window,byte_cnt); // LPDDR4
           simulation_wait_ns(50000);                    
           check_read_training_stats(sample_cnt,initial_delay,mov_window,adj_step,pass_criteria,read_wr_num,sample_break,1,0, force_fail);
           break;

      case 42 :

           gate_adj_disable  = 1;
           phy_based_delay_adj = 1;

           sample_cnt = 1;  
           sample_break = 0;
           read_wr_num = 2; 
           adj_step = 5; 
           pass_criteria = 2; 
           byte_cnt = 1;
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA); 
#else 
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA); 
#endif 

          // This will cause right comparisons to fail and left comparisons to pass, causing left movement 
           for (i=0;i < NUM_SLICES;i++) {
             set_phy_delay_write_reg(i,initial_delay[i],0);
           }
           hw_write32((uint32_t) MCU_REGS_DDC_REGS_READ_TRAINING_CONFIGURATION_ADDRESS, DDC_REGS_READ_TRAINING_CONFIGURATION_PHY_BASED_SET(phy_based_delay_adj) | DDC_REGS_READ_TRAINING_CONFIGURATION_GATE_ADJ_DISABLE_SET(gate_adj_disable));

           mtc_write_training_basic_test(sample_cnt,read_wr_num,adj_step,pass_criteria,mov_window,byte_cnt); // LPDDR4
           check_write_training_stats(sample_cnt,initial_delay,mov_window,adj_step,pass_criteria,read_wr_num,sample_break,1,0,force_fail);

           break;

        /// -----------------------------  Rank 0 only tests -------------------------------------------------------
       case 43 :   // Write training with left movement ( Rank 0 only ) 
           program_single_rank_config();

           sample_cnt = 1;  
           sample_break = 0;
           read_wr_num = 2; 
           adj_step = 5; 
           pass_criteria = 2; 
           byte_cnt = 1;
           for( train_count=0; train_count < 5; train_count++) {
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA); 
#else 
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA); 
#endif 

          // This will cause right comparisons to fail and left comparisons to pass, causing left movement 
             for (i=0;i < NUM_SLICES;i++) {
               set_phy_delay_write_reg(i,initial_delay[i],0);
             }
             mtc_write_training_basic_test(sample_cnt,read_wr_num,adj_step,pass_criteria,mov_window,byte_cnt); // LPDDR4
             check_write_training_stats(sample_cnt,initial_delay,mov_window,adj_step,pass_criteria,read_wr_num,sample_break,1,0,force_fail);
             simulation_wait_ns(10000);                    
           }
           break;

       case 44 :   // Read training with left movement  ( Rank 0 only ) 
           read_methodA_init();
           program_single_rank_config();
           sample_cnt = 1;  
           read_wr_num = 2; 
           adj_step = 1; 
           pass_criteria = 2; 
           mov_window=get_freq_based_delay(MVM_DELTA); 
           byte_cnt = 1;
           for( train_count=0; train_count < 5; train_count++) {
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA); 
#else 
           initial_delay[0] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA); 
#endif 
             for (i=0;i <NUM_READ_DELAYS;i++) {
               for (j=0;j <NUM_SLICES;j++) {
                 set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
               }
             }
             sample_break= 0;
             mtc_read_training_basic_test(sample_cnt,read_wr_num,adj_step,pass_criteria,mov_window,byte_cnt); // LPDDR4
             check_read_training_stats(sample_cnt,initial_delay,mov_window,adj_step,pass_criteria,read_wr_num,sample_break,1,0, force_fail);
             simulation_wait_ns(10000);                    
           }
           break;

       case 45 :   
           program_single_rank_config();
           Disabling_PUB_mode();
           for( train_count=0; train_count < 1; train_count++) {
              mtc_zqcal_training_test_lpddr4_new_requirement(0,3); // MPC 2 step
              simulation_wait_ns(10000);  // amke sure no rank 1 activity , this gives the checkers time 
           }
           break;

       case 46 :  
           program_single_rank_config();
           Disabling_PUB_mode();
           read_methodA_init();
           mtc_zqcal_wr_rd_phy_combo_training_test_lpddr4_rank0_only(0,2);
           simulation_wait_ns(10000);  // amke sure no rank 1 activity , this gives the checkers time 
           break;

      case 47 :
           read_methodA_init();
           gate_adj_disable  = 1;
           phy_based_delay_adj = 0;
           hw_write32((uint32_t) MCU_REGS_DDC_REGS_READ_TRAINING_CONFIGURATION_ADDRESS, DDC_REGS_READ_TRAINING_CONFIGURATION_PHY_BASED_SET(phy_based_delay_adj) |
                   DDC_REGS_READ_TRAINING_CONFIGURATION_GATE_ADJ_DISABLE_SET(gate_adj_disable));
           sample_cnt = 2;  
           read_wr_num = 2; 
           adj_step = 1; 
           pass_criteria = 2; 
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           mov_window=get_freq_based_delay(MVM_DELTA); 
#else 
           initial_delay[0] = get_freq_based_golden_read_delay();
           initial_delay[1] = get_freq_based_golden_read_delay();
           mov_window = 1; 
#endif 
           sample_break = 0;
           byte_cnt = 1;
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
                set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
             } 
           }
           simulation_wait_ns(100);                    
           mtc_read_training_basic_test(sample_cnt,read_wr_num,adj_step,pass_criteria,mov_window,byte_cnt); // LPDDR4
           check_read_training_stats(sample_cnt,initial_delay,mov_window,adj_step,pass_criteria,read_wr_num,sample_break,1,0, force_fail);
           break;

       // DQ delays 
       case 48 : 
           mov_window = get_freq_based_delay(MVM_DELTA);
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_wr[i][j] = 16;// 80ps (LEFT MOV)
             }
           }
           create_write_training_delay_testcase(mov_window);
           break;

       case 49 : 
           mov_window = get_freq_based_delay(MVM_DELTA);
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_wr[i][j] = 32; // 160ps (LEFT MOV) 
             }
           }
           create_write_training_delay_testcase(mov_window);
           break;

       case 50 : 
           mov_window = get_freq_based_delay(MVM_DELTA);
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_wr[i][j] = 40; // 200ps (LEFT MOV)
             }
           }
           if (active_ranks == 1)
           {   
               program_single_rank_config();
               set_rank_id(0);
           }
           create_write_training_delay_testcase(mov_window);
           break;

       case 51 : 
           mov_window = get_freq_based_delay(MVM_DELTA);
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_wr[i][j] = 8; // 40ps (PASS)
             }
           }
           create_write_training_delay_testcase(mov_window);
           break;

       case 52 : 
           mov_window = get_freq_based_delay(MVM_DELTA);
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_wr[i][j] = 4; // 20ps  (PASS)
             }
           }
           create_write_training_delay_testcase(mov_window);
           break;

       case 53 : 
           mov_window = MVM_DELTA+2;
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_wr[i][j] = 12; // 60ps  (LEFT MOV)
             }
           }
           create_write_training_delay_testcase(mov_window);
           break;

       case 54 : 
           program_single_rank_config();
        //sample_cnt, read_wr_num,adj_step,pass_criteria, mov_window,byte_cnt

        for (mov_window = 5; mov_window < MOV_WINDOW_MAX - MVM_DELTA ; mov_window = mov_window+5) {
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+get_freq_based_delay(MVM_DELTA);
#else 
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1);
#endif 
           for (i=0;i < NUM_SLICES;i++) {
             set_phy_delay_write_reg(i,initial_delay[i],0);
           }
           mtc_write_training_basic_test(3,2,1,2,mov_window,1); // LPDDR4
           check_write_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0,force_fail);
        }
           break;

       case 55 : 
        program_single_rank_config();
        read_methodA_init();
        for (mov_window = 5; mov_window < MOV_WINDOW_MAX - MVM_DELTA ; mov_window = mov_window+5) {

#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
#else 
           initial_delay[0] = get_freq_based_golden_read_delay()+1;
           initial_delay[1] = get_freq_based_golden_read_delay()+1;
#endif 
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
               set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
             }
           }
           mtc_read_training_basic_test(3,2,1,2,mov_window,1);  // LPDDR4
           check_read_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0, force_fail);
        }
           break;

       case 56 : 
        //sample_cnt, read_wr_num,adj_step,pass_criteria, mov_window,byte_cnt
        program_single_rank_config();

        for (mov_window = 5; mov_window < MOV_WINDOW_MAX ; mov_window = mov_window+10) {
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+mov_window;
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+mov_window;
#else 
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1);
#endif 
           for (i=0;i < NUM_SLICES;i++) {
             set_phy_delay_write_reg(i,initial_delay[i],0);
           }
           mtc_write_training_basic_test(3,2,1,2,mov_window,1); // LPDDR4
           check_write_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0,force_fail);
        }
           break;

       case 57 : 
        program_single_rank_config();
        read_methodA_init();
        for (mov_window = 5; mov_window < MOV_WINDOW_MAX ; mov_window = mov_window+10) {

#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay()+mov_window;
           initial_delay[1] = get_freq_based_golden_read_delay()+mov_window;
#else 
           initial_delay[0] = get_freq_based_golden_read_delay()+1;
           initial_delay[1] = get_freq_based_golden_read_delay()+1;
#endif 
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
               set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
             }
           }
           mtc_read_training_basic_test(3,2,1,2,mov_window,1);  // LPDDR4
           check_read_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0, force_fail);
        }
           break;

       case 58 : 
         if (active_ranks == 1)
         {   
             program_single_rank_config();
             set_rank_id(0);
         }
        //sample_cnt, read_wr_num,adj_step,pass_criteria, mov_window,byte_cnt

        for (mov_window = 5; mov_window < (MOV_WINDOW_MAX - MVM_DELTA) ; mov_window = mov_window+5) {
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+get_freq_based_delay(MVM_DELTA);
#else 
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1);
#endif 
           for (i=0;i < NUM_SLICES;i++) {
             set_phy_delay_write_reg(i,initial_delay[i],0);
             if (active_ranks == 3)
             {
               set_phy_delay_write_reg(i,initial_delay[i],1);
             }
           }
           mtc_write_training_basic_test_no_polling(3,2,1,2,mov_window,1); // LPDDR4
           dont_program_training_regs_for_write = 1;
           poll_status_reg_for_write_training_completion(/*global_hold_ctrl*/0);
           check_write_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0,force_fail);
           if (active_ranks == 3)
           {
             hw_status("Polling for second rank training mov_window=%xH",mov_window);
             poll_status_reg_for_write_training_completion(/*global_hold_ctrl*/0);
             check_write_training_stats(3,initial_delay,mov_window,1,2,2,0,1,1,force_fail);
           }
           dont_program_training_regs_for_write = 0;
        }
           break;

       case 59 : 
         if (active_ranks == 1)
         {   
             program_single_rank_config();
             set_rank_id(0);
         }
         read_methodA_init();
         if (active_ranks == 1)
         {   
             program_single_rank_config();
             set_rank_id(0);
         }
        for (mov_window = 5; mov_window < (MOV_WINDOW_MAX - MVM_DELTA) ; mov_window = mov_window+5) {

#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
           initial_delay[1] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
#else 
           initial_delay[0] = get_freq_based_golden_read_delay()+1;
           initial_delay[1] = get_freq_based_golden_read_delay()+1;
#endif 
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
               set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
               if (active_ranks == 3)
               {
                 set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],1);
               }
             }
           }
           mtc_read_training_basic_test_no_polling(3,2,1,2,mov_window,1);  // LPDDR4
           dont_program_training_regs_for_read = 1;
           poll_status_reg_for_read_training_completion(/*global_hold_ctrl*/0);
           check_read_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0, force_fail);
           if (active_ranks == 3)
           {
             poll_status_reg_for_read_training_completion(/*global_hold_ctrl*/0);
             check_read_training_stats(3,initial_delay,mov_window,1,2,2,0,1,1, force_fail);
           }
           dont_program_training_regs_for_read = 0;
        }
           break;

       case 60 : 
         if (active_ranks == 1)
         {   
             program_single_rank_config();
             set_rank_id(0);
         }
        //sample_cnt, read_wr_num,adj_step,pass_criteria, mov_window,byte_cnt

        //for (mov_window = 5; mov_window < 15 ; mov_window = mov_window+10) {
        for (mov_window = 5; mov_window < MOV_WINDOW_MAX ; mov_window = mov_window+10) {
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+mov_window;
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+mov_window;
#else 
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1);
#endif 
           for (i=0;i < NUM_SLICES;i++) {
             set_phy_delay_write_reg(i,initial_delay[i],0);
             if (active_ranks == 3)
             {
               set_phy_delay_write_reg(i,initial_delay[i],1);
             }
           }
           mtc_write_training_basic_test_no_polling(3,2,1,2,mov_window,1); // LPDDR4
           dont_program_training_regs_for_write = 1;
           poll_status_reg_for_write_training_completion(/*global_hold_ctrl*/0);
           check_write_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0,force_fail);
           if (active_ranks == 3)
           {
             poll_status_reg_for_write_training_completion(/*global_hold_ctrl*/0);
             check_write_training_stats(3,initial_delay,mov_window,1,2,2,0,1,1,force_fail);
           }
           dont_program_training_regs_for_write = 0;
        }
           break;

       case 61 : 
        if (active_ranks == 1)
        {   
            program_single_rank_config();
            set_rank_id(0);
        }
        read_methodA_init();
        if (active_ranks == 1)
        {   
            program_single_rank_config();
            set_rank_id(0);
        }
        for (mov_window = 5; mov_window < MOV_WINDOW_MAX ; mov_window = mov_window+10) {

#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay()+mov_window;
           initial_delay[1] = get_freq_based_golden_read_delay()+mov_window;
#else 
           initial_delay[0] = get_freq_based_golden_read_delay()+1;
           initial_delay[1] = get_freq_based_golden_read_delay()+1;
#endif 
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
               set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
               if (active_ranks == 3)
               {
                 set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],1);
               }
             }
           }
           mtc_read_training_basic_test_no_polling(3,2,1,2,mov_window,1);  // LPDDR4
           dont_program_training_regs_for_read = 1;
           poll_status_reg_for_read_training_completion(/*global_hold_ctrl*/0);
           check_read_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0, force_fail);
           poll_status_reg_for_read_training_completion(/*global_hold_ctrl*/0);
           if (active_ranks == 3)
           {
             check_read_training_stats(3,initial_delay,mov_window,1,2,2,0,1,1, force_fail);
             dont_program_training_regs_for_read = 0;
           }
        }
           break;

       case 62 : 
        //sample_cnt, read_wr_num,adj_step,pass_criteria, mov_window,byte_cnt

         if (active_ranks == 1)
         {   
             program_single_rank_config();
             set_rank_id(0);
         }

#ifdef CONF_HAS___PHY_RTL
       initial_delay[0] = get_freq_based_golden_write_delay(0, 0)+get_freq_based_delay(MVM_DELTA);
       initial_delay[1] = get_freq_based_golden_write_delay(0, 1)+get_freq_based_delay(MVM_DELTA);
#else 
       initial_delay[0] = get_freq_based_golden_write_delay(0, 0);
       initial_delay[1] = get_freq_based_golden_write_delay(0, 1);
#endif 
         
        for(j=0; i < NUM_RANKS; j++) {
          for (i=0;i < NUM_SLICES;i++) {
             global_final_adj_values[j][i] = initial_delay[i];
             global_final_adj_values[j][i] = initial_delay[i];
          }
        }

        for (train_count = 0; train_count < 8 ; train_count++) {
           mov_window = rand_interval(1,63);
           for (i=0;i < NUM_SLICES;i++) {
             set_phy_delay_write_reg(i,global_final_adj_values[0][i],0);
             if (active_ranks == 3)
             {
               set_phy_delay_write_reg(i,global_final_adj_values[1][i],1);
             }
           }
           mtc_write_training_basic_test_no_polling(3,2,1,2,mov_window,1); // LPDDR4
           dont_program_training_regs_for_write = 1;
           poll_status_reg_for_write_training_completion(/*global_hold_ctrl*/0);
           check_write_training_stats(3,global_final_adj_values[0],mov_window,1,2,2,0,1,0,force_fail);
           if (active_ranks == 3)
           {
             hw_status("Polling for second rank training mov_window=%xH",mov_window);
             poll_status_reg_for_write_training_completion(/*global_hold_ctrl*/0);
             check_write_training_stats(3,global_final_adj_values[1],mov_window,1,2,2,0,1,1,force_fail);
           }
           dont_program_training_regs_for_write = 0;
        }
           break;

       case 63 : 
         if (active_ranks == 1)
         {   
             program_single_rank_config();
             set_rank_id(0);
         }
         read_methodA_init();
         if (active_ranks == 1)
         {   
             program_single_rank_config();
             set_rank_id(0);
         }


#ifdef CONF_HAS___PHY_RTL
        initial_delay[0] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
        initial_delay[1] = get_freq_based_golden_read_delay()+get_freq_based_delay(MVM_DELTA);
#else 
        initial_delay[0] = get_freq_based_golden_read_delay()+1;
        initial_delay[1] = get_freq_based_golden_read_delay()+1;
#endif 


        for (train_count = 0; train_count < 2 ; train_count++) {
 
           mov_window = rand_interval(1,63);
           for (i=0;i <NUM_READ_DELAYS;i++) {
             for (j=0;j <NUM_SLICES;j++) {
               set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0);
               if (active_ranks == 3)
               {
                 set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],1);
               }
             }
           }
           mtc_read_training_basic_test_no_polling(3,2,1,2,mov_window,1);  // LPDDR4
           dont_program_training_regs_for_read = 1;
           poll_status_reg_for_read_training_completion(/*global_hold_ctrl*/0);
           check_read_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0, force_fail);
           if (active_ranks == 3)
           {
             poll_status_reg_for_read_training_completion(/*global_hold_ctrl*/0);
             check_read_training_stats(3,initial_delay,mov_window,1,2,2,0,1,1, force_fail);
           }
           dont_program_training_regs_for_read = 0;
        }
           break;

       // DQS delays 
       case 64 : 
           //right_move_delay_predict_wr = 1;
           mov_window = get_freq_based_delay(MVM_DELTA);
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_wr[i][j] = -16; // 80ps  (RIGHT MOV)
             }
           }
           //global_wire_delay_reg_wr = 16;// 80ps (RIGHT MOV)
           create_write_training_delay_testcase(mov_window);
           break;

       case 65 : 
           //right_move_delay_predict_wr = 1;
           mov_window = get_freq_based_delay(MVM_DELTA);
           //global_wire_delay_reg_wr = 32; // 160ps (RIGHT MOV) 
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_wr[i][j] = -32; // 160ps  (RIGHT MOV)
             }
           }
           create_write_training_delay_testcase(mov_window);
           break;

       case 66 : 
           //right_move_delay_predict_wr = 1;
           mov_window = get_freq_based_delay(MVM_DELTA);
           //global_wire_delay_reg_wr = 40; // 200ps (RIGHT MOV)
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_wr[i][j] = -40; // 200ps  (RIGHT MOV)
             }
           }
           create_write_training_delay_testcase(mov_window);
           break;

       case 67 : 
           //right_move_delay_predict_wr = 1;
           mov_window = get_freq_based_delay(MVM_DELTA);
           //global_wire_delay_reg_wr = 8; // 40ps (PASS)
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_wr[i][j] = -8; // 40ps  (PASS)
             }
           }
           create_write_training_delay_testcase(mov_window);
           break;

       case 68 : 
           //right_move_delay_predict_wr = 1;
           mov_window = get_freq_based_delay(MVM_DELTA);
           //global_wire_delay_reg_wr = 4; // 20ps  (PASS)
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_wr[i][j] = -4; // 20ps  (PASS)
             }
           }
           create_write_training_delay_testcase(mov_window);
           break;

       case 69 : 
           //right_move_delay_predict_wr = 1;
           mov_window = MVM_DELTA+2;
           //global_wire_delay_reg_wr = 12; // 60ps (RIGHT MOV)
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_wr[i][j] = -12; // 60ps  (RIGHT MOV)
             }
           }
           create_write_training_delay_testcase(mov_window);
           break;

       // DQ delays 
       case 70 : 
           //right_move_delay_predict_rd = 1;
           mov_window = get_freq_based_delay(MVM_DELTA);
           //global_wire_delay_reg_rd = 16;// 80ps (LEFT MOV)
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_rd[i][j] = -16; // 80ps  (RIGHT MOV)
             }
           }
           create_read_training_delay_testcase(mov_window);
           break;

       case 71 : 
           //right_move_delay_predict_rd = 1;
           mov_window = get_freq_based_delay(MVM_DELTA);
           //global_wire_delay_reg_rd = 32; // 160ps (LEFT MOV) 
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_rd[i][j] = -32; // 160ps  (RIGHT MOV)
             }
           }
           create_read_training_delay_testcase(mov_window);
           break;

       case 72 : 
           //right_move_delay_predict_rd = 1;
           mov_window = get_freq_based_delay(MVM_DELTA);
           //global_wire_delay_reg_rd = 40; // 200ps (LEFT MOV)
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_rd[i][j] = -40; // 200ps  (RIGHT MOV)
             }
           }
           create_read_training_delay_testcase(mov_window);
           break;

       case 73 : 
           //right_move_delay_predict_rd = 1;
           mov_window = get_freq_based_delay(MVM_DELTA);
           //global_wire_delay_reg_rd = 8; // 40ps (PASS)
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_rd[i][j] = -8; // 40ps  (PASS)
             }
           }
           create_read_training_delay_testcase(mov_window);
           break;

       case 74 : 
           //right_move_delay_predict_rd = 1;
           mov_window = get_freq_based_delay(MVM_DELTA);
           //global_wire_delay_reg_rd = 4; // 20ps  (PASS)
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_rd[i][j] = -4; // 20ps  (PASS)
             }
           }
           create_read_training_delay_testcase(mov_window);
           break;

       case 75 : 
           //right_move_delay_predict_rd = 1;
           mov_window = MVM_DELTA+2;
           //global_wire_delay_reg_rd = 12; // 60ps (LEFT MOV)
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_rd[i][j] = -12; // 60ps  (RIGHT MOV)
             }
           }
           create_read_training_delay_testcase(mov_window);
           break;

       // DQS delays 
       case 76 : 
           mov_window = get_freq_based_delay(MVM_DELTA);
           //global_wire_delay_reg_rd = 16;// 80ps (RIGHT MOV)
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_rd[i][j] = 16; // 80ps  (LEFT MOV)
             }
           }
           create_read_training_delay_testcase(mov_window);
           break;

       case 77 : 
           mov_window = get_freq_based_delay(MVM_DELTA);
           //global_wire_delay_reg_rd = 32; // 160ps (RIGHT MOV) 
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_rd[i][j] = 32; // 160ps  (LEFT MOV)
             }
           }
           create_read_training_delay_testcase(mov_window);
           break;

       case 78 : 
           mov_window = get_freq_based_delay(MVM_DELTA);
           //global_wire_delay_reg_rd = 40; // 200ps (RIGHT MOV)
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_rd[i][j] = 40; // 200ps  (LEFT MOV)
             }
           }
           create_read_training_delay_testcase(mov_window);
           break;

       case 79 : 
           mov_window = get_freq_based_delay(MVM_DELTA);
           //global_wire_delay_reg_rd = 8; // 40ps (PASS)
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_rd[i][j] = 8; // 40ps  (PASS)
             }
           }
           create_read_training_delay_testcase(mov_window);
           break;

       case 80 : 
           mov_window = get_freq_based_delay(MVM_DELTA);
           //global_wire_delay_reg_rd = 4; // 20ps  (PASS)
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_rd[i][j] = 4; // 20ps  (PASS)
             }
           }
           create_read_training_delay_testcase(mov_window);
           break;

       case 81 : 
           mov_window = MVM_DELTA+2;
           //global_wire_delay_reg_rd = 12; // 60ps (RIGHT MOV)
           for (i=0; i<NUM_RANKS; i++) {
             for (j=0; j<NUM_SLICES; j++) {
               global_wire_delay_reg_rd[i][j] = 12; // 60ps  (LEFT MOV)
             }
           }
           create_read_training_delay_testcase(mov_window);
           break;

       default : 
           hw_status("ERROR: Invalid test number: %xH\n",mtc_test_num);
    }

    hw_status("Exiting %s\n", __func__);

}

// -------------------------------------
// create_write_training_delay_testcase()
// -------------------------------------
void  create_write_training_delay_testcase(int mov_window){

int initial_delay[NUM_SLICES];
int i;

           //sample_cnt, read_wr_num,adj_step,pass_criteria, mov_window,byte_cnt
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1);
#else
           initial_delay[0] = get_freq_based_golden_write_delay(0, 0);
           initial_delay[1] = get_freq_based_golden_write_delay(0, 1);
           mov_window = 1;
#endif
           for (i=0;i < NUM_SLICES;i++) {
             set_phy_delay_write_reg(i,initial_delay[i],0);
           }
           mtc_write_training_basic_test(3,2,1,2,mov_window,1); // LPDDR4

           check_write_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0,force_fail);

}

// -------------------------------------
// create_read_training_delay_testcase()
// -------------------------------------
void  create_read_training_delay_testcase(int mov_window){

int initial_delay[NUM_SLICES];
int i,j;

           //sample_cnt, read_wr_num,adj_step,pass_criteria, mov_window,byte_cnt
#ifdef CONF_HAS___PHY_RTL
           initial_delay[0] = get_freq_based_golden_read_delay();
           initial_delay[1] = get_freq_based_golden_read_delay();
#else
           initial_delay[0] = get_freq_based_golden_read_delay();
           initial_delay[1] = get_freq_based_golden_read_delay();
           mov_window = 1;
#endif

           for (i=0;i < NUM_READ_DELAYS ;i++) {
            for (j=0;j < NUM_SLICES ;j++) {
              set_phy_delay_read_reg(i*MAX_SLICES+j,initial_delay[j],0); // rank0
            }
           }
        
           mtc_read_training_basic_test(3,2,1,2,mov_window,1); // LPDDR4

           check_read_training_stats(3,initial_delay,mov_window,1,2,2,0,1,0, force_fail);

}
