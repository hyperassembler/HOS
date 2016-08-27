#ifndef __MTC_TRAIN_HEADER_INC_
#define __MTC_TRAIN_HEADER_INC_

#include "conf_defs.h"
#include <kahalu_sim_init.h>

// prototypes
void mtc_phy_update_training_test();
void mtc_training_tests(int mtc_test_num,int dram_mode,int autoref_int,int delay_model,int dram_frequency, int active_ranks);
void  create_write_training_delay_testcase(int mov_window);
extern void set_autorefresh(const int mc, int interval,int dram_mode, const int dram_frequency);

#define PHY_REGS_PIR_OFFSET   0x1
#define PHY_REGS_PGSR0_OFFSET 0x12
#define STATUS_IDONE_COMPARE  0x1
#define POLLING_LIMIT         100

#ifdef CONF_HAS___PHY_RTL
  #define GOLDEN_RD_DELAY_VAL_1600 0x1f
  #define GOLDEN_WR_DELAY_VAL_1600 0x5
  #define GOLDEN_WDQSL_VAL_1600    0x3
  #define GOLDEN_RDGSL_VAL_1600    0x1
  #define GOLDEN_RD_DELAY_VAL_1333 0x26
  #define GOLDEN_WR_DELAY_VAL_1333 0x3d
  #define GOLDEN_RD_DELAY_VAL_400  0x7d
  #define GOLDEN_WR_DELAY_VAL_400  0xdf
  #define MVM_DELTA 20
  #define MVM_DELTA_RD 20
#else
  #define GOLDEN_RD_DELAY_VAL_1600 0
  #define GOLDEN_WR_DELAY_VAL_1600 1
  #define GOLDEN_RD_DELAY_VAL_1333 0
  #define GOLDEN_WR_DELAY_VAL_1333 1
  #define GOLDEN_RD_DELAY_VAL_400  0
  #define GOLDEN_WR_DELAY_VAL_400  1
  #define MVM_DELTA 5
  #define MVM_DELTA_RD 5
#endif

#endif
