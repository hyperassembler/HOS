
#include "kahalu_sim_init.h"

#ifdef CONF_HAS___PHY_RTL
void phy_init_synps_lp4_reg_write(const int mc, const int dram_frequency) {
    uint32_t rd_data = 0;
    uint32_t wr_data = 0;
	
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);
    hw_status("KAHALU_SIM_INIT: PROGRAMMING TRAINING REGISTERS for frequency %0d\n",dram_frequency);
   
   //Select backdoor values based on frequency
   //400MHz
   if (dram_frequency == 400) {
      hw_status("KAHALU_SIM_INIT: PROGRAMMING REGISTERS WITH TRAINED VALUES FOR 400MHz\n");
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACMDLR1_ADDRESS + mc_base_addr), 0x00fc00fc); //0x00f000f0
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GCR5_ADDRESS + mc_base_addr), 0x09093f3f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GCR6_ADDRESS + mc_base_addr), 0x09091919);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR0_ADDRESS + mc_base_addr), 0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR1_ADDRESS + mc_base_addr), 0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR2_ADDRESS + mc_base_addr), 0x00000001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR5_ADDRESS + mc_base_addr), 0x09093f3f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR6_ADDRESS + mc_base_addr), 0x09091919);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR0_ADDRESS + mc_base_addr), 0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR1_ADDRESS + mc_base_addr), 0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR2_ADDRESS + mc_base_addr), 0x00000001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_ADDRESS + mc_base_addr), 0x00181800);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0MDLR1_ADDRESS + mc_base_addr), 0x000000e0);

      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS + mc_base_addr), 0x00000000);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR0_ADDRESS + mc_base_addr), 0x000000f9);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS + mc_base_addr), 0x000000df);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR2_ADDRESS + mc_base_addr), 0x0000003b);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR5_ADDRESS + mc_base_addr), 0x0000001f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_ADDRESS + mc_base_addr), 0x01010001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR0_ADDRESS + mc_base_addr), 0x000000f9);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR1_ADDRESS + mc_base_addr), 0x000000df);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR2_ADDRESS + mc_base_addr), 0x0000003b);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR5_ADDRESS + mc_base_addr), 0x0000001f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GTR0_ADDRESS + mc_base_addr), 0x01010001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR3_ADDRESS + mc_base_addr), 0x0000007d);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR4_ADDRESS + mc_base_addr), 0x0000007d);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR3_ADDRESS + mc_base_addr), 0x0000007d);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR4_ADDRESS + mc_base_addr), 0x0000007d);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR5_ADDRESS + mc_base_addr), 0x09093f3f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR6_ADDRESS + mc_base_addr), 0x09091919);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR0_ADDRESS + mc_base_addr), 0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR1_ADDRESS + mc_base_addr), 0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR2_ADDRESS + mc_base_addr), 0x00000001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1MDLR1_ADDRESS + mc_base_addr), 0x000000fc);

      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS + mc_base_addr), 0x00000001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR0_ADDRESS + mc_base_addr), 0x000000f9);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS + mc_base_addr), 0x000000df);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR2_ADDRESS + mc_base_addr), 0x0000003b);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR5_ADDRESS + mc_base_addr), 0x0000001f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_ADDRESS + mc_base_addr), 0x01010001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR0_ADDRESS + mc_base_addr), 0x000000f9);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR1_ADDRESS + mc_base_addr), 0x000000df);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR2_ADDRESS + mc_base_addr), 0x0000003b);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR5_ADDRESS + mc_base_addr), 0x0000001f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GTR0_ADDRESS + mc_base_addr), 0x01010001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR3_ADDRESS + mc_base_addr), 0x0000007d);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR4_ADDRESS + mc_base_addr), 0x0000007d);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR3_ADDRESS + mc_base_addr), 0x0000007d);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR4_ADDRESS + mc_base_addr), 0x0000007d);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR5_ADDRESS + mc_base_addr), 0x09093f3f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR6_ADDRESS + mc_base_addr), 0x09091919);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR0_ADDRESS + mc_base_addr), 0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR1_ADDRESS + mc_base_addr), 0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR2_ADDRESS + mc_base_addr), 0x00000001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1MDLR1_ADDRESS + mc_base_addr), 0x000000fc);

      //FIXME: Check if this is needed
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_ADDRESS + mc_base_addr), 0x00181800);

      //set phy to do non-contiguous read response FING
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR3_ADDRESS + mc_base_addr));
      hw_sleep(15);
      wr_data = rd_data | 0x2 << PHY_REGS_DWC_DDRPHY_PUB_PGCR3_RDMODE_LSB;
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR3_ADDRESS + mc_base_addr), wr_data);
      hw_sleep(15);

      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_ADDRESS + mc_base_addr));
      hw_sleep(15);
      wr_data = rd_data | 0x2 << PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_RDMODE_LSB;
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_ADDRESS + mc_base_addr), wr_data);
      hw_sleep(15);

      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_ADDRESS + mc_base_addr));
      hw_sleep(15);
      wr_data = rd_data | 0x2 << PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_RDMODE_LSB;
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_ADDRESS + mc_base_addr), wr_data);
      hw_sleep(15);

   //800MHz
   } else if (dram_frequency == 800) { 
      hw_status("KAHALU_SIM_INIT: PROGRAMMING REGISTERS WITH TRAINED VALUES FOR 800MHz\n");
      
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACMDLR1_ADDRESS + mc_base_addr), 0x007c007c);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GCR5_ADDRESS + mc_base_addr), 0x09093f3f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GCR6_ADDRESS + mc_base_addr), 0x09091919);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR0_ADDRESS + mc_base_addr), 0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR1_ADDRESS + mc_base_addr), 0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR2_ADDRESS + mc_base_addr), 0x00000001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ZQ0SR_ADDRESS + mc_base_addr),  0x00000230);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ZQ1SR_ADDRESS + mc_base_addr),  0x00000200);

      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS + mc_base_addr), 0x00000000);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR0_ADDRESS + mc_base_addr), 0x0000007c);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS + mc_base_addr), 0x00000024);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR2_ADDRESS + mc_base_addr), 0x00000005);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR3_ADDRESS + mc_base_addr), 0x0000003f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR4_ADDRESS + mc_base_addr), 0x0000003f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR5_ADDRESS + mc_base_addr), 0x0000000f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_ADDRESS + mc_base_addr),  0x02010002);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0MDLR1_ADDRESS+ mc_base_addr),  0x00000100);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR5_ADDRESS + mc_base_addr),  0x09093f3f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR6_ADDRESS + mc_base_addr),  0x09091919);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR0_ADDRESS + mc_base_addr),  0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR1_ADDRESS + mc_base_addr),  0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR2_ADDRESS + mc_base_addr),  0x00000001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR0_ADDRESS + mc_base_addr), 0x0000007c);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR1_ADDRESS + mc_base_addr), 0x00000024);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR2_ADDRESS + mc_base_addr), 0x00000005);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR3_ADDRESS + mc_base_addr), 0x0000003f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR4_ADDRESS + mc_base_addr), 0x0000003f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR5_ADDRESS + mc_base_addr), 0x0000000f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GTR0_ADDRESS + mc_base_addr), 0x02010002);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1MDLR1_ADDRESS + mc_base_addr),  0x00000060);

      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS + mc_base_addr), 0x00000001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR0_ADDRESS + mc_base_addr), 0x0000007c);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS + mc_base_addr), 0x00000024);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR2_ADDRESS + mc_base_addr), 0x00000005);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR3_ADDRESS + mc_base_addr), 0x0000003f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR4_ADDRESS + mc_base_addr), 0x0000003f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR5_ADDRESS + mc_base_addr), 0x0000000f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_ADDRESS + mc_base_addr),  0x02010002);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0MDLR1_ADDRESS+ mc_base_addr),  0x00000100);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR5_ADDRESS + mc_base_addr),  0x09093f3f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR6_ADDRESS + mc_base_addr),  0x09091919);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR0_ADDRESS + mc_base_addr),  0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR1_ADDRESS + mc_base_addr),  0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR2_ADDRESS + mc_base_addr),  0x00000001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR0_ADDRESS + mc_base_addr), 0x0000007c);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR1_ADDRESS + mc_base_addr), 0x00000024);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR2_ADDRESS + mc_base_addr), 0x00000005);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR3_ADDRESS + mc_base_addr), 0x0000003f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR4_ADDRESS + mc_base_addr), 0x0000003f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR5_ADDRESS + mc_base_addr), 0x0000000f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GTR0_ADDRESS + mc_base_addr), 0x02010002);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1MDLR1_ADDRESS + mc_base_addr),  0x00000060);

      //set phy to do non-contiguous read response FING
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR3_ADDRESS + mc_base_addr));
      hw_sleep(15);
      wr_data = rd_data | 0x2 << PHY_REGS_DWC_DDRPHY_PUB_PGCR3_RDMODE_LSB;
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR3_ADDRESS + mc_base_addr), wr_data);
      hw_sleep(15);

      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_ADDRESS + mc_base_addr));
      hw_sleep(15);
      wr_data = rd_data | 0x2 << PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_RDMODE_LSB;
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_ADDRESS + mc_base_addr), wr_data);
      hw_sleep(15);

      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_ADDRESS + mc_base_addr));
      hw_sleep(15);
      wr_data = rd_data | 0x2 << PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_RDMODE_LSB;
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_ADDRESS + mc_base_addr), wr_data);
      hw_sleep(15);

   //1066MHz
   }else if (dram_frequency == 1066) { 
      hw_status("KAHALU_SIM_INIT: PROGRAMMING REGISTERS WITH TRAINED VALUES FOR 1066MHz\n");
      
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACMDLR1_ADDRESS + mc_base_addr), 0x00500050);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ZQ1SR_ADDRESS + mc_base_addr), 0x00000203);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GCR5_ADDRESS + mc_base_addr), 0x09093f3f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GCR6_ADDRESS + mc_base_addr), 0x09091919);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR0_ADDRESS + mc_base_addr), 0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR1_ADDRESS + mc_base_addr), 0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR2_ADDRESS + mc_base_addr), 0x00000001);

      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS + mc_base_addr), 0x00000000);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR0_ADDRESS + mc_base_addr), 0x0000005d);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS + mc_base_addr), 0x00000033);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR2_ADDRESS + mc_base_addr), 0x00000055);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR3_ADDRESS + mc_base_addr), 0x0000002f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR4_ADDRESS + mc_base_addr), 0x0000002f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR5_ADDRESS + mc_base_addr), 0x0000000b);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0MDLR1_ADDRESS + mc_base_addr), 0x0000005e);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_ADDRESS + mc_base_addr), 0x02010001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR5_ADDRESS + mc_base_addr), 0x09093f3f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR6_ADDRESS + mc_base_addr), 0x09091919);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR0_ADDRESS + mc_base_addr), 0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR1_ADDRESS + mc_base_addr), 0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR2_ADDRESS + mc_base_addr), 0x00000001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR0_ADDRESS + mc_base_addr), 0x0000005d);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR1_ADDRESS + mc_base_addr), 0x00000033);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR2_ADDRESS + mc_base_addr), 0x00000055);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR3_ADDRESS + mc_base_addr), 0x0000002f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR4_ADDRESS + mc_base_addr), 0x0000002f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR5_ADDRESS + mc_base_addr), 0x0000000b);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1MDLR1_ADDRESS + mc_base_addr), 0x00000040);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GTR0_ADDRESS + mc_base_addr), 0x02010001);

      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS + mc_base_addr), 0x00000001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR0_ADDRESS + mc_base_addr), 0x0000005d);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS + mc_base_addr), 0x00000033);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR2_ADDRESS + mc_base_addr), 0x00000055);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR3_ADDRESS + mc_base_addr), 0x0000002f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR4_ADDRESS + mc_base_addr), 0x0000002f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR5_ADDRESS + mc_base_addr), 0x0000000b);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0MDLR1_ADDRESS + mc_base_addr), 0x0000005e);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_ADDRESS + mc_base_addr), 0x02010001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR5_ADDRESS + mc_base_addr), 0x09093f3f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR6_ADDRESS + mc_base_addr), 0x09091919);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR0_ADDRESS + mc_base_addr), 0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR1_ADDRESS + mc_base_addr), 0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR2_ADDRESS + mc_base_addr), 0x00000001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR0_ADDRESS + mc_base_addr), 0x0000005d);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR1_ADDRESS + mc_base_addr), 0x00000033);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR2_ADDRESS + mc_base_addr), 0x00000055);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR3_ADDRESS + mc_base_addr), 0x0000002f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR4_ADDRESS + mc_base_addr), 0x0000002f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR5_ADDRESS + mc_base_addr), 0x0000000b);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1MDLR1_ADDRESS + mc_base_addr), 0x00000040);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GTR0_ADDRESS + mc_base_addr), 0x02010001);


      //set phy to do non-contiguous read response FING
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR3_ADDRESS + mc_base_addr));
      hw_sleep(15);
      wr_data = rd_data | 0x2 << PHY_REGS_DWC_DDRPHY_PUB_PGCR3_RDMODE_LSB;
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR3_ADDRESS + mc_base_addr), wr_data);
      hw_sleep(15);

      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_ADDRESS + mc_base_addr));
      hw_sleep(15);
      wr_data = rd_data | 0x2 << PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_RDMODE_LSB;
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_ADDRESS + mc_base_addr), wr_data);
      hw_sleep(15);

      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_ADDRESS + mc_base_addr));
      hw_sleep(15);
      wr_data = rd_data | 0x2 << PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_RDMODE_LSB;
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_ADDRESS + mc_base_addr), wr_data);
      hw_sleep(15);

   //1333MHz
   } else if (dram_frequency == 1333) { 
      hw_status("KAHALU_SIM_INIT: PROGRAMMING REGISTERS WITH TRAINED VALUES FOR 1333MHz\n");
      
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACMDLR1_ADDRESS + mc_base_addr), 0x004a004a);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GCR5_ADDRESS + mc_base_addr), 0x09093f3f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GCR6_ADDRESS + mc_base_addr), 0x09091919);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR0_ADDRESS + mc_base_addr), 0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR1_ADDRESS + mc_base_addr), 0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR2_ADDRESS + mc_base_addr), 0x00000001);

      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS + mc_base_addr), 0x00000000);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR0_ADDRESS + mc_base_addr), 0x0000004a);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS + mc_base_addr), 0x0000003d);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR2_ADDRESS + mc_base_addr), 0x0000003b);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR3_ADDRESS + mc_base_addr), 0x00000026);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR4_ADDRESS + mc_base_addr), 0x00000026);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR5_ADDRESS + mc_base_addr), 0x00000009);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_ADDRESS + mc_base_addr),  0x02010001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR5_ADDRESS + mc_base_addr),  0x09093f3f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR6_ADDRESS + mc_base_addr),  0x09091919);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR0_ADDRESS + mc_base_addr),  0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR1_ADDRESS + mc_base_addr),  0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR2_ADDRESS + mc_base_addr),  0x00000001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR0_ADDRESS + mc_base_addr), 0x0000004a);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR1_ADDRESS + mc_base_addr), 0x0000003d);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR2_ADDRESS + mc_base_addr), 0x0000003b);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR3_ADDRESS + mc_base_addr), 0x00000026);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR4_ADDRESS + mc_base_addr), 0x00000026);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR5_ADDRESS + mc_base_addr), 0x00000009);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GTR0_ADDRESS + mc_base_addr),  0x02010001);

      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS + mc_base_addr), 0x00000001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR0_ADDRESS + mc_base_addr), 0x0000004a);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS + mc_base_addr), 0x0000003d);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR2_ADDRESS + mc_base_addr), 0x0000003b);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR3_ADDRESS + mc_base_addr), 0x00000026);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR4_ADDRESS + mc_base_addr), 0x00000026);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR5_ADDRESS + mc_base_addr), 0x00000009);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_ADDRESS + mc_base_addr),  0x02010001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR5_ADDRESS + mc_base_addr),  0x09093f3f);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR6_ADDRESS + mc_base_addr),  0x09091919);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR0_ADDRESS + mc_base_addr),  0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR1_ADDRESS + mc_base_addr),  0x01010101);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR2_ADDRESS + mc_base_addr),  0x00000001);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR0_ADDRESS + mc_base_addr), 0x0000004a);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR1_ADDRESS + mc_base_addr), 0x0000003d);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR2_ADDRESS + mc_base_addr), 0x0000003b);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR3_ADDRESS + mc_base_addr), 0x00000026);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR4_ADDRESS + mc_base_addr), 0x00000026);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR5_ADDRESS + mc_base_addr), 0x00000009);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GTR0_ADDRESS + mc_base_addr),  0x02010001);

      //set phy to do non-contiguous read response FING
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR3_ADDRESS + mc_base_addr));
      hw_sleep(15);
      wr_data = rd_data | 0x2 << PHY_REGS_DWC_DDRPHY_PUB_PGCR3_RDMODE_LSB;
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR3_ADDRESS + mc_base_addr), wr_data);
      hw_sleep(15);

      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_ADDRESS + mc_base_addr));
      hw_sleep(15);
      wr_data = rd_data | 0x2 << PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_RDMODE_LSB;
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_ADDRESS + mc_base_addr), wr_data);
      hw_sleep(15);

      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_ADDRESS + mc_base_addr));
      hw_sleep(15);
      wr_data = rd_data | 0x2 << PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_RDMODE_LSB;
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_ADDRESS + mc_base_addr), wr_data);
      hw_sleep(15);

   //1600MHz as default  
   } else {
      hw_status("KAHALU_SIM_INIT: PROGRAMMING REGISTERS WITH TRAINED VALUES FOR 1600MHz\n");
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACMDLR1_ADDRESS + mc_base_addr), 0x003c003c);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GCR5_ADDRESS + mc_base_addr), 0x09093f3f);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GCR6_ADDRESS + mc_base_addr), 0x09091919);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR0_ADDRESS + mc_base_addr), 0x01010101);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR1_ADDRESS + mc_base_addr), 0x01010101);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR2_ADDRESS + mc_base_addr), 0x00000001);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0MDLR1_ADDRESS + mc_base_addr), 0x00000040);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR5_ADDRESS + mc_base_addr), 0x09093f3f);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR6_ADDRESS + mc_base_addr), 0x09091919);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR0_ADDRESS + mc_base_addr), 0x01010101);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR1_ADDRESS + mc_base_addr), 0x01010101);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR2_ADDRESS + mc_base_addr), 0x00000001);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1MDLR1_ADDRESS + mc_base_addr), 0x00000020);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_ADDRESS + mc_base_addr), 0x00381800);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_ADDRESS + mc_base_addr), 0x00381800);
      //hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTEDR0_ADDRESS + mc_base_addr), 0x0001c4a2);
      //hw_sleep(15);
      //hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTEDR1_ADDRESS + mc_base_addr), 0x00007e00);
      //hw_sleep(15);
      //hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTEDR2_ADDRESS + mc_base_addr), 0x00007e00);
      //hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_VTDR_ADDRESS + mc_base_addr),   0x7f003200);
      hw_sleep(200);

      //Ensure PGCR3[25]-WDLVT is programmed to 1'b0 to select MTC based WR training.
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GCR3_ADDRESS + mc_base_addr));
      hw_sleep(15);
      wr_data = rd_data & (~PHY_REGS_DWC_DDRPHY_PUB_DX0GCR3_WDLVT_FIELD_MASK);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GCR3_ADDRESS + mc_base_addr), wr_data);
      hw_sleep(15);
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR3_ADDRESS + mc_base_addr));
      hw_sleep(15);
      wr_data = rd_data & (~PHY_REGS_DWC_DDRPHY_PUB_DX1GCR3_WDLVT_FIELD_MASK);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR3_ADDRESS + mc_base_addr), wr_data);
      //---------------------------------------------------------------------------------------------
      //For RANK0
      //---------------------------------------------------------------------------------------------
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS + mc_base_addr), 0x00000000);
      hw_sleep(200);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR0_ADDRESS + mc_base_addr), 0x0000003e);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS + mc_base_addr), 0x00000005+(0x3F*0x3));
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR2_ADDRESS + mc_base_addr), 0x00000028);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR5_ADDRESS + mc_base_addr), 0x00000007);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_ADDRESS + mc_base_addr),   0x03010001);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR0_ADDRESS + mc_base_addr), 0x0000003e);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR1_ADDRESS + mc_base_addr), 0x00000005+(0x3F*0x3));
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR2_ADDRESS + mc_base_addr), 0x00000028);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR5_ADDRESS + mc_base_addr), 0x00000007);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GTR0_ADDRESS + mc_base_addr),   0x03010001);
      hw_sleep(200);
      //---------------------------------------------------------------------------------------------
      //For RANK1
      //---------------------------------------------------------------------------------------------
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS + mc_base_addr),   0x00010001);
      hw_sleep(200);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR0_ADDRESS + mc_base_addr), 0x0000003e);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS + mc_base_addr), 0x00000005+(0x3F*0x3));
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR2_ADDRESS + mc_base_addr), 0x00000028);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR5_ADDRESS + mc_base_addr), 0x00000007);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_ADDRESS + mc_base_addr),   0x03010001);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR0_ADDRESS + mc_base_addr), 0x0000003e);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR1_ADDRESS + mc_base_addr), 0x00000005+(0x3F*0x3));
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR2_ADDRESS + mc_base_addr), 0x00000028);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR5_ADDRESS + mc_base_addr), 0x00000007);
      hw_sleep(15);
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GTR0_ADDRESS + mc_base_addr),   0x03010001);
      hw_sleep(15);

      //set phy to do non-contiguous read response FING
      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR3_ADDRESS + mc_base_addr));
      hw_sleep(15);
      wr_data = rd_data | 0x2 << PHY_REGS_DWC_DDRPHY_PUB_PGCR3_RDMODE_LSB;
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR3_ADDRESS + mc_base_addr), wr_data);
      hw_sleep(15);

      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_ADDRESS + mc_base_addr));
      hw_sleep(15);
      wr_data = rd_data | 0x2 << PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_RDMODE_LSB;
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_ADDRESS + mc_base_addr), wr_data);
      hw_sleep(15);

      rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_ADDRESS + mc_base_addr));
      hw_sleep(15);
      wr_data = rd_data | 0x2 << PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_RDMODE_LSB;
      hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_ADDRESS + mc_base_addr), wr_data);
      hw_sleep(15);

   }
}
	
#endif //CONF_HAS___PHY_RTL


