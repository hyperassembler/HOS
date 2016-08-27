#ifndef MTC_INIT_H
#define MTC_INIT_H

#include "conf_defs.h"
#include <math.h>
#include <assert.h>

#define WDLY_MASK 0x1ff
#define NUM_SLICES 2
#define MAX_SLICES 4
#define NUM_RANKS 2
#define NUM_READ_DELAYS 3
#define MOV_WINDOW_MAX 63

#define DQSL 3
#define TPRD 0x3f

#define DQS_CYCLE_TIME 625
#define DQS_HALF_CYCLE_TIME DQS_CYCLE_TIME/2

// ----- Top level init functions  -----
void program_non_default_regs_for_mtc_read_and_write_training_init();
void program_non_default_regs_for_mtc_write_training_init();
void program_mtc_training_window_size(int val);
void program_mtc_training_adj_step(int val);
void read_methodA_init();

int get_WDQPRD(int dqs_no,int rankid);

// ----- Register Access functions  ------
void set_rank_id(int rankid);
void set_dxngcr3_read_training_disable_drift_compensation(int dqs_no,int rankid);
int get_phy_delay_write_reg(int dqs_no,int rankid);
int get_mtc_delay_write_reg(int dqs_no,int rankid);
int get_pub_gcr3_gateacctlclk();
void set_pub_gcr3_gateacctlclk(int value);
void set_dxngcr3_wdlvt(int dqs_no,int rankid,int value);
void set_pubgcr3_pure(int value);
void set_crb_active_ranks(int value);
int get_phy_dxngtr0_dgsl(int dqs_no,int rankid);
int get_phy_dxngtr0(int dqs_no,int rankid);
int get_phy_dxngtr0_wdqsl(int dqs_no,int rankid);
int get_dxnmdlr0_iprd(int dqs_no,int rankid);
void set_mtc_dxngsr0 (int dqs_no,int rankid,int value);
int get_mtc_dxngsr6(int dqs_no,int rankid);
void set_mtc_dxngtr0 (int dqs_no,int rankid,int value);
int get_mtc_dxngtr0_dgsl(int dqs_no,int rankid);
int get_phy_dxngsr0_gdqsprd(int dqs_no,int rankid);
int get_phy_dxngsr0(int dqs_no,int rankid);

// Helper functions 
unsigned int rand_interval(unsigned int min, unsigned int max);
 
// --- External Global variables ----
 int global_delay_model;
 int global_dram_mode, global_dram_frequency;
 int global_wdqdprd[NUM_SLICES];
 // Positive number for global_wire_delay_reg_wr means Write DQ is
 // delayed; thus the movement will be to the left;
 // Negative number for global_wire_delay_reg_wr means Write DQS is
 // delayed; thus the movement will be to the right.
 int global_wire_delay_reg_wr[NUM_RANKS][NUM_SLICES];

 // Positive number for global_wire_delay_reg_rd means Read DQS is
 // delayed; thus the movement will be to the left;
 // Negative number for global_wire_delay_reg_rd means Read DQ is
 // delayed; thus the movement will be to the right.
 int global_wire_delay_reg_rd[NUM_RANKS][NUM_SLICES];

 int global_drift_enabled_for_phy_based_training;

 int global_wdqsl[NUM_RANKS][NUM_SLICES];
 int global_golden_wdqsl[NUM_RANKS][NUM_SLICES];
 int global_golden_wr_delay_val[NUM_RANKS][NUM_SLICES];
 int global_golden_rd_lr2_val[NUM_RANKS][NUM_SLICES];
 int global_golden_rd_lr3_val[NUM_RANKS][NUM_SLICES]; //Same is used for lr4 also
 int program_single_rank;
 int dxngtr0_dgsl[NUM_RANKS][NUM_SLICES];
 int dxngsr0_gdqsprd[NUM_RANKS][NUM_SLICES];
 int adjust_gate_delay[NUM_RANKS][NUM_SLICES];
 int global_final_adj_values[NUM_RANKS][NUM_SLICES];
 int global_final_gate_adj_values[NUM_RANKS][NUM_SLICES];
 int random_write_data ;
 int dont_program_training_regs_for_write ;
 int dont_program_training_regs_for_read ;
 int trn_frq;

// read configuration parameters
 int gate_adj_disable;
 int phy_based_delay_adj;
 int wdlvt_enable;

// Global default MTC test control variables 
int global_sample_cnt;
int global_zqcal_break;

#endif 
