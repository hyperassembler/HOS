
#include "kahalu_sim_init.h"
#include "snps_lp4_phy_lib.h"

#ifdef CONF_HAS___PHY_RTL
void phy_init_synps_lp4(const int mc, const int active_ranks, const int ddc_mode, const int wr_dbi_dis, const int rd_dbi_dis, const int dram_frequency, const int phy_init_train, const int dump_phy_regs, const int set_bdlr_val) {
      uint32_t rd_data = 0;
      uint32_t wr_data = 0;
      uint32_t rd_addr = 0;
      uint32_t rl_val = 0;
      uint32_t rtp_val = 0;
      uint32_t tmp_set_bdlr_val;
   
      uint32_t mc_base_addr = get_mcu_baseaddr(mc);

      hw_status("KAHALU_SIM_INIT: SNPS PHY INIT frequency = %0d, ddc_mode = %0d, wr_dbi_dis= %0d, rd_dbi_dis = %0d\n", dram_frequency, ddc_mode, wr_dbi_dis, rd_dbi_dis);

      program_ddc_phy_params(mc, rd_dbi_dis, dram_frequency);
      
      hw_status("KAHALU_SIM_INIT: DEASSERTING RESET to PHY\n");
      hw_write32((uint32_t*)(MCU_REGS_PHYCTRL_REGS_SNP_CTL_RST_N_ADDRESS + mc_base_addr), 0x01);

      //Enable PUB mode
      hw_status("KAHALU_SIM_INIT: ENABLING PUB\n");
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR1_ADDRESS + mc_base_addr), 0x02024640);
       
      //for MTC coverage
      if (set_bdlr_val != 0)
      {
          wr_data = PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_RANKWID_SET(0) |  PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_RANKRID_SET(0);
          hw_phy_write32((uint32_t*) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS, wr_data);
          tmp_set_bdlr_val = set_bdlr_val & 0xff;
          wr_data = (tmp_set_bdlr_val << 24) | (tmp_set_bdlr_val << 16) | (tmp_set_bdlr_val << 8) | tmp_set_bdlr_val;
          hw_phy_write32((uint32_t*) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR3_ADDRESS, wr_data);
          hw_phy_write32((uint32_t*) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR4_ADDRESS, wr_data);
          hw_phy_write32((uint32_t*) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR5_ADDRESS, tmp_set_bdlr_val);
          hw_phy_write32((uint32_t*) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR3_ADDRESS, wr_data);
          hw_phy_write32((uint32_t*) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR4_ADDRESS, wr_data);
          hw_phy_write32((uint32_t*) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR5_ADDRESS, tmp_set_bdlr_val);
          wr_data = PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_RANKWID_SET(1) |  PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_RANKRID_SET(1);
          hw_phy_write32((uint32_t*) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS, wr_data);
          wr_data = (tmp_set_bdlr_val << 24) | (tmp_set_bdlr_val << 16) | (tmp_set_bdlr_val << 8) | tmp_set_bdlr_val;
          hw_phy_write32((uint32_t*) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR3_ADDRESS, wr_data);
          hw_phy_write32((uint32_t*) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR4_ADDRESS, wr_data);
          hw_phy_write32((uint32_t*) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR5_ADDRESS, tmp_set_bdlr_val);
          hw_phy_write32((uint32_t*) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR3_ADDRESS, wr_data);
          hw_phy_write32((uint32_t*) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR4_ADDRESS, wr_data);
          hw_phy_write32((uint32_t*) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR5_ADDRESS, tmp_set_bdlr_val);
          wr_data = PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_RANKWID_SET(0) |  PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_RANKRID_SET(0);
          hw_phy_write32((uint32_t*) MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS, wr_data);
      }

      //Reconfigure DQ/DM Mapping for DXnDQMAP;  
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0DQMAP0_ADDRESS + mc_base_addr), 0x00035678);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0DQMAP1_ADDRESS + mc_base_addr), 0x00004012);
 
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1DQMAP0_ADDRESS + mc_base_addr), 0x00035678);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1DQMAP1_ADDRESS + mc_base_addr), 0x00004012);
     
      //Programming DRAM Configuration (setting memory type to LP4)
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCR_ADDRESS + mc_base_addr), 0x0000040d);
      
      //Driving retention enable ports of PHY top 
      hw_write32((uint32_t*)(MCU_REGS_PHYCTRL_REGS_SNP_RET_EN_N_ADDRESS + mc_base_addr), 0x1);
      hw_write32((uint32_t*)(MCU_REGS_PHYCTRL_REGS_SNP_RET_EN_I_ADDRESS + mc_base_addr), 0x2);
      hw_write32((uint32_t*)(MCU_REGS_PHYCTRL_REGS_SNP_EXT_EN_I_ADDRESS + mc_base_addr), 0x01);
      
      //PHY Timing registers
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PTR1_ADDRESS + mc_base_addr), 0x2e8112c0);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PTR3_ADDRESS + mc_base_addr), 0x00000014);
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PTR4_ADDRESS + mc_base_addr));
      rd_data = rd_data & (uint32_t)(~PHY_REGS_DWC_DDRPHY_PUB_PTR4_TDINIT1_FIELD_MASK);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PTR4_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_PTR4_TDINIT1_SET(lp4_param_db.xpd[ddc_mode]+2)) );
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PTR5_ADDRESS + mc_base_addr), 0x00000014);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PTR6_ADDRESS + mc_base_addr), 0x03300640);
 
      //FREQ VARIATION -- Frequency of clk_mem = 0.5 * DRAM clock frequency  
      //PLL Control Register 0 for 400MHz
      if (dram_frequency == 400) {
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_PLLCR0_FRQSEL_SET(0x6)) );
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL0PLLCR0_FRQSEL_SET(0x6)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL1PLLCR0_FRQSEL_SET(0x6)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL2PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL2PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL2PLLCR0_FRQSEL_SET(0x6)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL3PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL3PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL3PLLCR0_FRQSEL_SET(0x6)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL4PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL4PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL4PLLCR0_FRQSEL_SET(0x6)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL5PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL5PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL5PLLCR0_FRQSEL_SET(0x6)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL6PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL6PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL6PLLCR0_FRQSEL_SET(0x6)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL7PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL7PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL7PLLCR0_FRQSEL_SET(0x6)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL8PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL8PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL8PLLCR0_FRQSEL_SET(0x6)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SLBPLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SLBPLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SLBPLLCR0_FRQSEL_SET(0x6)));
      } else if (dram_frequency == 800)  {
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_PLLCR0_FRQSEL_SET(0x2)) );
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL0PLLCR0_FRQSEL_SET(0x2)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL1PLLCR0_FRQSEL_SET(0x2)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL2PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL2PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL2PLLCR0_FRQSEL_SET(0x2)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL3PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL3PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL3PLLCR0_FRQSEL_SET(0x2)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL4PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL4PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL4PLLCR0_FRQSEL_SET(0x2)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL5PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL5PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL5PLLCR0_FRQSEL_SET(0x2)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL6PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL6PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL6PLLCR0_FRQSEL_SET(0x2)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL7PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL7PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL7PLLCR0_FRQSEL_SET(0x2)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL8PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL8PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL8PLLCR0_FRQSEL_SET(0x2)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SLBPLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SLBPLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SLBPLLCR0_FRQSEL_SET(0x2)));
      } else if (dram_frequency == 1066)  {
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_PLLCR0_FRQSEL_SET(0x1)) );
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL0PLLCR0_FRQSEL_SET(0x1)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL1PLLCR0_FRQSEL_SET(0x1)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL2PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL2PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL2PLLCR0_FRQSEL_SET(0x1)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL3PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL3PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL3PLLCR0_FRQSEL_SET(0x1)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL4PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL4PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL4PLLCR0_FRQSEL_SET(0x1)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL5PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL5PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL5PLLCR0_FRQSEL_SET(0x1)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL6PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL6PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL6PLLCR0_FRQSEL_SET(0x1)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL7PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL7PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL7PLLCR0_FRQSEL_SET(0x1)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL8PLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL8PLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SL8PLLCR0_FRQSEL_SET(0x1)));
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SLBPLLCR0_ADDRESS + mc_base_addr));
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SLBPLLCR0_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_DX8SLBPLLCR0_FRQSEL_SET(0x1)));
      }
     
      //DRAM timing parameter
      // DTPR0:
      // [4:0] = RTP (MR2 [2:0])
      // [14:8] = RP (maps to RPab typical cycles = 34)
      // [22:16] = RAS 
      // [28:24] = RRD
      
       wr_data = 0;
             
       if(ddc_mode == DDC_MICRON){
            hw_status("KAHALU_SIM_INIT: DDC_MODE is DDC_MICRON\n");
            rl_val = lp4_param_db.rl[DDC_TYP];
            rtp_val = lp4_param_db.rtp[DDC_TYP];
            //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.rl = %d\n",lp4_param_db.rl[DDC_TYP]);
            //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.rtp = %d\n",lp4_param_db.rtp[DDC_TYP]);
       } else{
            hw_status("KAHALU_SIM_INIT: DDC_MODE is not DDC_MICRON\n");
            rl_val = lp4_param_db.rl[DDC_REG];
            //rl_val = 28;
            rtp_val = lp4_param_db.rtp[DDC_REG];
            //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.rl = %d\n",lp4_param_db.rl[DDC_TYP]);
            //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.rtp = %d\n",lp4_param_db.rtp[DDC_TYP]);
       }
       
        if((rl_val == 6) && (rtp_val == 8)){
            wr_data = 0;
        }else if (((rl_val == 10)||(rl_val == 12)) && (rtp_val == 8)){
            wr_data = 1;
        }else if((rl_val == 14) && (rtp_val == 8)){
            wr_data = 2;
        }else if((rl_val == 20) && (rtp_val == 8)){
            wr_data = 3;
        }else if(((rl_val == 24)||(rl_val == 28)) && (rtp_val == 10)){
            wr_data = 4;
        }else if(((rl_val == 28)||(rl_val == 32)) && (rtp_val == 12)){ //*
            wr_data = 5;
        }else if(((rl_val == 32) ||(rl_val == 36)) && (rtp_val == 14)){
            wr_data = 6;
        }else if(((rl_val == 36)||(rl_val == 40)) && (rtp_val == 16)){
            wr_data = 7;
        }else{
            wr_data = 0;
        }
                 

      //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.rpab=%d\n",lp4_param_db.rpab[ddc_mode]);
      //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.ras=%d\n" ,lp4_param_db.ras[ddc_mode]);
      //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.rrd=%d\n",lp4_param_db.rrd[ddc_mode]);

      
      //hw_status("ddc_mode=%d\n",ddc_mode);
      //hw_status("RPAB: {%d,%d,%d,%d,%d,%d}",lp4_param_db.rpab[0],lp4_param_db.rpab[1],lp4_param_db.rpab[2],lp4_param_db.rpab[3],lp4_param_db.rpab[4],lp4_param_db.rpab[5]);

      hw_status("DTPR0.rpab=%d\n",lp4_param_db.rpab[ddc_mode]);
      hw_status("DTPR0.ras=%d\n" ,lp4_param_db.ras[ddc_mode]);
      hw_status("DTPR0.rrd=%d\n",lp4_param_db.rrd[ddc_mode]);
      wr_data = ((wr_data                     << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR0_TRTP_LSB) | 
                 (lp4_param_db.rpab[ddc_mode] << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR0_TRP_LSB) |
                 (lp4_param_db.ras[ddc_mode]  << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR0_TRAS_LSB) |
                 (lp4_param_db.rrd[ddc_mode]  << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR0_TRRD_LSB) );
                                   
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTPR0_ADDRESS + mc_base_addr), wr_data);
      
      // DTPR1:  
      //PHY_REGS_DWC_DDRPHY_PUB_DTPR1_TWLMRD_LSB 
      //PHY_REGS_DWC_DDRPHY_PUB_DTPR1_TFAW_LSB 
      //PHY_REGS_DWC_DDRPHY_PUB_DTPR1_TMOD_LSB
      //PHY_REGS_DWC_DDRPHY_PUB_DTPR1_TMRD_LSB

      //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.faw=%d\n",lp4_param_db.faw[ddc_mode]);
      //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.mrd=%d\n" ,lp4_param_db.mrd[ddc_mode]);
      hw_status("DTPR1.faw=%d\n",lp4_param_db.faw[ddc_mode]);
      hw_status("DTPR1.mrd=%d\n",lp4_param_db.mrd[ddc_mode]);
      wr_data = ((16                          << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR1_TWLMRD_LSB) | 
                 (lp4_param_db.faw[ddc_mode]  << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR1_TFAW_LSB) |
                 (7                           << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR1_TMOD_LSB) |    //MOD is DDR3/DDR4 param. Don't care for LPDDR.
                 (lp4_param_db.mrd[ddc_mode]  << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR1_TMRD_LSB) );

      //Use database timing value to program timing registers
      //hw_phy_write32((uint32_t)MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTPR1_ADDRESS, 0x1740071A);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTPR1_ADDRESS + mc_base_addr), wr_data);
      
      // DTRP2:
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR2_TRTW_LSB 
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR2_TRTODT_LSB 
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR2_TCMDCKE_LSB
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR2_TCKE_LSB 
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR2_TXS_LSB 
      //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.cmdcke=%d\n",lp4_param_db.cmdcke[ddc_mode]);
      //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.cke=%d\n" ,lp4_param_db.cke[ddc_mode]);
      //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.xsr=%d\n" ,lp4_param_db.xsr[ddc_mode]);
      hw_status("DTPR2.cmdcke=%d\n",lp4_param_db.cmdcke[ddc_mode]);
      hw_status("DTPR2.cke=%d\n" ,lp4_param_db.cke[ddc_mode]);
      hw_status("DTPR2.xsr=%d\n" ,lp4_param_db.xsr[ddc_mode]);
      wr_data = ((0                           << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR2_TRTW_LSB) |    //Stardard bus turn around delay; No additional delay
                 (0                           << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR2_TRTODT_LSB) |  //MOD is DDR3/DDR4 param. Don't care for LPDDR.
                 (lp4_param_db.cmdcke[ddc_mode]<< (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR2_TCMDCKE_LSB) |  
                 (lp4_param_db.cke[ddc_mode]  << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR2_TCKE_LSB) |  
                 (7                           << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR2_TVRCG_LSB) |  
                 (lp4_param_db.xsr[ddc_mode]  << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR2_TXS_LSB) );

      //Use database timing value to program timing registers
      //hw_phy_write32((uint32_t)MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTPR2_ADDRESS, 0x000c01d9);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTPR2_ADDRESS + mc_base_addr), wr_data);
      
      // DTRP3:
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR3_TOFDX_LSB 
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR3_TCCD_LSB 
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR3_TDLLK_LSB
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR3_TDQSCKMAX_LSB 
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR3_TDQSCK_LSB 
      
      //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.dqsck=%d\n",lp4_param_db.dqsck[ddc_mode]);
      hw_status("DTPR3.dqsckmax=%d\n",lp4_param_db.dqsck[ddc_mode]);
      hw_status("DTPR3.dqsck=%d\n",lp4_param_db.dqsck[ddc_mode]);
      wr_data = ((0                           << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR3_TOFDX_LSB) |   //ODT turn-off dealy extention = 0
                 (0                           << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR3_TCCD_LSB) |    //CCD=BL/2
                 (384                         << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR3_TDLLK_LSB) |   //Don't care. DLL locking time. Use default in PUB.  
                 (lp4_param_db.dqsck[ddc_mode]<< (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR3_TDQSCKMAX_LSB) |  
                 (lp4_param_db.dqsck[ddc_mode]<< (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR3_TDQSCK_LSB) );

      //1600MHz setting
      //hw_phy_write32((uint32_t)MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTPR3_ADDRESS, 0x02000606);
      //1333MHz setting
      //hw_phy_write32((uint32_t)MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTPR3_ADDRESS, 0x02000505);
      //Use database timing value to program timing registers
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTPR3_ADDRESS + mc_base_addr), wr_data);
      
      // DTRP4:
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR4_TAOND_TAOFD_LSB 
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR4_TRFC_LSB 
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR4_TWLO_LSB 
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR4_TXP_LSB 
      
      //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.rfcab=%d\n",lp4_param_db.rfcab[ddc_mode]);
      //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.xpd=%d\n",lp4_param_db.xpd[ddc_mode]);
      hw_status("DTPR4.rfcab=%d\n",lp4_param_db.rfcab[ddc_mode]);
      hw_status("DTPR4.xpd=%d\n",lp4_param_db.xpd[ddc_mode]);
      wr_data = ((0                           << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR4_TAOND_TAOFD_LSB) | //DDR2 only. Don't care. Set to 0.
                 (lp4_param_db.rfcab[ddc_mode]<< (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR4_TRFC_LSB) |
                 (43                          << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR4_TWLO_LSB) |    //Not used by DDC; Use default  
                 (lp4_param_db.xpd[ddc_mode]  << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR4_TXP_LSB) );

      //Use database timing value to program timing registers
      //hw_phy_write32((uint32_t)MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTPR4_ADDRESS, 0x01202814);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTPR4_ADDRESS + mc_base_addr), wr_data);
      
      // DTRP5:
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR5_TRC_LSB 
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR5_TRCD_LSB 
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR5_TWTR_LSB 
      
      //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.rcpb=%d\n",lp4_param_db.rcpb[ddc_mode]);
      //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.rcd=%d\n",lp4_param_db.rcd[ddc_mode]);
      //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.wtr=%d\n",lp4_param_db.wtr[ddc_mode]);
      hw_status("DTPR5.rcpb=%d\n",lp4_param_db.rcpb[ddc_mode]);
      hw_status("DTPR5.rcd=%d\n",lp4_param_db.rcd[ddc_mode]);
      hw_status("DTPR5.wtr=%d\n",lp4_param_db.wtr[ddc_mode]);
      wr_data = ((lp4_param_db.rcpb[ddc_mode] << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR5_TRC_LSB) | 
                 (lp4_param_db.rcd[ddc_mode]  << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR5_TRCD_LSB) |
                 (lp4_param_db.wtr[ddc_mode]  << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR5_TWTR_LSB) );

      //Use database timing value to program timing registers
      //hw_phy_write32((uint32_t)MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTPR5_ADDRESS, 0x60654410);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTPR5_ADDRESS + mc_base_addr), wr_data);
      
      // DTRP6:
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR6_PUBWLEN_LSB 
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR6_PUBRLEN_LSB 
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR6_PUBWL_LSB 
    //PHY_REGS_DWC_DDRPHY_PUB_DTPR6_PUBRL_LSB 

      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTPR6_ADDRESS + mc_base_addr), 0x00000000); //RL/WL disabled in PUB. Will be calculated from MR settings.
      
      
//ZQCR Configuration register for ZQ cal  
hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ZQCR_ADDRESS + mc_base_addr), 0x008a2c58);

 
// PIR. Initiates PLL initialization, Impedence caliberation, Delay line caliberation.
hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x00000073);
  
   //FREQ VARIATION
   //PGCR2 PHY general configuration register
   if (dram_frequency == 400) {
      //mem_clk_freq = 200MHz
      //mem_clk_period = 5ns
      //Adding 10% margin = 5.5ns
      //tREFPRD = (9*3900/mem_clk_period)-600 = 5781 = 0x1695
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR2_ADDRESS + mc_base_addr), 0x10f01695);
   } else if (dram_frequency == 800) {
      //mem_clk_freq = 400MHz
      //mem_clk_period = 2.5ns
      //Adding 10% margin = 2.75ns
      //tREFPRD = (9*3900/mem_clk_period)-600 = 12164 = 0x2f84
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR2_ADDRESS + mc_base_addr), 0x10f02f84);
   } else if (dram_frequency == 1066) {
      //mem_clk_freq = 533MHz
      //mem_clk_period = 1.876ns
      //Adding 10% margin = 2.06ns
      //tREFPRD = (9*3900/mem_clk_period)-600 = 12164 = 0x4037
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR2_ADDRESS + mc_base_addr), 0x10f04037);
   } else if (dram_frequency == 1333) {
      //mem_clk_freq = 666.6666MHz
      //mem_clk_period = 1.5ns
      //Adding 10% margin = 1.65ns
      //tREFPRD = (9*3900/mem_clk_period)-600 = 20672 = 0x50c0
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR2_ADDRESS + mc_base_addr), 0x10f050c0);
   } else {
      //mem_clk_freq = 800MHz
      //mem_clk_period = 1.25ns
      //Adding 10% margin = 1.375ns
      //tREFPRD = (9*3900/mem_clk_period)-600 = 24927 = 0x615f
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR2_ADDRESS + mc_base_addr), 0x10f0615f);
   } 


   //Waiting for PLL initialization, Impedence caliberation, Delay line caliberation to be complete  
   rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));

   rd_data = 0;
    do{      
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));
    }while((rd_data & 0x00000001) != 0x00000001);
     
    hw_status("KAHALU_SIM_INIT: PLL INITIALIZATION COMPLETE\n"); 

  
  
//MR0
hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR0_ADDRESS + mc_base_addr), 0x00000000);
  
//FREQ VARIATION
//MR1 
   if (dram_frequency == 400) {
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR1_ADDRESS + mc_base_addr), 0x00000016);
   } else if (dram_frequency == 800) { 
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR1_ADDRESS + mc_base_addr), 0x00000026);	//nWR=16, wr-preable =2 nclk, BL=2'b10
   } else if (dram_frequency == 1066) { 
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR1_ADDRESS + mc_base_addr), 0x00000036);	//nWR=20, wr-preable =2 nclk, BL=2'b10
   } else if (dram_frequency == 1333) { 
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR1_ADDRESS + mc_base_addr), 0x00000046);	//nWR=24, wr-preable =2 nclk, BL=2'b10
   } else {
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR1_ADDRESS + mc_base_addr), 0x00000056);	//nWR=30, wr-preable =2 nclk, BL=2'b10
   }
 
//MR3

        //write MR3 to enable rd and wr dbi except when dbi is disabled by test 
        if( (wr_dbi_dis ==1) && (rd_dbi_dis == 1) )
          wr_data = 0x31;       //00110001  wr_dbi + rd_dbi cleared
        else if(wr_dbi_dis) 
          wr_data = 0x71;	//01110001  wr_dbi cleared 
        else if(rd_dbi_dis) 
          wr_data = 0xb1;	//10110001  rd_dbi cleared 
        else
          wr_data = 0xf1;	//enable  both 

//hw_phy_write32((uint32_t)MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR3_ADDRESS, 0x000000f1);
hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR3_ADDRESS + mc_base_addr), wr_data);

//FREQ VARIATION
//MR2
   if (dram_frequency == 400) {
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR2_ADDRESS + mc_base_addr), 0x000000009); 
   } else if (dram_frequency == 800) { 
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR2_ADDRESS + mc_base_addr), 0x000000012);
   } else if (dram_frequency == 1066) { 
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR2_ADDRESS + mc_base_addr), 0x00000001b);
   } else if (dram_frequency == 1333) { 
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR2_ADDRESS + mc_base_addr), 0x000000024);
   } else {
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR2_ADDRESS + mc_base_addr), 0x00000002D);
   }
//MR4
hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR4_ADDRESS + mc_base_addr), 0x00000003);

//MR12
hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR12_ADDRESS + mc_base_addr), 0x0000004D);

//MR11
hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR11_ADDRESS + mc_base_addr), 0x00000000);
  
  
// DX8SL0DXCTL2 Data slice control register. Value got from Synopsys
hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_ADDRESS + mc_base_addr), 0x00281800);
  
// DX8SL1DXCTL2 Data slice control register. Value got from Synopsys
hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_ADDRESS + mc_base_addr), 0x00281800);
  
// DX8SL2DXCTL2 Data slice control register. Value got from Synopsys
hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL2DXCTL2_ADDRESS + mc_base_addr), 0x00281800);
 
// DX8SL3DXCTL2 Data slice control register. Value got from Synopsys
hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL2DXCTL2_ADDRESS + mc_base_addr), 0x00281800);
 
// DX8SL0DQSCTL Data slice control register. Value got from Synopsys
hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DQSCTL_ADDRESS + mc_base_addr), 0x012640c4);
  
// DX8SL1DQSCTL Data slice control register. Value got from Synopsys
hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DQSCTL_ADDRESS + mc_base_addr), 0x012640c4);
  
// DX8SL2DQSCTL Data slice control register. Value got from Synopsys
hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL2DQSCTL_ADDRESS + mc_base_addr), 0x012640c4);
  
// DX8SL3DQSCTL Data slice control register. Value got from Synopsys
hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL3DQSCTL_ADDRESS + mc_base_addr), 0x012640c4);
  
// DSGCR PHY update request disabled
hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DSGCR_ADDRESS + mc_base_addr), 0x02a04180);
  
// DTCR0  Training configuration register. 
hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTCR0_ADDRESS + mc_base_addr), 0x900051c7);

if (active_ranks == 1) {
    //DTCR1 Training configuration register.
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTCR1_ADDRESS + mc_base_addr), 0x00010236);
} else {
    //DTCR1 Training configuration register.
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTCR1_ADDRESS + mc_base_addr), 0x00030236);
}
 
 
// Starting DRAM initializarion
hw_status("KAHALU_SIM_INIT: DRAM INITIALIZATION STARTED\n");
hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x00000181); 

rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));

      rd_data = 0;
    do{      
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));
    }while((rd_data & 0x00000001) != 0x00000001);
     
    hw_status("KAHALU_SIM_INIT: DRAM INITIALIZATION COMPLETE\n");

if (dump_phy_regs) {
    hw_status("DUMPING ALL REGISTERS BEFORE TRAINING\n");

    rd_addr = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RIDR_ADDRESS;
    do
    {      
	hw_read32((uint32_t*)(rd_addr+mc_base_addr));
	rd_addr += 4;
    } while (rd_addr != MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX4SLBIOCR_ADDRESS);
}

if (!phy_init_train) 
{
    hw_status("KAHALU_SIM_INIT: PROGRAMMING REGISTERS WITH TRAINED VALUES for FREQUENCY %0d\n",dram_frequency);
    phy_init_synps_lp4_reg_write(mc, dram_frequency);
} 
else
{
    hw_status("TRAININGS START\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x0012FE01); 
    rd_data = 0;
    do
    {      
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));
    } while ((rd_data & 0x00000001) != 0x00000001);

    hw_status("TRAININGS COMPLETE\n");
    if (rd_data & 0x7ffc0000)
    {
        hw_status("ERROR: PHY_INIT_TRAIN failed; PGSR0 = 0x%0x\n", rd_data);
    }
}
    
if (dump_phy_regs) {
    hw_status("DUMPING ALL REGISTERS AFTER TRAINING\n");

    rd_addr = MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RIDR_ADDRESS;
    do
    {      
	    hw_read32((uint32_t*)(rd_addr+mc_base_addr));
	    rd_addr += 4;
    } while (rd_addr != MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX4SLBIOCR_ADDRESS);
}

      hw_status("KAHALU_SIM_INIT: DISABLING PUB\n");
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR1_ADDRESS + mc_base_addr), 0x02004600);
      hw_status("KAHALU_SIM_INIT: PHY INITIALIZATION COMPLETE\n");
      
}

#endif    

