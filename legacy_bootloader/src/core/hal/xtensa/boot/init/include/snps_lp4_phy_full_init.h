
#include "kahalu_sim_init.h"
#include "snps_lp4_phy_lib.h"

#ifdef CONF_HAS___PHY_RTL
void phy_regs_initial_settings(const int mc, const int dram_frequency, const int ddc_mode, const int active_ranks, const int wr_dbi_dis, const int rd_dbi_dis);

void perform_cbt();

void wait_for_pgsr0_idone();

void set_pub_regs(const int mc, const int dram_mode, const int dram_frequency, const int ddc_mode, const int active_ranks, const int wr_dbi_dis, const int rd_dbi_dis);

int phy_real_init_synps_lp4 
     (
         const int mc,
         const int ddc_mode, 
         const int active_ranks, 
         const int dram_frequency,
         const int wr_dbi_dis, 
         const int rd_dbi_dis,
         const int skip_part_phy_init
     );

#endif    

