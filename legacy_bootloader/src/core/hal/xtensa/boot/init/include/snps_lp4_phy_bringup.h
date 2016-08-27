
#include "kahalu_sim_init.h"
#include "snps_lp4_phy_lib.h"

#ifdef CONF_HAS___PHY_RTL
void phy_init_synps_lp4
     (
          const int mc,
          const int active_ranks, 
          const int ddc_mode, 
          const int wr_dbi_dis, 
          const int rd_dbi_dis, 
          const int dram_frequency, 
          const int phy_init_train, 
          const int dump_phy_regs,
          const int set_bdlr_val
     );

#endif    

