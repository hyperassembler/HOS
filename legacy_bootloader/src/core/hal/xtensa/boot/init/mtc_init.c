#include <mtc_init.h>
#include <mtc_train.h>
#include "conf_defs.h"
#include <math.h>
#include <assert.h>


// ------------------
// Helper functions
// ------------------
unsigned int rand_interval(unsigned int min, unsigned int max)
{
    int r;
    const unsigned int range = 1 + max - min;
    const unsigned int buckets = RAND_MAX / range;
    const unsigned int limit = buckets * range;

    /* Create equal size buckets all in a row, then fire randomly towards
     * the buckets until you land in one of them. All buckets are equally
     * likely. If you land off the end of the line of buckets, try again. */
    do
    {
        r = rand();
    } while (r >= limit);

    return (unsigned int) min + (r / buckets);
}


// -------------------------
//  read_methodA_init()
// -------------------------
void read_methodA_init() {

int phy_reg_address;
int read_value;
int write_val;
int i,j;

/*  -- Section 7.2 ---------
MTC_TRAINING_CTRL.rd_tr_shadow_regs_only =1;
READ_TRAINING_CONFIGURATION.gate_adjust_disable = 1;
*/
  //  DXNGCR3.RGSLVT = 0; DXNGCR3.RDLVT = 0; DXNGCR3.RGLVT = 0;
  for (i=0;i < NUM_RANKS;i++) {
     for (j=0;j < NUM_SLICES;j++) {
        set_dxngcr3_read_training_disable_drift_compensation(j,i);
     }
  }
    // Program READ_TRAINING_CONFIGURATION register 
    hw_write32((uint32_t *) MCU_REGS_DDC_REGS_READ_TRAINING_CONFIGURATION_ADDRESS,
                   DDC_REGS_READ_TRAINING_CONFIGURATION_PHY_BASED_SET(0) |
                   DDC_REGS_READ_TRAINING_CONFIGURATION_GATE_ADJ_DISABLE_SET(1));

   // DQSDR0.DFTDTEN = 1'b0;
    phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DQSDR0_ADDRESS;
    // read modified write operation
    read_value = hw_phy_read32((uint32_t *)  phy_reg_address);
    write_val = PHY_REGS_DWC_DDRPHY_PUB_DQSDR0_DFTDTEN_MODIFY(read_value,0);
    printf("Programming set_dqsdr0.dftden=0  Addr = %xH , Data = %xH\n",phy_reg_address,read_value);

    hw_phy_write32((uint32_t *)  phy_reg_address, write_val);
    hw_sleep(15);
}

// ---------------------
// read_methodB_init
// ---------------------
int write_val,rd_val;
int tmp_val;
int gap_set;

void read_methodB_init(const int default_mtc_setup) {
//1.                   DQSDR0.DFTDTEN=1 (enable dqs drift detection)
//2.                   DQSDR0.DFTUPMODE=1

     rd_val = hw_phy_read32((uint32_t *)  MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DQSDR0_ADDRESS);
     tmp_val = PHY_REGS_DWC_DDRPHY_PUB_DQSDR0_DFTDTEN_MODIFY(rd_val,1);
     write_val = PHY_REGS_DWC_DDRPHY_PUB_DQSDR0_DFTUPMODE_MODIFY(tmp_val,1);
     hw_phy_write32((uint32_t *)  MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DQSDR0_ADDRESS, write_val);

//3.                   DSGCR.PUREN=0 (disable phy update for VT drift, use MC update instead)
//4.                   DSGCR.PUAD = 8 
//5.                   DSGCR.CTLZUEN = 1
//6.                   DSGCR.PHYZUEN = 0
     rd_val = hw_read32((uint32_t *)  MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DSGCR_ADDRESS);
     tmp_val = PHY_REGS_DWC_DDRPHY_PUB_DSGCR_PUAD_MODIFY(rd_val,8);
     write_val = PHY_REGS_DWC_DDRPHY_PUB_DSGCR_CTLZUEN_MODIFY(tmp_val,1);
     tmp_val =  PHY_REGS_DWC_DDRPHY_PUB_DSGCR_PHYZUEN_MODIFY(write_val,0);
     write_val = PHY_REGS_DWC_DDRPHY_PUB_DSGCR_PUREN_MODIFY(tmp_val,0);
     hw_phy_write32((uint32_t *)  MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DSGCR_ADDRESS, write_val);

//7.                   DQSDR1.DFTRDIDLC =0 (no phy inserted traffic)
//8.                   DQSDR1.DFTRDB2BC =0 (disable interrupt of b2b read)
     rd_val = hw_read32((uint32_t *)  MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DQSDR1_ADDRESS);
     tmp_val = PHY_REGS_DWC_DDRPHY_PUB_DQSDR1_DFTRDIDLC_MODIFY(rd_val,0);
     write_val = PHY_REGS_DWC_DDRPHY_PUB_DQSDR1_DFTRDB2BC_MODIFY(tmp_val,0);
     hw_phy_write32((uint32_t *)  MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DQSDR1_ADDRESS, write_val);

     
     if (!default_mtc_setup) {
//9.                   DQSDR2.DFTTHRESH = 7 (drift threshold)
//10.                DQSDR2.DFTMNTPRD = 0xffff (80us)
         rd_val = hw_read32((uint32_t *)  MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DQSDR2_ADDRESS);
         tmp_val = PHY_REGS_DWC_DDRPHY_PUB_DQSDR2_DFTMNTPRD_MODIFY(rd_val, 0xffff);
         write_val = PHY_REGS_DWC_DDRPHY_PUB_DQSDR2_DFTTHRSH_MODIFY(tmp_val,7);
         hw_phy_write32((uint32_t *)  MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DQSDR2_ADDRESS, write_val);
     } else {

//Changed these values as per Sherry's request to create more phy update events.
//9.                   DQSDR2.DFTTHRESH = 1 (drift threshold)
//10.                DQSDR2.DFTMNTPRD = 0x320 (1us)
         rd_val = hw_read32((uint32_t *)  MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DQSDR2_ADDRESS);
         tmp_val = PHY_REGS_DWC_DDRPHY_PUB_DQSDR2_DFTMNTPRD_MODIFY(rd_val, 0x320);
         write_val = PHY_REGS_DWC_DDRPHY_PUB_DQSDR2_DFTTHRSH_MODIFY(tmp_val,1);
         hw_phy_write32((uint32_t *)  MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DQSDR2_ADDRESS, write_val);
     }

//11.                DQSDR0.DFTDLY = 2 (adjust taps)
     rd_val = hw_read32((uint32_t *)  MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DQSDR0_ADDRESS);
     write_val = PHY_REGS_DWC_DDRPHY_PUB_DQSDR0_DFTDLY_MODIFY(rd_val,2);
     hw_phy_write32((uint32_t *)  MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DQSDR0_ADDRESS, write_val);

     // Program READ_TRAINING_CONFIGURATION register 
     hw_write32((uint32_t *) MCU_REGS_DDC_REGS_READ_TRAINING_CONFIGURATION_ADDRESS,
                   DDC_REGS_READ_TRAINING_CONFIGURATION_PHY_BASED_SET(1) |
                   DDC_REGS_READ_TRAINING_CONFIGURATION_GATE_ADJ_DISABLE_SET(1));

     // program read gap
     gap_set = rand_interval(1,5);
     hw_write32((uint32_t *) MCU_REGS_DDC_REGS_MPC_READ_GAP_ADDRESS, DDC_REGS_MPC_READ_GAP_GAP_SET(gap_set));
     printf("Programming gap_set=%xH\n",gap_set);
}


// --------------------
// mtc_base_init()
// --------------------
void mtc_base_init(const int default_mtc_setup, const int min_tr_freq) {

int rd_val;
uint32_t rd_wr_num;
int r, s;

     gate_adj_disable  = 1;
     phy_based_delay_adj = 0;
     wdlvt_enable = 0;
     for (r=0; r<NUM_RANKS; r++)
     {
         for (s=0; s<NUM_SLICES; s++)
         {
             global_wire_delay_reg_wr[r][s] = 0;
             global_wire_delay_reg_rd[r][s] = 0;
         }
     }
     program_single_rank = 0;
     random_write_data = 0;
     dont_program_training_regs_for_write = 0;
     dont_program_training_regs_for_read = 0;


    // ----- MTC default test control variables --------
     if (default_mtc_setup) {
         global_sample_cnt = rand_interval(1,(min_tr_freq/2));
         //global_sample_cnt = 1;
         rd_wr_num = rand_interval(2, 5);
         //rd_wr_num = 2;
         hw_status("KAHALU_SIM_INIT: DEFAULT_MTC_SETUP: Programming READ_WRITE_NUM = %0d\n", rd_wr_num);
         hw_write32((uint32_t*)(MCU_REGS_DDC_REGS_READ_WRITE_NUM_ADDRESS), rd_wr_num);

     } else {
         global_sample_cnt = rand_interval(1,15);
         rd_wr_num = 5;
     }

     hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_SAMPLE_CNT_ADDRESS, global_sample_cnt);
     printf("KAHALU_SIM_INIT: DEFAULT_MTC_SETUP: global_sample_cnt = %xH \n",global_sample_cnt);

     hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_PASS_CRITERIA_ADDRESS, (rd_wr_num * global_sample_cnt/2));
     printf("KAHALU_SIM_INIT: DEFAULT_MTC_SETUP: pass_criteria = %xH \n", (rd_wr_num * global_sample_cnt/2));


     // Program ZQCAL values correctly for MTC spec ( Default reset values are not functional) 
     hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MPC_VALUE1_ADDRESS, 0x51); // from lpddr4 jedec spec
     hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MPC_VALUE2_ADDRESS, 0x4f); // from lpddr4 jedec spec
     hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT1_ADDRESS, 30/1.25); 
     hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WAIT2_ADDRESS, 0); 

     // Program ZQCAL break correctly wrt no of ranks 
     rd_val = hw_read32((uint32_t *)  MCU_REGS_CRB_HIER_REGS_ACTIVE_RANKS_ADDRESS);
     if ( CRB_HIER_REGS_ACTIVE_RANKS_ACTIVE_RANK_GET(rd_val) == 1 ) {
       hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_ZQ_CAL_MODE_ADDRESS, 
               DDC_REGS_ZQ_CAL_MODE_BREAK_SET(1) | DDC_REGS_ZQ_CAL_MODE_MODE_SET(2));
       global_zqcal_break =  1; // 1 rank 
     } else if ( CRB_HIER_REGS_ACTIVE_RANKS_ACTIVE_RANK_GET(rd_val) == 3 ){
        hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_ZQ_CAL_MODE_ADDRESS, 
               DDC_REGS_ZQ_CAL_MODE_BREAK_SET(0) | DDC_REGS_ZQ_CAL_MODE_MODE_SET(2));
       global_zqcal_break =  0; // both ranks 
     }
     printf(" global_zqcal_break = %xH \n",global_zqcal_break);

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
     hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_WRITE_EXP_DATA_8_ADDRESS, 0xa55a0ff0);
     

     // Enabling Controller PHY update ack
     rd_val = hw_phy_read32((uint32_t *)  MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DSGCR_ADDRESS);
     printf("DSGCR = %xH\n",rd_val);
     rd_val = rd_val | PHY_REGS_DWC_DDRPHY_PUB_DSGCR_CUAEN_SET(1);
     hw_phy_write32((uint32_t *) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DSGCR_ADDRESS, rd_val);

     printf("NEW DSGCR = %xH\n",rd_val);
     printf("\n");

     printf("PUB_PGCR3_GATEACCTLCLK = %xH\n",get_pub_gcr3_gateacctlclk());

}


// --------------------
// mtc_write_init_no_base_init()
// --------------------
void mtc_write_init_no_base_init(const int default_mtc_setup, const int min_tr_freq) {

    int i,j;
    int rd_val;
    uint32_t initial_delay[NUM_SLICES];


#ifdef CONF_HAS___PHY_RTL


    // WDQPRD programming 
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


#else

     for (i=0;i < NUM_RANKS;i++) {
      for (j=0;j < NUM_SLICES;j++) {
        dxngtr0_dgsl[i][j] = get_phy_dxngtr0_dgsl(j,i);
        dxngsr0_gdqsprd[i][j] = get_phy_dxngsr0_gdqsprd(j,i);
      }
     }

    // Using default values MTC spec 1.00 
    // Program WDLVT = 0 for MTC WRITE_TRAINING_CONFIG
    //hw_write32((uint32_t *) MCU_REGS_DDC_REGS_WRITE_TRAINING_CONFIG_ADDRESS,
    //                      DDC_REGS_WRITE_TRAINING_CONFIG_VALUE_SET(wdlvt_enable));

#endif

    hw_write32((uint32_t *) MCU_REGS_DDC_REGS_READ_TRAINING_CONFIGURATION_ADDRESS,
                         DDC_REGS_READ_TRAINING_CONFIGURATION_PHY_BASED_SET(phy_based_delay_adj) |
                         DDC_REGS_READ_TRAINING_CONFIGURATION_GATE_ADJ_DISABLE_SET(gate_adj_disable));

    trn_frq = rand_interval(1,5);
    printf("trn_frq = %xH\n",trn_frq);

}

// --------------------
// mtc_write_init()
// --------------------
void mtc_write_init(const int default_mtc_setup, const int min_tr_freq) {

    int i,j;
    int rd_val;
    uint32_t initial_delay[NUM_SLICES];

    mtc_base_init(default_mtc_setup, min_tr_freq);


#ifdef CONF_HAS___PHY_RTL


    // WDQPRD programming 
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


#else

     for (i=0;i < NUM_RANKS;i++) {
      for (j=0;j < NUM_SLICES;j++) {
        dxngtr0_dgsl[i][j] = get_phy_dxngtr0_dgsl(j,i);
        dxngsr0_gdqsprd[i][j] = get_phy_dxngsr0_gdqsprd(j,i);
      }
     }

    // Using default values MTC spec 1.00 
    // Program WDLVT = 0 for MTC WRITE_TRAINING_CONFIG
    //hw_write32((uint32_t *) MCU_REGS_DDC_REGS_WRITE_TRAINING_CONFIG_ADDRESS,
    //                      DDC_REGS_WRITE_TRAINING_CONFIG_VALUE_SET(wdlvt_enable));

#endif

    hw_write32((uint32_t *) MCU_REGS_DDC_REGS_READ_TRAINING_CONFIGURATION_ADDRESS,
                         DDC_REGS_READ_TRAINING_CONFIGURATION_PHY_BASED_SET(phy_based_delay_adj) |
                         DDC_REGS_READ_TRAINING_CONFIGURATION_GATE_ADJ_DISABLE_SET(gate_adj_disable));

    trn_frq = rand_interval(1,5);
    printf("trn_frq = %xH\n",trn_frq);

}

// ---------------------------------------------------------
// set_dxngcr3_read_training_disable_drift_compensation
// ---------------------------------------------------------
void set_dxngcr3_read_training_disable_drift_compensation(int dqs_no,int rankid){
int phy_reg_address;
int read_value;
int write_val;
int tmp_val;
   
    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GCR3_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR3_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2GCR3_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3GCR3_ADDRESS; break;
    }

    // read modified write operation
    read_value = hw_phy_read32((uint32_t *)  phy_reg_address);

    write_val = PHY_REGS_DWC_DDRPHY_PUB_DX0GCR3_RGSLVT_MODIFY(read_value,0);
    tmp_val =  PHY_REGS_DWC_DDRPHY_PUB_DX0GCR3_RDLVT_MODIFY(write_val,0);
    write_val = PHY_REGS_DWC_DDRPHY_PUB_DX1GCR3_RGLVT_MODIFY(tmp_val,0);

    printf("Programming set_dxngcr3_read_training_disable_drift_compensation Addr = %xH , Data = %xH\n",phy_reg_address,read_value);

    hw_phy_write32((uint32_t *)  phy_reg_address, write_val);
    hw_sleep(15);

}

// --------------------
// set_phy_dxngtr0()
// --------------------
void set_phy_dxngtr0(int dqs_no,int rankid, int write_value){
    int phy_reg_address;

    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GTR0_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2GTR0_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3GTR0_ADDRESS; break;
    }
    hw_phy_write32((uint32_t *)  phy_reg_address, write_value);
}

// ----------------------------------------------------------------
// program_non_default_regs_for_mtc_read_and_write_training_init()
// ----------------------------------------------------------------
void program_non_default_regs_for_mtc_read_and_write_training_init()
{

    mtc_write_init(/*default_mtc_setup*/0, /*min_tr_freq*/0);
    read_methodA_init();
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, 6);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR1_FREQUENCY_ADDRESS, 6);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR2_FREQUENCY_ADDRESS, 3);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR3_FREQUENCY_ADDRESS, 3);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0xc); // enable read and write training
}

// ----------------------------------------------------------------
// program_non_default_regs_for_mtc_init()
// ----------------------------------------------------------------
void program_non_default_regs_for_mtc_init
(
    const int training_en, 
    const int active_ranks,
    const int mtc_use_read_methodB
)
{
    int rd_tr_freq, wr_tr_freq, min_tr_freq;
    int rd_tr_sel;
    int wdqsl, write_value, slice;

    rd_tr_freq = rand_interval(4,10);
    wr_tr_freq = rand_interval(4,10);
    if (rd_tr_freq <= wr_tr_freq) {
        min_tr_freq = rd_tr_freq;
    } else {
        min_tr_freq = wr_tr_freq;
    }

    mtc_base_init(/*default_mtc_setup*/1, /*min_tr_freq*/min_tr_freq);

    for (slice=0; slice < NUM_SLICES; slice++) {
        // Get the value of WDQDPRD using algo in section 7.1.1
        wdqsl = get_phy_dxngtr0_wdqsl(/*dqs_no*/slice,/*rankid*/0);
        write_value  = PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_DGSL_SET(1) | PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_WDQSL_SET(wdqsl) 
                       | PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_WLSL_SET(1);
        set_mtc_dxngtr0(slice,/*rankid*/0,write_value);
        // Get the value of WDQDPRD using algo in section 7.1.1
        if (active_ranks == 3) {
            wdqsl = get_phy_dxngtr0_wdqsl(/*dqs_no*/slice,/*rankid*/1);
            write_value  = PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_DGSL_SET(1) | PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_WDQSL_SET(wdqsl) 
                       | PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_WLSL_SET(1);
            set_mtc_dxngtr0(slice,/*rankid*/1,write_value);
        }
    }

    if (training_en & 4) {
        hw_status("KAHALU_SIM_INIT: DEFAULT_MTC_SETUP: Programming MTC Write Init\n");
        hw_status("KAHALU_SIM_INIT: DEFAULT_MTC_SETUP: Write TR Frequency = %0d\n", wr_tr_freq);
        mtc_write_init_no_base_init(/*default_mtc_setup*/1, /*min_tr_freq*/min_tr_freq);
    }
    if (training_en & 8) {
        hw_status("KAHALU_SIM_INIT: DEFAULT_MTC_SETUP: Read TR Frequency = %0d\n", rd_tr_freq);
        if (!mtc_use_read_methodB) {
            hw_status("KAHALU_SIM_INIT: DEFAULT_MTC_SETUP: Programming MTC Read Init MethodA\n");
            read_methodA_init();
        } else {
            hw_status("KAHALU_SIM_INIT: DEFAULT_MTC_SETUP: Programming MTC Read Init MethodB\n");
            write_value = hw_read32((uint32_t *)  MCU_REGS_DDC_REGS_CONFIG_ADDRESS);
            write_value = DDC_REGS_CONFIG_ENABLE_PHY_INITIATED_UPDATE_MODIFY(write_value, 1);
            hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_CONFIG_ADDRESS, write_value);
            read_methodB_init(/*default_mtc_setup*/1);
        }
    }
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, 12);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR1_FREQUENCY_ADDRESS, 12);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR2_FREQUENCY_ADDRESS, rd_tr_freq);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR3_FREQUENCY_ADDRESS, wr_tr_freq);
    //hw_status("KAHALU_SIM_INIT: Enabling MTC RunTime Trainings: 0x%0x\n", training_en);
    //hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, training_en);
    if (active_ranks == 1) {
        program_single_rank = 1;
        set_rank_id(0);
    }
}

// ----------------------------------------------------------------
// en_mtc_trainings(const int training_en)
// ----------------------------------------------------------------
void en_mtc_trainings(const int training_en)
{

    hw_status("KAHALU_SIM_INIT: DEFAULT_MTC_SETUP: Enabling MTC RunTime Trainings: 0x%0x\n", training_en);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, training_en);
}

// ----------------------------------------------------------------
// program_non_default_regs_for_mtc_write_training_init()
// ----------------------------------------------------------------
void program_non_default_regs_for_mtc_write_training_init()
{

    mtc_write_init(/*default_mtc_setup*/0, /*min_tr_freq*/0);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, 6);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR1_FREQUENCY_ADDRESS, 6);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR2_FREQUENCY_ADDRESS, 3);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR3_FREQUENCY_ADDRESS, 3);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x4); // enable write training
}

// ----------------------------------------------------------------
// program_non_default_regs_for_mtc_read_training_init()
// ----------------------------------------------------------------
void program_non_default_regs_for_mtc_read_training_init()
{

    mtc_write_init(/*default_mtc_setup*/0, /*min_tr_freq*/0);
    read_methodA_init();
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR0_FREQUENCY_ADDRESS, 6);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR1_FREQUENCY_ADDRESS, 6);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR2_FREQUENCY_ADDRESS, 3);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TR3_FREQUENCY_ADDRESS, 3);
    hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS, 0x8); // enable read training
}


// -----------------------------------
// program_mtc_wr_training_window_size()
// -----------------------------------
void program_mtc_wr_training_window_size(int val) {
   int rdata;

   assert ( val < 64 && val > 0 );
   hw_status("%s: Programming WR mov_window_size = %0d\n", __func__, val);
   rdata = hw_read32((uint32_t *)  MCU_REGS_DDC_REGS_MOV_WINDOW_ADDRESS);
   rdata &= (~DDC_REGS_MOV_WINDOW_WINDOW_WR_FIELD_MASK);
   rdata |= (val << DDC_REGS_MOV_WINDOW_WINDOW_WR_LSB);
   hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MOV_WINDOW_ADDRESS, rdata);

}

// -----------------------------------
// program_mtc_rd_training_window_size()
// -----------------------------------
void program_mtc_rd_training_window_size(int val) {
   int rdata;

   assert ( val < 64 && val > 0 );
   hw_status("%s: Programming RD mov_window_size = %0d\n", __func__, val);
   rdata = hw_read32((uint32_t *)  MCU_REGS_DDC_REGS_MOV_WINDOW_ADDRESS);
   rdata &= (~DDC_REGS_MOV_WINDOW_WINDOW_RD_FIELD_MASK);
   rdata |= (val << DDC_REGS_MOV_WINDOW_WINDOW_RD_LSB);
   hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_MOV_WINDOW_ADDRESS, rdata);

}

// -----------------------------------
// program_mtc_training_adj_step()
// -----------------------------------
void program_mtc_training_adj_step(int val) {

  hw_write32((uint32_t *)  MCU_REGS_DDC_REGS_ADJ_STEP_ADDRESS, val);  

}

// ------------------- Register access functions ---------------------------------
// -------------
// set_rank_id
// -------------
void set_rank_id(int rankid) {
int write_value;
    write_value = PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_RANKWID_SET(rankid) |  PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_RANKRID_SET(rankid);
    hw_phy_write32((uint32_t *)  MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS, write_value);
    //hw_status("Setting rankid = %0d\n", rankid);
    //printf("Programming Addr = %xH , Data = %xH \n", MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS,write_value);
    hw_sleep(15);
}


// --------------------------------
// get_phy_delay_write_reg()
// --------------------------------
int get_phy_delay_write_reg(int dqs_no,int rankid){
int phy_reg_address;
    
    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR1_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2LCDLR1_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3LCDLR1_ADDRESS; break;
    }

    return hw_phy_read32((uint32_t *)  phy_reg_address);
}

// --------------------------------
// get_mtc_delay_write_reg()
// --------------------------------
int get_mtc_delay_write_reg(int dqs_no,int rankid){
int mtc_reg_address;
    
    set_rank_id(rankid);
    if ( rankid == 0 ) {
       switch(dqs_no) {
        case 0 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0LCDLR1_RANK0_ADDRESS; break;
        case 1 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR1_RANK0_ADDRESS; break;
      }
    } else {
       switch(dqs_no) {
        case 0 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0LCDLR1_RANK1_ADDRESS; break;
        case 1 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1LCDLR1_RANK1_ADDRESS; break;
       }
    }
    return hw_read32((uint32_t *)  mtc_reg_address);
}

// --------------------------------
// get_pub_gcr3_gateacctlclk()
// --------------------------------
int get_pub_gcr3_gateacctlclk(){
int phy_reg_address;
int read_value;
    
    phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR3_ADDRESS; 

    read_value = hw_phy_read32((uint32_t *)  phy_reg_address);
    printf("Reading pub_gcr3_gateacctl Addr = %xH , Data = %xH\n",phy_reg_address,read_value);
    return (PHY_REGS_DWC_DDRPHY_PUB_PGCR3_GATEACCTLCLK_GET(read_value));
}

// --------------------------------
// set_pub_gcr3_gateacctlclk()
// --------------------------------
void set_pub_gcr3_gateacctlclk(int value){
int phy_reg_address;
int read_value;
    
    phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR3_ADDRESS; 

    // read modified write operation 
    read_value = hw_phy_read32((uint32_t *)  phy_reg_address);

    read_value = read_value | PHY_REGS_DWC_DDRPHY_PUB_PGCR3_GATEACCTLCLK_SET(value);
    printf("Programming pub_gcr3_gateacctl Addr = %xH , Data = %xH\n",phy_reg_address,read_value);

    hw_phy_write32((uint32_t *)  phy_reg_address, read_value);
    hw_sleep(15);
}

// --------------------------------
// set_dxngcr3_wdlvt
// --------------------------------
void set_dxngcr3_wdlvt(int dqs_no,int rankid,int value){
int phy_reg_address;
int read_value;
    
    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GCR3_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR3_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2GCR3_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3GCR3_ADDRESS; break;
    }

    // read modified write operation 
    read_value = hw_phy_read32((uint32_t *)  phy_reg_address);

    read_value = read_value | PHY_REGS_DWC_DDRPHY_PUB_DX0GCR3_WDLVT_SET(value);
    printf("Programming set_dxngcr3_wdlvt Addr = %xH , Data = %xH\n",phy_reg_address,read_value);

    hw_phy_write32((uint32_t *)  phy_reg_address, read_value);
    hw_sleep(15);
}

// --------------------------------
// set_pubgcr3_pure()
// --------------------------------
void set_pubgcr3_pure(int value){
int phy_reg_address;
int read_value;
    
    phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DSGCR_ADDRESS; 

    // read modified write operation 
    read_value = hw_phy_read32((uint32_t *)  phy_reg_address);

    read_value = read_value | PHY_REGS_DWC_DDRPHY_PUB_DSGCR_PUREN_SET(value);
    printf("Programming set_pubgcr3_pure Addr = %xH , Data = %xH\n",phy_reg_address,read_value);

    hw_phy_write32((uint32_t *)  phy_reg_address, read_value);
    hw_sleep(15);
}

// --------------------------------
// set_crb_active_ranks()
// --------------------------------
void set_crb_active_ranks(int value){
int reg_address;
int read_value;
    
    reg_address = MCU_REGS_CRB_HIER_REGS_ACTIVE_RANKS_ADDRESS; 

    read_value = CRB_HIER_REGS_ACTIVE_RANKS_ACTIVE_RANK_SET(value);

    hw_write32((uint32_t *)  reg_address, read_value);
    printf("Programming CRB Addr = %xH , Data = %xH\n",reg_address,read_value);
}

// --------------------
// get_phy_dxngtr0_dgsl()
// --------------------
int get_phy_dxngtr0_dgsl(int dqs_no,int rankid){
    int rd_value;
    int phy_reg_address;

    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GTR0_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2GTR0_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3GTR0_ADDRESS; break;
    }
    rd_value =  hw_phy_read32((uint32_t *)  phy_reg_address);
    return ((rd_value & PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_DGSL_FIELD_MASK) >> PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_DGSL_LSB);
}

// --------------------
// get_phy_dxngtr0()
// --------------------
int get_phy_dxngtr0(int dqs_no,int rankid){
    int rd_value;
    int phy_reg_address;

    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GTR0_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2GTR0_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3GTR0_ADDRESS; break;
    }
    rd_value =  hw_phy_read32((uint32_t *)  phy_reg_address);
    return rd_value;
}

// --------------------
// get_phy_dxngtr0_wdqsl()
// --------------------
int get_phy_dxngtr0_wdqsl(int dqs_no,int rankid){
    int rd_value;
    int phy_reg_address;

    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GTR0_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2GTR0_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3GTR0_ADDRESS; break;
    }
    rd_value =  hw_phy_read32((uint32_t *)  phy_reg_address);
    hw_sleep(15);
    return ((rd_value & PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_WDQSL_FIELD_MASK) >> PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_WDQSL_LSB);
}

// --------------------
// get_dxnmdlr0_iprd()
// --------------------
int get_dxnmdlr0_iprd(int dqs_no,int rankid){
    int rd_value;
    int phy_reg_address;

    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0MDLR0_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1MDLR0_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2MDLR0_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3MDLR0_ADDRESS; break;
    }
    rd_value =  hw_phy_read32((uint32_t *)  phy_reg_address);
    return (PHY_REGS_DWC_DDRPHY_PUB_DX0MDLR0_IPRD_GET(rd_value));
}


// -------------------
// set_mtc_dxngsr0
// --------------------
void set_mtc_dxngsr0 (int dqs_no,int rankid,int value){
    int mtc_reg_address;

    switch(dqs_no) {
        case 0 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0GSR0_ADDRESS; break;
        case 1 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1GSR0_ADDRESS; break;
    }

    hw_write32((uint32_t *)  mtc_reg_address, value);
    printf("Programming MTC_DXNGSR0 Addr = %xH , Data = %xH\n",mtc_reg_address,value);
}

// --------------------
// get_mtc_dxngsr6()
// --------------------
// Read IPRD/TPRD value from MTC design 
int get_mtc_dxngsr6(int dqs_no,int rankid){
    int rd_value;
    int mtc_reg_address;

    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0GSR6_ADDRESS; break;
        case 1 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1GSR6_ADDRESS; break;
    }
    rd_value =  hw_read32((uint32_t *)  mtc_reg_address);
    return (DDC_REGS_MC_DX0GSR6_VALUE_GET(rd_value));
}

// -------------------
// set_mtc_dxngtr0
// --------------------
void set_mtc_dxngtr0 (int dqs_no,int rankid,int value){
    int mtc_reg_address;

    if ( rankid == 0 ) {
      switch(dqs_no) {
        case 0 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0GTR0_RANK0_ADDRESS; break;
        case 1 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1GTR0_RANK0_ADDRESS; break;
      }
    } else {
      switch(dqs_no) {
        case 0 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0GTR0_RANK1_ADDRESS; break;
        case 1 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1GTR0_RANK1_ADDRESS; break;
      }
    }

    hw_write32((uint32_t *)  mtc_reg_address, value);
    printf("Programming MC_DX%dGTR0_RANK%d Addr = %xH , Data = %xH\n", dqs_no, rankid, mtc_reg_address, value);
}

// --------------------
// get_mtc_dxngtr0()
// --------------------
int get_mtc_dxngtr0_dgsl(int dqs_no,int rankid){
    int rd_value;
    int mtc_reg_address;

    if ( rankid == 0 ) {
      switch(dqs_no) {
        case 0 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0GTR0_RANK0_ADDRESS; break;
        case 1 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1GTR0_RANK0_ADDRESS; break;
      }
    } else {
      switch(dqs_no) {
        case 0 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0GTR0_RANK1_ADDRESS; break;
        case 1 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1GTR0_RANK1_ADDRESS; break;
      }
    }
    rd_value =  hw_read32((uint32_t *)  mtc_reg_address);
    return (PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_DGSL_GET(rd_value)); 
}


// -------------------------
// get_phy_dxngsr0_gdqsprd()
// -------------------------
int get_phy_dxngsr0_gdqsprd(int dqs_no,int rankid){
    int rd_value;
    int phy_reg_address;

    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GSR0_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR0_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2GSR0_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3GSR0_ADDRESS; break;
    }
    rd_value =  hw_phy_read32((uint32_t *)  phy_reg_address);
    return ((rd_value & PHY_REGS_DWC_DDRPHY_PUB_DX0GSR0_GDQSPRD_FIELD_MASK) >> PHY_REGS_DWC_DDRPHY_PUB_DX0GSR0_GDQSPRD_LSB);
}

// -------------------------
// get_phy_dxngsr0()
// -------------------------
int get_phy_dxngsr0(int dqs_no,int rankid){
    int rd_value;
    int phy_reg_address;

    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GSR0_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR0_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2GSR0_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3GSR0_ADDRESS; break;
    }
    rd_value =  hw_phy_read32((uint32_t *)  phy_reg_address);
    return rd_value;
}

// ------------
// get_WDQPRD()
// ------------
int get_WDQPRD(int dqs_no,int rankid) {

int local0_iprd,local0_wdqd0_offset,local0_wdqsl0,local0_wld,local0_wlsl;
int tmp_var0;
int phy_reg_address;
int mtc_reg_address;
int write_val;
int r0_val;
int r1_val;
int gtr0_val;
int local0_wdqd1_offset,local0_wdqsl1;
int wdqprd;

// --- Complete algo from Spec 1.00 Section 7.1.1  -----
//1.      Read DX0MDLR0.IPRD => local0_iprd
local0_iprd = get_dxnmdlr0_iprd(dqs_no,rankid);

//2.      Read DX0LCDLR1.WDQD => local0_wdqd0_offset
    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR1_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2LCDLR1_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3LCDLR1_ADDRESS; break;
    }
    r1_val = hw_phy_read32((uint32_t *)  phy_reg_address);
    local0_wdqd0_offset =  PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_WDQD_GET(r1_val);
    printf("WDQPRD programming : local0_wdqd0_offset = %xH\n",local0_wdqd0_offset);

//3.      Read DX0GTR0.WDQDSL => local0_wdqsl0
   local0_wdqsl0 = get_phy_dxngtr0_wdqsl(dqs_no,rankid);
   printf("WDQPRD programming : local0_wdqsl0 = %xH\n",local0_wdqsl0);
    
//4.      Read DX0LCDLR0.WLD => local0_wld

    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR0_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR0_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2LCDLR0_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3LCDLR0_ADDRESS; break;
    }
    r0_val = hw_phy_read32((uint32_t *)  phy_reg_address);
    local0_wld =  PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR0_WLD_GET(r0_val);
    printf("WDQPRD programming : local0_wld = %xH\n",local0_wld);

//5.      Read DX0GTR0.WLSL => local0_wlsl
    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GTR0_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2GTR0_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3GTR0_ADDRESS; break;
    }
    gtr0_val =  hw_phy_read32((uint32_t *)  phy_reg_address);
    local0_wlsl = PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_WLSL_GET(gtr0_val);
    printf("WDQPRD programming : local0_wlsl = %xH\n",local0_wlsl);

//6.      Calculate tmp_var0 = (local0_wdqsl0*local0_iprd) + local0_wdqd0_offset
    tmp_var0 = (local0_wdqsl0*local0_iprd) + local0_wdqd0_offset;
    printf("WDQPRD programming : tmp_var0 = %xH\n",tmp_var0);

//7.      Program DX0LCDLR0.WLD = 0 
    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR0_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR0_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2LCDLR0_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3LCDLR0_ADDRESS; break;
    }
    r0_val = hw_phy_read32((uint32_t *)  phy_reg_address);
    write_val = PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR0_WLD_MODIFY(r0_val,0);

    printf("WDQPRD programming Addr %xH Data %xH: DX0LCDLR0.WLD\n",phy_reg_address,write_val);
    hw_phy_write32((uint32_t *)  phy_reg_address,write_val);

//8.      Program DX0GTR0.WLSL = 0
    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GTR0_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2GTR0_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3GTR0_ADDRESS; break;
    }
    write_val = PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_WLSL_MODIFY(gtr0_val,0);
    hw_phy_write32((uint32_t *)  phy_reg_address,write_val);
    printf("WDQPRD programming Addr %xH Data %xH:  DX0GTR0.WLSL\n",phy_reg_address,write_val);

//9.      Program DX0LCDLR1.WDQD = tmp_var0 
   set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR1_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2LCDLR1_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3LCDLR1_ADDRESS; break;
    }
    r1_val = hw_phy_read32((uint32_t *)  phy_reg_address);
    write_val =  PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_WDQD_MODIFY(r1_val,tmp_var0);
    hw_phy_write32((uint32_t *)  phy_reg_address,write_val);
    printf("WDQPRD programming Addr %xH Data %xH: DX0LCDLR1.WDQD\n",phy_reg_address,write_val);

//10.     Read DX0LCDLR1.WDQD => local0_wdqd1_offset
    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR1_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2LCDLR1_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3LCDLR1_ADDRESS; break;
    }
    r1_val = hw_phy_read32((uint32_t *)  phy_reg_address);
    local0_wdqd1_offset =  PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_WDQD_GET(r1_val);
    printf("WDQPRD programming : local0_wdqd1_offset = %xH\n",local0_wdqd1_offset);

//11.     Read DX0GTR0.WDQDSL => local0_wdqsl1
   local0_wdqsl1 = get_phy_dxngtr0_wdqsl(dqs_no,rankid);
    printf("WDQPRD programming : local0_wdqsl1 = %xH\n",local0_wdqsl1);

//12.     Program MC_DX0GSR6.WDQDPRD = (tmp_var0 - local0_wdqd1_offset)/ local0_wdqsl1
    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX0GSR6_ADDRESS; break;
        case 1 :  mtc_reg_address = MCU_REGS_DDC_REGS_MC_DX1GSR6_ADDRESS; break;
    }
    write_val = (tmp_var0 - local0_wdqd1_offset)/ local0_wdqsl1;
    wdqprd = write_val;
    hw_write32((uint32_t *)  mtc_reg_address,write_val);
    printf("WDQPRD programming : wdqprd actual = %xH\n",write_val);

//13.     Restore original LCDL value: program DX0LCDLR1.WDQD = local0_wdqsl0 * local0_wdqprd + local0_wdqd0_offset
    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR1_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2LCDLR1_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3LCDLR1_ADDRESS; break;
    }
    write_val  = PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_WDQD_MODIFY(r1_val,local0_wdqsl0 * local0_iprd + local0_wdqd0_offset);
    hw_phy_write32((uint32_t *)  phy_reg_address,write_val);
    printf("WDQPRD programming Addr %xH Data %xH Restore old DX0LCDLR1.WDQD\n",phy_reg_address,write_val);

//14.     Program DX0LCDLR0.WLD = local0_wld
    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR0_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR0_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2LCDLR0_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3LCDLR0_ADDRESS; break;
    }
    r0_val = hw_phy_read32((uint32_t *)  phy_reg_address);
    write_val = PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR0_WLD_MODIFY(r0_val,local0_wld);
    hw_phy_write32((uint32_t *)  phy_reg_address,write_val);
    printf("WDQPRD programming Addr %xH Data %xH: DX0LCDLR0.WLD \n",phy_reg_address,write_val);

//15.     Program DX0GTR0.WLSL = local0_wlsl
    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GTR0_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2GTR0_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3GTR0_ADDRESS; break;
    }
    write_val =  PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_WLSL_MODIFY(gtr0_val,local0_wlsl);
    hw_phy_write32((uint32_t *)  phy_reg_address,write_val);
    printf("WDQPRD programming Addr %xH Data %xH: DX0LCDLR0.WLSL \n",phy_reg_address,write_val);

//16.     double check: read back DX0LCDLR1.WDQD, it should be equal to local0_wdqd0_offset
    set_rank_id(rankid);
    switch(dqs_no) {
        case 0 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS; break;
        case 1 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR1_ADDRESS; break;
        case 2 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2LCDLR1_ADDRESS; break;
        case 3 :  phy_reg_address = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3LCDLR1_ADDRESS; break;
    }
    r1_val = hw_phy_read32((uint32_t *)  phy_reg_address);
    printf("WDQPRD programming : read back DX0LCDLR1.WDQD = %xH\n",(unsigned int) PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_WDQD_GET(r1_val));
     
    if (  PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_WDQD_GET(r1_val) != local0_wdqd0_offset ) {
        printf("WDQPRD programming : exp = %xH, actual = %xH\n",local0_wdqd0_offset,(unsigned int) PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_WDQD_GET(r1_val));
    }

     return wdqprd;
}
