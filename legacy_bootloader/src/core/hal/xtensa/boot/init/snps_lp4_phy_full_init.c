
#include "kahalu_sim_init.h"
#include "snps_lp4_phy_lib.h"

typedef struct DCU_CMD { 
    uint32_t data0;
    uint32_t data1;
    uint32_t mask;
    uint32_t addr;
    uint32_t bank;
    uint32_t rank;
    uint32_t cmd;
    uint32_t tag;
    uint32_t dtp;
    uint32_t rpt;
    uint32_t dcu_cache_addr;
    uint32_t previous_dcu_cache_addr;
} DCU_CMD_STRUCT;

uint32_t get_ca_bdl(int i, uint32_t ACBDLR6_val, uint32_t ACBDLR7_val) {
    if(i<4) {
        return ((ACBDLR6_val >> (i*8)) & 0xff );
    }
    else {
        return ((ACBDLR7_val >> ((i-4)*8)) & 0xff );
    }
}

void inc_ca_bdl(int i, uint32_t * ACBDLR6_val, uint32_t * ACBDLR7_val) {
    if(i<4) {
        *ACBDLR6_val += (1<<(i*8));
    }
    else {
        *ACBDLR7_val += (1<<((i-4)*8));
    }
}

void enable_boot_clocks(const int mc) {
#ifdef MCU_IP_VERIF
    uint32_t rd_data = 0;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);

    hw_status("KAHALU_SIM_INIT: Enabling boot clocks\n");
    rd_data = hw_read32((uint32_t*)(MCU_REGS_MCP_REGS_QOS_CONFIG_ADDRESS + mc_base_addr));
    rd_data = rd_data & (~0x8000);
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_QOS_CONFIG_ADDRESS + mc_base_addr), rd_data);

    rd_data = hw_read32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_SPARE0_ADDRESS + mc_base_addr));
    rd_data = rd_data & (~0x1);
    hw_write32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_SPARE0_ADDRESS + mc_base_addr), rd_data);
#else
    hw_write32((UINT32*)((HUP_CHIP_POR_DRAMC_CTRL_ADDRESS)),0x0); //DRAMC1 and DRAMC0 MUX Select
#endif //MCU_IP_VERIF 

}

void enable_fast_clocks(const int mc) {
#ifdef MCU_IP_VERIF
    uint32_t rd_data = 0;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);
    //TODO: FIXME: Temporarily enabling the fast clock initially itself
    //For real sequence, stage1 should use a slow clock and then switch to
    //fast clock
    
    hw_status("KAHALU_SIM_INIT: Enabling fast clocks\n");
    rd_data = hw_read32((uint32_t*)(MCU_REGS_MCP_REGS_QOS_CONFIG_ADDRESS + mc_base_addr));
    rd_data = rd_data | 0x8000;
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_QOS_CONFIG_ADDRESS + mc_base_addr), rd_data);
    
    rd_data = hw_read32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_SPARE0_ADDRESS + mc_base_addr));
    rd_data = rd_data | 0x1;
    hw_write32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_SPARE0_ADDRESS + mc_base_addr), rd_data);
#else
    hw_write32((UINT32*)((HUP_CHIP_POR_DRAMC_CTRL_ADDRESS)),0x11); //DRAMC1 and DRAMC0 MUX Select
#endif //MCU_IP_VERIF 
}

void write_single_dcu_reg(const int mc, DCU_CMD_STRUCT * dcu_struct, uint32_t auto_inc, uint32_t prt_val, uint32_t write_ptr)
{
    uint32_t cmd_word0;
    uint32_t cmd_word1;
    uint32_t auto_inc_en;
    uint32_t rd_data, wr_data, rd_data_masked;
    uint32_t dcu_addr = dcu_struct->dcu_cache_addr ;
    uint32_t previous_dcu_addr = dcu_struct->previous_dcu_cache_addr;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);
    
    cmd_word0 = 0;
    cmd_word0 |= (dcu_struct->mask & 0xff);
    cmd_word0 |= ((dcu_struct->addr & 0xffff)<<8);
    cmd_word0 |= ((dcu_struct->bank & 0xf)<<24);
    cmd_word0 |= ((dcu_struct->rank & 0x1)<<28);
    cmd_word0 |= ((dcu_struct->cmd & 0x7)<<29);
    
    cmd_word1 = 0;
    cmd_word1 |= ((dcu_struct->cmd >> 3) & 0x3);
    cmd_word1 |= ((dcu_struct->tag & 0x3)<<2);
    cmd_word1 |= ((dcu_struct->dtp & 0x3f)<<4);
    cmd_word1 |= ((dcu_struct->rpt & 0xff)<<10);
    
    //write address for first slice
    if(auto_inc == 0) {
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), (dcu_addr&0xf) );
    }
    else {
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), (dcu_addr&0xf) | 0x400);
    }
    
    //write data0
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), dcu_struct->data0 );
    
    //write address for second slice if not using auto increment
    if(auto_inc == 0) {
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), (dcu_addr&0xf) | 0x10);
    }
    
    //write data1
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), dcu_struct->data1 );
    
    //write address for third slice if not using auto increment
    if(auto_inc == 0) {
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), (dcu_addr&0xf) | 0x20);
    }
    
    //write mask, addr, bank, rank, and cmd[2:0]
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), cmd_word0 );
    
    //write address for fourth slice if not using auto increment
    if(auto_inc == 0) {
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), (dcu_addr&0xf) | 0x30);
    }
    
    //write cmd[4:3], tag, dtp, and rpt
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), cmd_word1 );
    
    //write pointer register if write_ptr does not equal 0
    if(write_ptr) {
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUTPR_ADDRESS + mc_base_addr), prt_val );
    }
    
    //reset command cache
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x4 | (previous_dcu_addr<<4) | (previous_dcu_addr<<8) | (42<<12));
    
    //send nop
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x0 | (previous_dcu_addr<<4) | (previous_dcu_addr<<8) | (42<<12));
    
    //write 0 to DCUGCR
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUGCR_ADDRESS + mc_base_addr), 0x00000000 );
    
    //send run intruction
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x1 | (dcu_addr<<4) | (dcu_addr<<8) | (42<<12));
    
    //wait for dcu command to finish
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr));
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr));
    } 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR1_ADDRESS + mc_base_addr)); // 0x00000000
}

// ----------------------------------------------------------------
// Initial programming of the phy registers with some known values
// as per SNPS reg_dump.log
// ----------------------------------------------------------------
void phy_regs_initial_settings(const int mc, const int dram_frequency, const int ddc_mode, const int active_ranks, const int wr_dbi_dis, const int rd_dbi_dis) {
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);
    uint32_t rd_data = 0;
    uint32_t wr_data = 0;
    uint32_t rd_addr = 0;
    uint32_t rl_val = 0;
    uint32_t rtp_val = 0;
        
    //hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_AACR_ADDRESS + mc_base_addr), 0x00000008 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR1_ADDRESS + mc_base_addr), 0x00040404 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR2_ADDRESS + mc_base_addr), 0x00040404 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR6_ADDRESS + mc_base_addr), 0x04040404 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR7_ADDRESS + mc_base_addr), 0x04040404 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR8_ADDRESS + mc_base_addr), 0x04040404 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR9_ADDRESS + mc_base_addr), 0x04040404 );
    /*hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_CATR0_ADDRESS + mc_base_addr), 0x001f1354 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_CATR1_ADDRESS + mc_base_addr), 0x0103cafb );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DQSDR0_ADDRESS + mc_base_addr), 0x30226004 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DQSDR1_ADDRESS + mc_base_addr), 0x6529688d );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTAR0_ADDRESS + mc_base_addr), 0x04703477 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTAR1_ADDRESS + mc_base_addr), 0x000f0027 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTAR2_ADDRESS + mc_base_addr), 0x000e0022 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DQSDR2_ADDRESS + mc_base_addr), 0x00001b27 );*/
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DSGCR_ADDRESS + mc_base_addr), 0x02a04180 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTCR0_ADDRESS + mc_base_addr), 0x900051c7 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTCR1_ADDRESS + mc_base_addr), 0x00030236 );
    if (active_ranks == 1) {
        //DTCR1 Training configuration register.
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTCR1_ADDRESS + mc_base_addr), 0x00010236);
    } else {
        //DTCR1 Training configuration register.
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTCR1_ADDRESS + mc_base_addr), 0x00030236);
    }
    //hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTDR0_ADDRESS + mc_base_addr), 0x35823667 );
    //hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTDR1_ADDRESS + mc_base_addr), 0xc656ef08 );
    
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
      //changing values for TCMDCKE and TCKE to try to fix timing error
      wr_data = ((0                           << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR2_TRTW_LSB) |    //Stardard bus turn around delay; No additional delay
                 (0                           << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR2_TRTODT_LSB) |  //MOD is DDR3/DDR4 param. Don't care for LPDDR.
                 (0xf << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR2_TCMDCKE_LSB) |  
                 (0xf  << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR2_TCKE_LSB) |  
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
      wr_data = ((0                           << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR4_TAOND_TAOFD_LSB) | //DDR2 only. Don't care. Set to 0.
                 (lp4_param_db.rfcab[ddc_mode]<< (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR4_TRFC_LSB) |
                 (43                          << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR4_TWLO_LSB) |    //Not used by DDC; Use default  
                 (0x1f  << (uint32_t)PHY_REGS_DWC_DDRPHY_PUB_DTPR4_TXP_LSB) );

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
    
    /*hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR0_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR1_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR2_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR3_ADDRESS + mc_base_addr), 0x07070707 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR4_ADDRESS + mc_base_addr), 0x07070707 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR5_ADDRESS + mc_base_addr), 0x00000007 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR6_ADDRESS + mc_base_addr), 0x00070700 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR0_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR1_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR2_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR3_ADDRESS + mc_base_addr), 0x07070707 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR4_ADDRESS + mc_base_addr), 0x07070707 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR5_ADDRESS + mc_base_addr), 0x00000007 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR6_ADDRESS + mc_base_addr), 0x00070700 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2BDLR0_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2BDLR1_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2BDLR2_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2BDLR3_ADDRESS + mc_base_addr), 0x07070707 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2BDLR4_ADDRESS + mc_base_addr), 0x07070707 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2BDLR5_ADDRESS + mc_base_addr), 0x00000007 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2BDLR6_ADDRESS + mc_base_addr), 0x00070700 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2GCR0_ADDRESS + mc_base_addr), 0x40300204 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2GCR4_ADDRESS + mc_base_addr), 0x0200003c );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2GCR5_ADDRESS + mc_base_addr), 0x58585858 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3BDLR0_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3BDLR1_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3BDLR2_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3BDLR3_ADDRESS + mc_base_addr), 0x07070707 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3BDLR4_ADDRESS + mc_base_addr), 0x07070707 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3BDLR5_ADDRESS + mc_base_addr), 0x00000007 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3BDLR6_ADDRESS + mc_base_addr), 0x00070700 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3GCR0_ADDRESS + mc_base_addr), 0x40300204 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3GCR3_ADDRESS + mc_base_addr), 0xeec0010b );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3GCR4_ADDRESS + mc_base_addr), 0x0200003c );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3GCR5_ADDRESS + mc_base_addr), 0x58585858 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX4BDLR0_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX4BDLR1_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX4BDLR2_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX4BDLR3_ADDRESS + mc_base_addr), 0x07070707 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX4BDLR4_ADDRESS + mc_base_addr), 0x07070707 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX4BDLR5_ADDRESS + mc_base_addr), 0x00000007 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX4BDLR6_ADDRESS + mc_base_addr), 0x00070700 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX4GCR0_ADDRESS + mc_base_addr), 0x40300204 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX4GCR3_ADDRESS + mc_base_addr), 0xfdc0010b );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX4GCR4_ADDRESS + mc_base_addr), 0x0200003c );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX4GCR5_ADDRESS + mc_base_addr), 0x58585858 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX5BDLR0_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX5BDLR1_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX5BDLR2_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX5BDLR3_ADDRESS + mc_base_addr), 0x07070707 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX5BDLR4_ADDRESS + mc_base_addr), 0x07070707 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX5BDLR5_ADDRESS + mc_base_addr), 0x00000007 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX5BDLR6_ADDRESS + mc_base_addr), 0x00070700 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX5GCR0_ADDRESS + mc_base_addr), 0x40300204 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX5GCR3_ADDRESS + mc_base_addr), 0xfec00109 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX5GCR4_ADDRESS + mc_base_addr), 0x0200003c );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX5GCR5_ADDRESS + mc_base_addr), 0x58585858 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX6BDLR0_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX6BDLR1_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX6BDLR2_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX6BDLR3_ADDRESS + mc_base_addr), 0x07070707 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX6BDLR4_ADDRESS + mc_base_addr), 0x07070707 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX6BDLR5_ADDRESS + mc_base_addr), 0x00000007 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX6BDLR6_ADDRESS + mc_base_addr), 0x00070700 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX6GCR0_ADDRESS + mc_base_addr), 0x40300204 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX6GCR3_ADDRESS + mc_base_addr), 0xe3c0010b );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX6GCR4_ADDRESS + mc_base_addr), 0x0200003c );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX6GCR5_ADDRESS + mc_base_addr), 0x58585858 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX7BDLR0_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX7BDLR1_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX7BDLR2_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX7BDLR3_ADDRESS + mc_base_addr), 0x07070707 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX7BDLR4_ADDRESS + mc_base_addr), 0x07070707 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX7BDLR5_ADDRESS + mc_base_addr), 0x00000007 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX7BDLR6_ADDRESS + mc_base_addr), 0x00070700 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX7GCR0_ADDRESS + mc_base_addr), 0x40300204 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX7GCR3_ADDRESS + mc_base_addr), 0x6ec0010b );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX7GCR4_ADDRESS + mc_base_addr), 0x0200003c );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX7GCR5_ADDRESS + mc_base_addr), 0x58585858 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8BDLR0_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8BDLR1_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8BDLR2_ADDRESS + mc_base_addr), 0x01010101 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8BDLR3_ADDRESS + mc_base_addr), 0x07070707 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8BDLR4_ADDRESS + mc_base_addr), 0x07070707 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8BDLR5_ADDRESS + mc_base_addr), 0x00000007 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8BDLR6_ADDRESS + mc_base_addr), 0x00070700 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8GCR0_ADDRESS + mc_base_addr), 0x40300204 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8GCR4_ADDRESS + mc_base_addr), 0x0200003c );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8GCR5_ADDRESS + mc_base_addr), 0x58585858 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0OSC_ADDRESS + mc_base_addr), 0x20019ffe );*/
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_ADDRESS + mc_base_addr), 0x00281800);
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_ADDRESS + mc_base_addr), 0x00281800);
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL2DXCTL2_ADDRESS + mc_base_addr), 0x00281800);
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL3DXCTL2_ADDRESS + mc_base_addr), 0x00281800);
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DQSCTL_ADDRESS + mc_base_addr), 0x012640c4);
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DQSCTL_ADDRESS + mc_base_addr), 0x012640c4);
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL2DQSCTL_ADDRESS + mc_base_addr), 0x012640c4);
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL3DQSCTL_ADDRESS + mc_base_addr), 0x012640c4);
    
    /*hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DXCCR_ADDRESS + mc_base_addr), 0x00000040 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_GPR0_ADDRESS + mc_base_addr), 0xbf0489f8 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_GPR1_ADDRESS + mc_base_addr), 0xd5587b31 );*/
}

uint32_t perform_cbt(const int mc, uint32_t * final_value) {

    uint32_t rd_data, wr_data, ACBDLR3_val, ACBDLR6_val, ACBDLR7_val, ACLCDLR_val, ACLCDLR_final, cache_addr, previous_cache_addr, rd_data_masked1, rd_data_masked2;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);
    uint32_t left_eye_done = 0;
    uint32_t left_eye_cnt = 0;
    uint32_t aclcdlr_left_eye_end = 0;
    uint32_t csbdl_left_eye_end = 0;
    uint32_t cabdl_left_eye_end = 0;
    uint32_t csbdl_csdeskew_end = 0;
    uint32_t cabdl_cadeskew_end = 0;
    uint32_t bdl_default = 4;
    uint32_t aclcdl_default = 0;
    uint32_t cs_deskew_done = 0;
    uint32_t ca_deskew_done = 0;
    uint32_t ca_deskew_done_cnt = 0;
    int i;
    DCU_CMD_STRUCT * dcu_struct_inst = malloc(sizeof(DCU_CMD_STRUCT));

    //--------------------------------------------------------------------//
    //8. Command Bus Training: 
    //--------------------------------------------------------------------//

    //  a. Enter CBT by setting MR13[0] (CBT) = 1’b1 via DCU 
    //     (Section 4.3.2.4, steps 2.a, 3, 4, 5, 6) 
    //  Section 4.3.2.4
    //  2.a.1 Satisfy the tDQSCKE constraint of setting DQS[0] low and DQS_n[0] high before pulling CKE low by setting: 
    //        DX0GCR3.DSOEMODE = 2’b01; DX0GCR3.DSNOEMODE= 2’b01.
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: a. Satisfy the tDQSCKE constraint\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GCR3_ADDRESS + mc_base_addr), 0xefd0014b );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX2GCR3_ADDRESS + mc_base_addr), 0xffd0014b );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR2_ADDRESS + mc_base_addr), 0xa0000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR3_ADDRESS + mc_base_addr), 0xffe0818b );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3GCR2_ADDRESS + mc_base_addr), 0xa0000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX3GCR3_ADDRESS + mc_base_addr), 0xeee0818b );
    
    //Set the BDL default values for the ACBDL register fields as follows:
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: a. Set the BDL default values\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR1_ADDRESS + mc_base_addr), 0x02020ae0 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR3_ADDRESS + mc_base_addr), 0x00000404 );
    
    //Set the field DCURR.DINST to 4’h4 which will reset all DCU run time registers.
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: a. Set the field DCURR.DINST to 4’h4\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x00000004 );
    
    //these reads are in the regdump, not 100% sure why they are here
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_CATR1_ADDRESS + mc_base_addr)); // 0x0103cafb
    if(rd_data != 0x0103cafb ) {
      hw_status("KAHALU_SIM_INIT: WARNING CATR1 read back 0x%0x instead of 0x80043210 \n", rd_data);
    }
        
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_CATR0_ADDRESS + mc_base_addr)); // 0x001f1354 
    if(rd_data != 0x001f1354 ) {
      hw_status("KAHALU_SIM_INIT: WARNING CATR0 read back 0x%0x instead of 0x80043210 \n", rd_data);
    }
        
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTPR1_ADDRESS + mc_base_addr)); // 0x57680017
    if(rd_data != 0x57680017 ) {
      hw_status("KAHALU_SIM_INIT: WARNING DTPR1 read back 0x%0x instead of 0x80043210 \n", rd_data);
    }
    
    //Send the mode register write command for MR13.CBT field to start the CBT. tMRD is the timing requirement after the MR13 write and before pulling CKE low.
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: a. Send the mode register write command for MR13.CBT field to start the CBT\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000004 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x1f10ab14 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000014 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x47ac73b4 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000024 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x20490d00 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000034 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x000001c0 );
    
    //Load DCUTPR.tDCUT1 with 4. The DCU will calculate the tMRD from the first tCK of the 1st CS pulse. But as per the Jedec it should be from the 4th tCK cycle after the 1st CS pulse.
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: a. Load DCUTPR.tDCUT1 with 4\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000005 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x98d83420 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000015 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x3608b514 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000025 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0xe0000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000035 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x000001d1 );
    
   
    //Execute DCU and Wait for DCU done;
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: Execute DCU and Wait for DCU done\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUTPR_ADDRESS + mc_base_addr), 0x01900417 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x00000004 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x00000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUGCR_ADDRESS + mc_base_addr), 0x00000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x00018541 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr)); // 0x00000001
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr)); // 0x00000001
    }
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR1_ADDRESS + mc_base_addr)); // 0x00000000
    if(rd_data != 0x00000000 ) {
      hw_status("KAHALU_SIM_INIT: WARNING DCUSR1 read back 0x%0x instead of 0x00000000 \n", rd_data);
    }
    
    //  b. Set CKE low via DCU (Section 4.3.2.4, step 7)
    //Pull the CKE low. tCAENT is the timing requirement after pulling CKE low and before sending VREFCA command. Hence load DCUTPR.tDCUT2 with (250/dram clk) corresponding to tCAENT.
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: b. Set CKE low via DCU (Section 4.3.2.4, step 7)\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000001 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x3bd409a4 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000011 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x9c4445f8 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000021 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0xe0000200 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000031 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x000001e4 );
    
    //Execute DCU and Wait for DCU done;
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: Execute DCU and Wait for DCU done\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUTPR_ADDRESS + mc_base_addr), 0x019036d4 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x00018544 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x00018540 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUGCR_ADDRESS + mc_base_addr), 0x00000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x00214111 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr)); // 0x00000001
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr)); // 0x00000001
    } 
    
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR1_ADDRESS + mc_base_addr)); // 0x00000000
    if(rd_data != 0x00000000 ) {
      hw_status("KAHALU_SIM_INIT: WARNING DCUSR1 read back 0x%0x instead of 0x00000000 \n", rd_data);
    } 
     
    //  c. If  FA requires PLL, remove PLL bypass mode 
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: c. If  FA requires PLL, remove PLL bypass mode \n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PLLCR0_ADDRESS + mc_base_addr), 0x001c0000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SLBPLLCR0_ADDRESS + mc_base_addr), 0x001c0000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SLBPLLCR0_ADDRESS + mc_base_addr), 0x001c0000 );
    
    
    
    //  d. Switch from tCKb (boot clock) to FA (functional clock) 
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: d. Switch from tCKb (boot clock) to FA (functional clock) \n");
    enable_fast_clocks(mc);
    
    //  e. Host programs PIR[29] (DCALPSE) = 1’b0 to resume the MDL calibrations
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x00000000 );
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: e. Host programs PIR[29] (DCALPSE) = 1’b0 to resume the MDL calibrations\n");
    
    
    //  f. Execute DDL, PLL lock at FA. Set PGCR2.CLRZCAL=1 to execute ZQ calibration.
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: f. Execute DDL, PLL lock at FA. Set PGCR2.CLRZCAL=1 to execute ZQ calibration.\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR2_ADDRESS + mc_base_addr), 0x40f05ff0 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x00000071 );
    
    
    //  g. Host waits for PGSR0.IDONE = 1’b1 and PGSR0.ZCDONE=1'b1.
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: g. Host waits for PGSR0.IDONE\n");
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));
    }
     
    //  h. Host programs PIR[29] (DCALPSE) = 1’b1 to pause the MDL calibrations
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: h. Host programs PIR[29] (DCALPSE) = 1’b1 to pause the MDL calibrations\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x20000000 );
    
    
    //  i. Program the registers required for CBT explained in Section 4.3.2.4, 2.b.
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: i. Program the registers required for CBT\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR3_ADDRESS + mc_base_addr), 0xaa550010 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACLCDLR_ADDRESS + mc_base_addr), 0x003e003e );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACIOCR0_ADDRESS + mc_base_addr), 0x30070001 );

    //  j. Perform CBT using DCU (Section 4.3.2.4, steps 8 through 15) 
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: j. Perform CBT using DCU (Section 4.3.2.4, steps 8 through 15)\n");
    //Send VREFCA pattern. tVREFca_Long is the timing requirement after sending VrefCA and before sending CA Calibration pattern. It is identical to tCAENT hence use the same DCUTPR.tDCUT2 field.
     hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: 8. Send VREFCA pattern.)\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x00214114 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_CATR1_ADDRESS + mc_base_addr)); // 0x0103cafb 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_CATR0_ADDRESS + mc_base_addr)); // 0x001f1354 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTPR1_ADDRESS + mc_base_addr)); // 0x57680017 
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000002 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x55555555 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000012 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x55555555 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000022 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0xe0000900 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000032 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x000011e0 );
    
    //Execute DCU and Wait for DCU done;
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: Execute DCU and Wait for DCU done\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUTPR_ADDRESS + mc_base_addr), 0x01900000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x00214114 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x00214110 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUGCR_ADDRESS + mc_base_addr), 0x00000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x00c4f221 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr));
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr));
    } 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR1_ADDRESS + mc_base_addr)); // 0x00000000
    
    //increment CA BDL by 2 taps
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: reading back initial values\n");
    ACBDLR6_val = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR6_ADDRESS + mc_base_addr)); // 0x04040404 
    ACBDLR7_val = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR7_ADDRESS + mc_base_addr)); // 0x04040404
    ACLCDLR_val = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACLCDLR_ADDRESS + mc_base_addr));
    ACBDLR3_val = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR3_ADDRESS + mc_base_addr));
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: ACBDLR3_val initial vlaue is 0x%0x\n",ACBDLR3_val);
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: ACBDLR6_val initial vlaue is 0x%0x\n",ACBDLR6_val);
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: ACBDLR7_val initial vlaue is 0x%0x\n",ACBDLR7_val);
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: ACLCDLR_val initial vlaue is 0x%0x\n",ACLCDLR_val);
    
    
    aclcdl_default = ACLCDLR_val & 0xfff;
    
    /*ACBDLR6_val += 0x02020202;
    ACBDLR7_val += 0x0202;
    bdl_default += 2;
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR6_ADDRESS + mc_base_addr), ACBDLR6_val );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR7_ADDRESS + mc_base_addr), ACBDLR7_val );*/
    
    //cbt_steps_12_15(mc);
    previous_cache_addr = 0;
    cache_addr = 0;
    
    //STEP12
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: left eye training start\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUTPR_ADDRESS + mc_base_addr), 0x002a0000 );
    
    while(left_eye_done == 0) {
        previous_cache_addr = cache_addr;
        do {
#ifdef MCU_IP_VERIF
            cache_addr = rand_interval(0,15);
#else
            cache_addr += 1;
            cache_addr = cache_addr % 16;
#endif
        }
        while (previous_cache_addr == cache_addr);
        dcu_struct_inst->data0 = rand();
        dcu_struct_inst->data1 = rand();
        dcu_struct_inst->mask = 0;
        dcu_struct_inst->addr = 0xc;
        dcu_struct_inst->bank = 0;
        dcu_struct_inst->rank = 0;
        dcu_struct_inst->cmd = 7;
        dcu_struct_inst->tag = 0;        
        dcu_struct_inst->dtp = 30; 
        dcu_struct_inst->rpt = 0;
        dcu_struct_inst->dcu_cache_addr = cache_addr;
        dcu_struct_inst->previous_dcu_cache_addr = previous_cache_addr;
        write_single_dcu_reg(mc, dcu_struct_inst,1, 0, 0);
        
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR1_ADDRESS + mc_base_addr)); // 0x00000000
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR6_ADDRESS + mc_base_addr)); // 0x3f000000
        rd_data_masked1 = rd_data & 0x3f000000;
        hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR6_ADDRESS read back 0x%0x\n",rd_data_masked1);
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR6_ADDRESS + mc_base_addr)); // 0x3f000000
        rd_data_masked2 = rd_data & 0x3f000000;
        hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR6_ADDRESS read back 0x%0x\n",rd_data_masked2); 
        
        if((rd_data_masked1 != 0x3f000000) || (rd_data_masked2 != 0x3f000000)) {
            left_eye_done = 1;
            aclcdlr_left_eye_end = ACLCDLR_val & 0xfff;
            cabdl_left_eye_end = ACBDLR6_val & 0xff;
            csbdl_left_eye_end = ACBDLR3_val;
        }
        else if((ACLCDLR_val & 0xfff) != 0x1ff) {
            ACLCDLR_val += 0x0010001;
            hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACLCDLR_ADDRESS + mc_base_addr), ACLCDLR_val );
        }
        else {
            hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: left eye training max reached (ACLCDLR_val & 0xfff) is 0x%0x\n", (ACLCDLR_val & 0xfff));
            ACLCDLR_val = 0x003e003e;
            hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACLCDLR_ADDRESS + mc_base_addr), 0x003e003e );
            ACBDLR6_val += 0x02020202;
            ACBDLR7_val += 0x0202;
            hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: left eye training max reached incrementing ACBDLR6_val to 0x%0x\n", ACBDLR6_val);
            hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: left eye training max reached incrementing ACBDLR7_val to 0x%0x\n", ACBDLR7_val);
            hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR6_ADDRESS + mc_base_addr), ACBDLR6_val );
            hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR7_ADDRESS + mc_base_addr), ACBDLR7_val );
            left_eye_cnt++;
        }
        if(left_eye_cnt==5) {
            hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: ERROR: left eye training max reached 5 times something is wrong\n");
            left_eye_done = 1;
            aclcdlr_left_eye_end = ACLCDLR_val & 0xfff;
            cabdl_left_eye_end = ACBDLR6_val & 0xff;
            csbdl_left_eye_end = ACBDLR3_val;
            return 0;
        }
          
    }
    
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: left eye training done ACLCDLR_val is 0x%0x\n", ACLCDLR_val);
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: cs deskew start\n");
    
    //step 13
    while(cs_deskew_done == 0) {
        previous_cache_addr = cache_addr;
        do {
#ifdef MCU_IP_VERIF
            cache_addr = rand_interval(0,15);
#else
            cache_addr += 1;
            cache_addr = cache_addr % 16;
#endif
        }
        while (previous_cache_addr == cache_addr);
        dcu_struct_inst->data0 = rand();
        dcu_struct_inst->data1 = rand();
        dcu_struct_inst->mask = 0;
        dcu_struct_inst->addr = 0xc;
        dcu_struct_inst->bank = 0;
        dcu_struct_inst->rank = 0;
        dcu_struct_inst->cmd = 7;
        dcu_struct_inst->tag = 0;        
        dcu_struct_inst->dtp = 30; 
        dcu_struct_inst->rpt = 0;
        dcu_struct_inst->dcu_cache_addr = cache_addr;
        dcu_struct_inst->previous_dcu_cache_addr = previous_cache_addr;
        write_single_dcu_reg(mc, dcu_struct_inst,1, 0, 0);
        
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR1_ADDRESS + mc_base_addr)); // 0x00000000
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR6_ADDRESS + mc_base_addr)); // 0x3f000000
        rd_data_masked1 = rd_data & 0x3f000000;
        hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR6_ADDRESS read back 0x%0x\n",rd_data_masked1);
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR6_ADDRESS + mc_base_addr)); // 0x3f000000
        rd_data_masked2 = rd_data & 0x3f000000;
        hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR6_ADDRESS read back 0x%0x\n",rd_data_masked2); 
        
        if((rd_data_masked1 == 0x3f000000) || (rd_data_masked2 == 0x3f000000)) {
            cs_deskew_done = 1;
        }
        else if((ACBDLR3_val & 0x3f) != 0x3f) {
            ACBDLR3_val += 1;
            hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR3_ADDRESS + mc_base_addr), ACBDLR3_val );
        }
        else {
            hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: cs deskew max reached (ACBDLR3_val & 0xff) is 0x%0x\n", (ACBDLR3_val & 0xff));
            ACBDLR3_val = csbdl_left_eye_end;
            hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: since cs deskew max reached resetting ACBDLR3  back to 0x%0x\n", ACBDLR3_val);
            hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR3_ADDRESS + mc_base_addr), ACBDLR3_val );
            cs_deskew_done = 1;
        }
          
    }
    
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: cs deskew done ACBDLR3_val is 0x%0x\n", ACBDLR3_val);
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: ca deskew start\n");
    
    //step 14
    while(ca_deskew_done == 0) {
        previous_cache_addr = cache_addr;
        do {
#ifdef MCU_IP_VERIF
            cache_addr = rand_interval(0,15);
#else
            cache_addr += 1;
            cache_addr = cache_addr % 16;
#endif
        }
        while (previous_cache_addr == cache_addr);
        dcu_struct_inst->data0 = rand();
        dcu_struct_inst->data1 = rand();
        dcu_struct_inst->mask = 0;
        dcu_struct_inst->addr = 0xc;
        dcu_struct_inst->bank = 0;
        dcu_struct_inst->rank = 0;
        dcu_struct_inst->cmd = 7;
        dcu_struct_inst->tag = 0;        
        dcu_struct_inst->dtp = 30; 
        dcu_struct_inst->rpt = 0;
        dcu_struct_inst->dcu_cache_addr = cache_addr;
        dcu_struct_inst->previous_dcu_cache_addr = previous_cache_addr;
        write_single_dcu_reg(mc, dcu_struct_inst,1, 0, 0);
        
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR1_ADDRESS + mc_base_addr)); // 0x00000000
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR6_ADDRESS + mc_base_addr)); // 0x3f000000
        rd_data_masked1 = rd_data & 0x3f000000;
        hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR6_ADDRESS read back 0x%0x\n",rd_data_masked1);
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR6_ADDRESS + mc_base_addr)); // 0x3f000000
        rd_data_masked2 = rd_data & 0x3f000000;
        hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR6_ADDRESS read back 0x%0x\n",rd_data_masked2); 
        
        if((rd_data_masked1 == 0x3f000000) && (rd_data_masked2 == 0x3f000000)) {
            ca_deskew_done = 1;
        }
        else {
            
            ca_deskew_done_cnt = 0;
            for(i = 0;i<6;i++) {
                uint32_t rd_data_masked1_shift = rd_data_masked1 >> 24;
                uint32_t rd_data_masked2_shift = rd_data_masked2 >> 24;
                uint32_t current_bit = (((rd_data_masked1_shift >> i) & 1) & ((rd_data_masked2_shift >> i) & 1));

                if(current_bit == 0) {
                    if(get_ca_bdl(i, ACBDLR6_val, ACBDLR7_val) < 0x3f) {
                        inc_ca_bdl(i, &ACBDLR6_val, &ACBDLR7_val);
                    }
                    else {
                        ca_deskew_done_cnt++;
                    }
                }
                else {
                    ca_deskew_done_cnt++;
                }
            }
            if(ca_deskew_done_cnt == 6) {
              hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: ERROR went through whole CA deskew and didn't get all to match\n"); 
              return 1;
            }
            hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR6_ADDRESS + mc_base_addr), ACBDLR6_val );
            hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR7_ADDRESS + mc_base_addr), ACBDLR7_val );
        }
    }
    
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: ca deskew done ACBDLR6_val is 0x%0x\n", ACBDLR6_val);
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: ca deskew done ACBDLR7_val is 0x%0x\n", ACBDLR7_val);
    
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: step 16 doing ACLCDLR final calculation\n");
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: aclcdlr_left_eye_end is 0x%0x\n", aclcdlr_left_eye_end);
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: cabdl_left_eye_end is 0x%0x\n", cabdl_left_eye_end);
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: aclcdl_default is 0x%0x\n", aclcdl_default);
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: bdl_default is 0x%0x\n", bdl_default);
    ACLCDLR_final = aclcdlr_left_eye_end + cabdl_left_eye_end - aclcdl_default - bdl_default;
    ACLCDLR_final = ACLCDLR_final | (ACLCDLR_final<<16);
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: ACLCDLR final calculation math is aclcdlr_left_eye_end + cabdl_left_eye_end - aclcdl_default - bdl_default\n");
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: ACLCDLR final calculation is 0x%0x\n", ACLCDLR_final);
    
    
    //  k. Save ACLCDLR[ACD,ACD1] fields (Section 4.3.2.4, step 16, 17) to be used later.
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: k. Save ACLCDLR[ACD,ACD1] fields\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACLCDLR_ADDRESS + mc_base_addr), ACLCDLR_final );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACLCDLR_ADDRESS + mc_base_addr));
    
    //  l. Revert PGCR3[31:16] (CKEN/CKNEN) 
    //     i. PGCR3[23:16] (CKEN) = 8’ b10101010 
    //     ii. PGCR3[31:24] (CKNEN) = 8' b01010101
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: l. Revert PGCR3\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR3_ADDRESS + mc_base_addr), 0x55aa0010 );
    
    //  m. Place PLL in bypass mode using phy_ctl_byp_clk period equal to tCKb
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: m. Place PLL in bypass mode using phy_ctl_byp_clk period equal to tCKb\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PLLCR0_ADDRESS + mc_base_addr), 0x801c0000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PLLCR0_ADDRESS + mc_base_addr), 0xc01c0000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PLLCR0_ADDRESS + mc_base_addr), 0x801c0000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR1_ADDRESS + mc_base_addr), 0x00020ae0 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR1_ADDRESS + mc_base_addr), 0x02020ae0 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SLBPLLCR0_ADDRESS + mc_base_addr), 0x001c0000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0PLLCR0_ADDRESS + mc_base_addr), 0x001c0000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1PLLCR0_ADDRESS + mc_base_addr), 0x001c0000 );

    //  n. Switch from FA (functional clock) to tCKB (boot clock) 
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING:  n. Switch from FA (functional clock) to tCKB (boot clock) \n");
    enable_boot_clocks(mc);
    
    //  o. Host programs PIR[29] (DCALPSE) = 1’b0 to resume the MDL calibrations 
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: o. Host programs PIR[29] (DCALPSE) = 1’b0\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x00000000 );
    
    //  p. Execute DDL at tCKb. Set PGCR2.CLRZCAL=1 to execute ZQ calibration.
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: p. Execute DDL at tCKb\n");
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR2_ADDRESS + mc_base_addr)); // 0x00f05ff0 
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR2_ADDRESS + mc_base_addr), 0x40f05ff0 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x00000061 );
    
    //  q. Host waits for PGSR0.IDONE = 1’b1 and PGSR0.ZCDONE=1'b1.
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: q. Host waits for PGSR0.IDONE\n");
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));
    }
    
    //  r. Exit CBT by setting CKE high via DCU (Section 4.3.2.4, step 18)
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING:  r. Exit CBT by setting CKE high via DCU (Section 4.3.2.4, step 17,18)\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000006 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0xc824a094 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000016 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x3b8044dc );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000026 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0xe0000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000036 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x000001c5 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000007 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0xe3548b80 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000017 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x8730e9f8 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000027 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0xe0000300 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000037 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x000001e4 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUTPR_ADDRESS + mc_base_addr), 0x01900022 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x00381554 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x00381550 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUGCR_ADDRESS + mc_base_addr), 0x00000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x00125761 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr)); // 0x00000001
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr)); // 0x00000001
    }
    
    //s. Set MR13[CBT] = 1’b0 via DCU (Section 4.3.2.4, step 19)
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING:  s. Set MR13[CBT] = 1’b0 via DCU (Section 4.3.2.4, step 19)\n");
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR1_ADDRESS + mc_base_addr)); // 0x00000000 
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000005 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0xdc249f10 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000015 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x9408f554 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000025 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x20480d00 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000035 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x000001c0 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUTPR_ADDRESS + mc_base_addr), 0x0000000a );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x00125764 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x00125760 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUGCR_ADDRESS + mc_base_addr), 0x00000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x001fc551 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr)); // 0x00000001
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr)); // 0x00000001
    }
    
    /*
    //more dcu commands not sure why these are here
    //writing to MR13 enablign cbt bit again for unknown reason and then SDRAM_NOP
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR1_ADDRESS + mc_base_addr)); // 0x00000000 
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x001fc554 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000403 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x3c284d94 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0xc7845ba4 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x20490d00 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x000001c0 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0xfc8ca068 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x44408b98 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0xe0000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x000001d1 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUTPR_ADDRESS + mc_base_addr), 0x01900417 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x001fc554 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x001fc550 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUGCR_ADDRESS + mc_base_addr), 0x00000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x002ed431 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr)); // 0x00000001
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr)); // 0x00000001
    }
    
    //SPECIAL_CMD CKE_LO
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR1_ADDRESS + mc_base_addr)); // 0x00000000 
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000005 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0xacbcd530 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000015 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x4fa84708 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000025 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0xe0000200 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000035 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x000001e4 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUTPR_ADDRESS + mc_base_addr), 0x01903871 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x002ed434 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x002ed430 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUGCR_ADDRESS + mc_base_addr), 0x00000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x007cc551 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr)); // 0x00000001
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr)); // 0x00000001
    } 
    
    //SPECIAL_CMD VREF CA Train command
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR1_ADDRESS + mc_base_addr)); // 0x00000000 
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000404 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x55555555 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x55555555 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0xe0000900 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x000011e0 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUTPR_ADDRESS + mc_base_addr), 0x01900000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x007cc554 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x007cc550 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUGCR_ADDRESS + mc_base_addr), 0x00000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x0023e441 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr));
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr));
    }
    
    //unknown special command
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR1_ADDRESS + mc_base_addr)); // 0x00000000 
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x465052d0 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000010 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x2f04e984 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000020 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0xe0000c00 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000030 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x000001e0 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUTPR_ADDRESS + mc_base_addr), 0x002a0000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x0023e444 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x0023e440 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUGCR_ADDRESS + mc_base_addr), 0x00000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x00048001 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr));
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr));
    }
    
    //SDRAM_NOP and SPECIAL_CMD DRAM reset low
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR1_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR6_ADDRESS + mc_base_addr)); // 0x3f000000
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR6_ADDRESS + mc_base_addr)); // 0x3f000000
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000404 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0xc9d84374 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0xa49c17fc );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0xe0000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x000001c5 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x919c7f50 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x4a100df8 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0xe0000300 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x000001e4 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUTPR_ADDRESS + mc_base_addr), 0x01900022 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x00048004 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x00048000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUGCR_ADDRESS + mc_base_addr), 0x00000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x001c7541 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr));
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr));
    }
    
    //mr13 write disable cbt bit
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR1_ADDRESS + mc_base_addr)); // 0x00000000 
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000004 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x9e109c44 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000014 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x507c09f0 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000024 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x20480d00 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUAR_ADDRESS + mc_base_addr), 0x00000034 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUDR_ADDRESS + mc_base_addr), 0x000001c0 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUTPR_ADDRESS + mc_base_addr), 0x0000000a );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x001c7544 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x001c7540 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUGCR_ADDRESS + mc_base_addr), 0x00000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCURR_ADDRESS + mc_base_addr), 0x002c9441 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr));
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR0_ADDRESS + mc_base_addr));
    } 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCUSR1_ADDRESS + mc_base_addr)); // 0x00000000
    */
    //  Clear the registers used in CBT as mentioned Section 4.3.2.4, step 23.
    hw_status("KAHALU_SIM_INIT: COMMAND_BUS_TRAINING: Clear the registers used in CBT\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GCR3_ADDRESS + mc_base_addr), 0xefc0010b );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR2_ADDRESS + mc_base_addr), 0x00000000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR3_ADDRESS + mc_base_addr), 0xffc0010b );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR1_ADDRESS + mc_base_addr), 0x02020aa0 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR1_ADDRESS + mc_base_addr), 0x02020a80 );
    
    *final_value = ACLCDLR_final;
    return 0;
}

#ifdef CONF_HAS___PHY_RTL
void wait_for_pgsr0_idone(const int mc)
{
    uint32_t rd_data = 0;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);
    hw_status("KAHALU_SIM_INIT: Waiting for PGSR0.IDONE to be set\n"); 
    rd_data = 0;
    do
    {      
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));
    } while((rd_data & PHY_REGS_DWC_DDRPHY_PUB_PGSR0_IDONE_FIELD_MASK) != PHY_REGS_DWC_DDRPHY_PUB_PGSR0_IDONE_FIELD_MASK);
    hw_status("KAHALU_SIM_INIT: PGSR0.IDONE got set\n"); 
}

void set_pub_regs (const int mc, const int dram_mode, const int dram_frequency, const int ddc_mode, const int active_ranks, const int wr_dbi_dis, const int rd_dbi_dis){
    uint32_t wl_set = 0;
    uint32_t wl_val = 0;
    uint32_t rl_val = 0;
    uint32_t rtp_val = 0;
    uint32_t wr_data = 0;
    uint32_t rd_data = 0;

    uint32_t mc_base_addr = get_mcu_baseaddr(mc);
    if(dram_mode == 1){//LP4
        //MR0
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR0_ADDRESS + mc_base_addr), 0x00000000);
      
        //MR1 
        if (dram_frequency == 400) {
          hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR1_ADDRESS + mc_base_addr), 0x00000016);
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

        //MR2
        if (dram_frequency == 400) {
          hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR2_ADDRESS + mc_base_addr), 0x000000009); 
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
        
        //MR14
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR14_ADDRESS + mc_base_addr), 0x0000005a );
        
        //ODTCR
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ODTCR_ADDRESS + mc_base_addr), 0x00000000 );
        
        //RANKIDR
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS + mc_base_addr), 0x00000001 );
        
        //PGCR1
        //includes 1. c.
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR1_ADDRESS + mc_base_addr), 0x02020a80 );
        
        //PGCR2
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR2_ADDRESS + mc_base_addr), 0x00f05ff0 );
        
        //PGCR3
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR3_ADDRESS + mc_base_addr), 0x55aa0010 );
        
        //PGCR4
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR4_ADDRESS + mc_base_addr), 0x001aefc3 );
        
        //PGCR5
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR5_ADDRESS + mc_base_addr), 0xb6d40000 );
        
        //PGCR8
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR8_ADDRESS + mc_base_addr), 0x801180aa );
        
        //PTR0
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PTR0_ADDRESS + mc_base_addr), 0x10c0346f);
        
        //PTR1
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PTR1_ADDRESS + mc_base_addr), 0x2e8112c0);
        
        //PTR2
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PTR2_ADDRESS + mc_base_addr), 0x00056a9a);
        
        //PTR3
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PTR3_ADDRESS + mc_base_addr), 0x00000014);
        
        //PTR4
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PTR4_ADDRESS + mc_base_addr));
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PTR4_ADDRESS + mc_base_addr), (uint32_t)(rd_data|PHY_REGS_DWC_DDRPHY_PUB_PTR4_TDINIT1_SET(lp4_param_db.xpd[ddc_mode])) );
        
        //PTR5
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PTR5_ADDRESS + mc_base_addr), 0x00000014);
        
        //PTR6
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PTR6_ADDRESS + mc_base_addr), 0x03300640 );
        
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS + mc_base_addr), 0x00000001 );
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RDIMMCR0_ADDRESS + mc_base_addr), 0xa8dcda75 );
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RDIMMCR1_ADDRESS + mc_base_addr), 0xbc0fc3e2 );
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RDIMMCR2_ADDRESS + mc_base_addr), 0x4774fb13 );
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RDIMMCR3_ADDRESS + mc_base_addr), 0x65004110 );
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RDIMMCR4_ADDRESS + mc_base_addr), 0x13d03ac5 );
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RDIMMGCR0_ADDRESS + mc_base_addr), 0x08400004 );
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RDIMMGCR1_ADDRESS + mc_base_addr), 0x00400c80 );
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RDIMMGCR2_ADDRESS + mc_base_addr), 0x9813d0c5 );
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_VTCR0_ADDRESS + mc_base_addr), 0xf005d41a );
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_VTCR1_ADDRESS + mc_base_addr), 0x75569674 );
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ZQ0OR0_ADDRESS + mc_base_addr), 0x025003ef );
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ZQ0OR1_ADDRESS + mc_base_addr), 0x018b02f0 );
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ZQ1OR0_ADDRESS + mc_base_addr), 0x005901d3 );
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ZQ1OR1_ADDRESS + mc_base_addr), 0x0224004f );
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ZQCR_ADDRESS + mc_base_addr), 0x008c2c3c );
        
       //Reconfigure DQ/DM Mapping for DXnDQMAP;  
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0DQMAP0_ADDRESS + mc_base_addr), 0x00035678);
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0DQMAP1_ADDRESS + mc_base_addr), 0x00004012);
   
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1DQMAP0_ADDRESS + mc_base_addr), 0x00035678);
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1DQMAP1_ADDRESS + mc_base_addr), 0x00004012);
        
        //1. b. ii. Program PUB MR13[6] (FSP-WR) = 1’b1 
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR13_ADDRESS + mc_base_addr), 0x00000048 );

        //1. d. Host Programs PGCR1[6](PUBMODE) = 1’b1 to disable DFI interface. 
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR1_ADDRESS + mc_base_addr), 0x02020ac0 );

    } else {//LP3
        hw_status("KAHALU_SIM_INIT: ERROR: LP3 mode is NOT supported.\n");
    }
}

void snps_lp4_phy_pll_setup (const int mc)
{
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);
    
    //Assert the PLL bypass control
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PLLCR0_ADDRESS + mc_base_addr), 0x801c0000 );
    
    //Wait for a min of 8 cfg_clk
    hw_sleep(8*64);
    
    //Assert PLL reset
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PLLCR0_ADDRESS + mc_base_addr), 0xc01c0000 );
    
    //Wait for a min of 8 cfg_clk
    hw_sleep(8*64);
    
    //De-assert PLL reset
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PLLCR0_ADDRESS + mc_base_addr), 0x801c0000 );
    
    //Wait for a min of 8 cfg_clk
    hw_sleep(8*64);
    
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR1_ADDRESS + mc_base_addr), 0x00020ac0 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR1_ADDRESS + mc_base_addr), 0x02020ac0 );
    
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SLBPLLCR0_ADDRESS + mc_base_addr), 0x001c0000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0PLLCR0_ADDRESS + mc_base_addr), 0x001c0000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1PLLCR0_ADDRESS + mc_base_addr), 0x001c0000 );
}

int phy_real_init_synps_lp4 
     (
         const int mc,
         const int ddc_mode, 
         const int active_ranks, 
         const int dram_frequency,
         const int wr_dbi_dis, 
         const int rd_dbi_dis,
         const int skip_part_phy_init
     )
{
    uint32_t rd_data = 0;
    uint32_t wr_data = 0;
    uint32_t rd_addr = 0;
    uint32_t rl_val = 0;
    uint32_t rtp_val = 0;
    uint32_t cbt_ACLCDLR_value = 0;
    uint32_t skip_command_bus_training = 0;
    uint32_t skip_write_leveling = 0;
    int ret;

    uint32_t mc_base_addr = get_mcu_baseaddr(mc);

    hw_status("KAHALU_SIM_INIT: skip_part_phy_init is 0x%0x\n",skip_part_phy_init);
    if(skip_part_phy_init & 0x1) {
        hw_status("KAHALU_SIM_INIT: enabling skip_command_bus_training\n");
        skip_command_bus_training = 1;
    }
    if(skip_part_phy_init & 0x2) {
        hw_status("KAHALU_SIM_INIT: enabling skip_write_leveling\n");
        skip_write_leveling = 1;
    }
    
    hw_status("KAHALU_SIM_INIT: SNPS PHY REAL INIT ddc_mode = %0d, wr_dbi_dis= %0d, rd_dbi_dis = %0d\n", ddc_mode, wr_dbi_dis, rd_dbi_dis);

    program_ddc_phy_params(mc, rd_dbi_dis, dram_frequency);
      
    hw_status("KAHALU_SIM_INIT: DEASSERTING RESET to PHY\n");
    hw_write32((uint32_t*)(MCU_REGS_PHYCTRL_REGS_SNP_CTL_RST_N_ADDRESS + mc_base_addr), 0x01);
      
    hw_status("KAHALU_SIM_INIT: Basic PHY Config at Low Freq\n");

    //Ensure PGCR5[0] is programmed to 1'b0 to select the registers for freqA
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR5_ADDRESS + mc_base_addr));
    wr_data = rd_data & (~PHY_REGS_DWC_DDRPHY_PUB_PGCR5_DDLPGRW_FIELD_MASK);
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR5_ADDRESS + mc_base_addr), wr_data);
 
    //Initial programming of the phy registers with some known values
    //as per SNPS reg_dump.log
    hw_status("KAHALU_SIM_INIT: phy_regs_initial_settings\n");
    phy_regs_initial_settings(mc, dram_frequency, ddc_mode, active_ranks, wr_dbi_dis, rd_dbi_dis);
    
    //Programming DRAM Configuration (setting memory type to LP4)
    wr_data = PHY_REGS_DWC_DDRPHY_PUB_DCR_DDRMD_SET(5) | //LPDDR4
             PHY_REGS_DWC_DDRPHY_PUB_DCR_DDR8BNK_FIELD_MASK | //Uses 8-banks
             PHY_REGS_DWC_DDRPHY_PUB_DCR_BYTEMASK_SET(0x1);
    //hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCR_ADDRESS + mc_base_addr), 0x0000040d); //SNPS_reg_dump= 0x1348d
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DCR_ADDRESS + mc_base_addr), wr_data);
    
    //Driving retention enable ports of PHY top 
    hw_write32((uint32_t*)(MCU_REGS_PHYCTRL_REGS_SNP_RET_EN_N_ADDRESS + mc_base_addr), 0x1);
    hw_write32((uint32_t*)(MCU_REGS_PHYCTRL_REGS_SNP_RET_EN_I_ADDRESS + mc_base_addr), 0x2);
    hw_write32((uint32_t*)(MCU_REGS_PHYCTRL_REGS_SNP_EXT_EN_I_ADDRESS + mc_base_addr), 0x01);

    //
    // Host programs the following for frequency FA (functional frqA) 
    // i. Program PUB MR1,2,3,11, 13, 12,14,22 registers based on FrqA (FA). 
    // ii. Program DRAM MR13.FSP-WR = 1. 
    // iii. Program all other applicable PUB specific registers for Frequency FA 
    //
    hw_status("KAHALU_SIM_INIT: set_pub_regs\n");
    set_pub_regs(mc, /*dram_mode*/1, dram_frequency, ddc_mode, active_ranks, wr_dbi_dis, rd_dbi_dis);
    
    //PLL Setup
    hw_status("KAHALU_SIM_INIT: snps_lp4_phy_pll_setup\n");
    enable_boot_clocks(mc);
    snps_lp4_phy_pll_setup(mc);

    
    //PIR Settings
    hw_status("KAHALU_SIM_INIT: ZQ and DRAM Initialization using PIR register\n");
    
    //3. Host triggers DDL, ZQ and DRAM Initialization using PIR register
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x000001a3 );
    //earlier version of code sets this but not in reg dump
    //PHY_REGS_DWC_DDRPHY_PUB_PGCR2_ICPC_SET(1)
    
    //4. Host waits for PGSR0.IDONE = 1’b1
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));
    }
    
    //Make sure the ZCAL completed without errors
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ZQ0SR_ADDRESS + mc_base_addr));
    if ((rd_data & 0x300) != 0x200) {
        hw_status("KAHALU_SIM_INIT: ERROR: ZCAL Failed: ZQ0SR = 0x%0x\n", rd_data);
    } else {
        hw_status("KAHALU_SIM_INIT: ZCAL success: ZQ0SR = 0x%0x\n", rd_data);
    }
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ZQ1SR_ADDRESS + mc_base_addr));
    if ((rd_data & 0x300) != 0x200) {
        hw_status("KAHALU_SIM_INIT: ERROR: ZCAL Failed: ZQ1SR = 0x%0x\n", rd_data);
    } else {
        hw_status("KAHALU_SIM_INIT: ZCAL success: ZQ1SR = 0x%0x\n", rd_data);
    }
    
    hw_status("KAHALU_SIM_INIT: ZCAL and DRAM Init completed\n");
    
    //Removed previous settings for phy settings that conflicted with reg dump, copied to bottom in case needed later
    
    //6. Host programs PIR[29] (DCALPSE) = 1’b1 to pause the MDL calibrations
    hw_status("KAHALU_SIM_INIT: Pause MDL Calibrations\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x20000000 );
    
    //7. Host enables VT Inhibit by programming register bit PGCR6[0] (INHVT) = 1’b1
    hw_status("KAHALU_SIM_INIT: Enabling VT Inhibit\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR6_ADDRESS + mc_base_addr), 0x00013001 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR1_ADDRESS + mc_base_addr)); // 0x40000000
    if(rd_data != 0x40000000 ) {
          hw_status("KAHALU_SIM_INIT: WARNING PGSR1 read back 0x%0x instead of 0x40000000 \n");
        }
      
    //8. Command Bus Training:
    if(skip_command_bus_training == 0) {
        hw_status("KAHALU_SIM_INIT: command bus training start\n");
        ret = perform_cbt(mc, &cbt_ACLCDLR_value);
        if(ret!=0) {
          return ret;
        }
    }
    else {
        hw_status("KAHALU_SIM_INIT: skipping command bus training\n");
    }
    
    //9. Host programs– PGCR1(PUBMODE) = 1 in case it has been cleared in Step 8.s.
    hw_status("KAHALU_SIM_INIT: 9. Host programs– PGCR1(PUBMODE) = 1 in case it has been cleared in Step 8.s.\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR1_ADDRESS + mc_base_addr), 0x02020ac0 );


    //10. Host Programs SCHCR1[2](ALLRANK)= 1’b1
    hw_status("KAHALU_SIM_INIT: 10. Host Programs SCHCR1[2](ALLRANK)= 1’b1\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_SCHCR1_ADDRESS + mc_base_addr), 0x00000004 );
    
    //11. Host Programs SCHCR0.SCHTRIG to 1, SCHCR0[7:4](CMD) = SPECIAL_COMMAND and SCHCR0[11:8](SP_CMD) = CKE_LO to bring dram CKE low
    hw_status("KAHALU_SIM_INIT: 11. Host Programs SCHCR0.SCHTRIG to 1, SCHCR0[7:4](CMD) = SPECIAL_COMMAND and SCHCR0[11:8](SP_CMD) = CKE_LO to bring dram CKE low.\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_SCHCR0_ADDRESS + mc_base_addr), 0x00000271 );
    
    //12. Wait for tctrl_delay to complete the command execution
    //FIXME: TODO: This needs to be tuned correctly; for now using some large
    //arbitrary value.
    hw_status("KAHALU_SIM_INIT: 12. Wait for tctrl_delay to complete the command execution\n");
    hw_sleep(10000);
    
    //13. Host programs PIR[29] (DCALPSE) = 1’b1 to pause the MDL calibrations
    hw_status("KAHALU_SIM_INIT: 13. Host programs PIR[29] (DCALPSE) = 1’b1 to pause the MDL calibrations\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x20000000 );
    
    //14. If frequency FA requires PLL, remove PLL bypass mode
    hw_status("KAHALU_SIM_INIT: 14. If frequency FA requires PLL, remove PLL bypass mode\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PLLCR0_ADDRESS + mc_base_addr), 0x001c0000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SLBPLLCR0_ADDRESS + mc_base_addr), 0x001c0000 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SLBPLLCR0_ADDRESS + mc_base_addr), 0x001c0000 );
    
    
    //15. Host/System switch from tCKb (boot clock) to FA (functional clock)
    hw_status("KAHALU_SIM_INIT: 15. Host/System switch from tCKb (boot clock) to FA (functional clock)\n");
    enable_fast_clocks(mc);
    
    
    //16. Host programs PIR[29] (DCALPSE) = 1’b0 to resume the MDL calibrations
    hw_status("KAHALU_SIM_INIT: 16. Host programs PIR[29] (DCALPSE) = 1’b0 to resume the MDL calibrations\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x00000000 );
    
    //17. Host performs DDL, PLL (if required) using PIR and sets PGCR2.CLRZCAL=1 to execute ZQ calibration.
    hw_status("KAHALU_SIM_INIT: 17. Host performs DDL, PLL (if required) using PIR and sets PGCR2.CLRZCAL=1 to execute ZQ calibration.\n");
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR2_ADDRESS + mc_base_addr)); // 0x00f05ff0 
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR2_ADDRESS + mc_base_addr), 0x40f05ff0 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x00000071 );
    
    //18. Host waits for PGSR0.IDONE=1 and PGSR0.ZCDONE=1'b1
    hw_status("KAHALU_SIM_INIT: 18. Host waits for PGSR0.IDONE=1 and PGSR0.ZCDONE=1'b1\n");
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));
    while((rd_data & 0x9) != 0x9)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr)); //// 0x8000001f 
    }
    
    //19. Invert PGCR3[31:16] (CKEN/CKNEN) and program ACLCDLR[ACD,ACD1] from step 8.k
    //a. PGCR3[23:16] (CKEN) = 8’ b01010101
    //b. PGCR3[31:24] (CKNEN) = 8' b10101010
    hw_status("KAHALU_SIM_INIT: 19. Invert PGCR3[31:16] (CKEN/CKNEN) and program ACLCDLR[ACD,ACD1] from step 8.k\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR3_ADDRESS + mc_base_addr), 0xaa550010 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACLCDLR_ADDRESS + mc_base_addr)); // 0x00000000
    if(skip_command_bus_training == 0) {
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACLCDLR_ADDRESS + mc_base_addr), cbt_ACLCDLR_value );
    }
    else {
        hw_status("KAHALU_SIM_INIT: skipping write to ACLCDLR since skipped command bus training\n");
    }
    
    
    //20. Host disables VT Inhibit by programming register bit PGCR6[0] (INHVT) = 1’b0
    hw_status("KAHALU_SIM_INIT: 20. Host disables VT Inhibit by programming register bit PGCR6[0] (INHVT) = 1’b0\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR6_ADDRESS + mc_base_addr), 0x00013000 );
    
    //21. Host Programs PUB MR13[7](FSP-OP) = 1’b1
    hw_status("KAHALU_SIM_INIT: 21. Host Programs PUB MR13[7](FSP-OP) = 1’b1\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR13_ADDRESS + mc_base_addr), 0x000000c8 );
    
    //adding some delay here, not sure if necessary
    hw_sleep(1000);
    
    //22. Host Programs SCHCR1[2](ALLRANK) = 1’b1
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_SCHCR1_ADDRESS + mc_base_addr), 0x00000004 );
    
    //23. Host programs SCHCR0.SCHTRIG to 1, SCHCR0[7:4](CMD) = SPECIAL_COMMAND and SCHCR0[11:8](SP_CMD) = CKE_HI to bring CKE high.
    hw_status("KAHALU_SIM_INIT: 23. Host programs SCHCR0.SCHTRIG to 1, SCHCR0[7:4](CMD) = SPECIAL_COMMAND and SCHCR0[11:8](SP_CMD) = CKE_HI to bring CKE high.\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_SCHCR0_ADDRESS + mc_base_addr), 0x00000371 );
    
    //24. Host programs SCHCR0.CMD to issue LOAD_MODE with MR13.FSP-OP=1
    hw_status("KAHALU_SIM_INIT: 24. Host programs SCHCR0.CMD to issue LOAD_MODE with MR13.FSP-OP=1.\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_SCHCR1_ADDRESS + mc_base_addr), 0x00c80d04 );
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_SCHCR0_ADDRESS + mc_base_addr), 0x00000311 );
    
    //adding some delay here, not sure if necessary
    hw_sleep(1000);
    
    //25. Host Programs PGCR1[6](PUBMODE) = 1’b0 to allow controller traffic
    hw_status("KAHALU_SIM_INIT: 25. Host Programs PGCR1[6](PUBMODE) = 1’b0 to allow controller traffic\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR1_ADDRESS + mc_base_addr), 0x02020a80 );
    
    //adding some delay here, not sure if necessary
    hw_sleep(1000);
    
    //26. Host performs all required trainings (except CBT and dram init) on FA using PIR registers.
    hw_status("KAHALU_SIM_INIT: 26. Host performs all required trainings (except CBT and dram init) on FA using PIR registers.\n");
    
    
    hw_status("KAHALU_SIM_INIT: adding delay here to match Synopsys example\n");
    hw_sleep(10000);

    //enable write leveling
    if(skip_write_leveling == 0) {
        hw_status("KAHALU_SIM_INIT: Start Write leveling.\n");
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x00000201 );
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));
        while((rd_data & 0x1) != 1)
        {
            rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr)); // 0x8000003f 
        }
    }
    else {
        hw_status("KAHALU_SIM_INIT: Skipping Write leveling.\n");
    }
    
    //enable Read DQS Gate Training
    hw_status("KAHALU_SIM_INIT: Start Read DQS Gate Training.\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x00000401 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr)); // 0x8000007f 
    }
    
    //enable Write DQS2DQ Training
    hw_status("KAHALU_SIM_INIT: Start Write DQS2DQ Training.\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x00100001 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr)); // 0x8000807f 
    }
    
    //enable Write Leveling Adjust
    hw_status("KAHALU_SIM_INIT: Start Write Leveling Adjust.\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x00000801 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr)); // 0x800080ff 
    }
    
    //enable Read Data Bit Deskew
    hw_status("KAHALU_SIM_INIT: Start Read Data Bit Deskew.\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x00001001 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr)); // 0x800081ff 
    }
    
    //enable Write Data Bit Deskew
    hw_status("KAHALU_SIM_INIT: Start Write Data Bit Deskew.\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x00002001 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr)); // 0x800083ff 
    }
    
    //enable Read Data Eye Training
    hw_status("KAHALU_SIM_INIT: Start Read Data Eye Training.\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x00004001 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr)); // 0x800087ff 
    }
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTEDR0_ADDRESS + mc_base_addr)); // 0x0001ae97 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTEDR1_ADDRESS + mc_base_addr)); // 0x00008c07
    
    //enable Write Data Eye Training
    hw_status("KAHALU_SIM_INIT: Start Write Data Eye Training.\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x00008001 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr));
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr)); // 0x80008fff 
    }
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTEDR0_ADDRESS + mc_base_addr)); // 0x0001ae97 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTEDR1_ADDRESS + mc_base_addr)); // 0x00008c07 
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GCR0_ADDRESS + mc_base_addr), 0x40900204 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DTCR1_ADDRESS + mc_base_addr)); // 0x00030236 
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR0_ADDRESS + mc_base_addr), 0x40900204 );
    
    //VREF Training
    hw_status("KAHALU_SIM_INIT: Start VREF Training.\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr), 0x00020001 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr)); // 0x80008ffe 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr)); // 0x80008ffe 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr)); // 0x80008ffe 
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS + mc_base_addr), 0x00000000 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS + mc_base_addr)); // 0x00000039 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr)); // 0x80008ffe 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr)); // 0x80008ffe 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr)); // 0x80008ffe 
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS + mc_base_addr), 0x00010000 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS + mc_base_addr)); // 0x00000039 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr)); // 0x80008ffe 
    while((rd_data & 0x1) != 1)
    {
        rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr)); // 0x8000cffe 
    }
    
    //dump out registers
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PIR_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GSR0_ADDRESS + mc_base_addr)); // 0x007f1fa0 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GSR1_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GSR2_ADDRESS + mc_base_addr)); // 0x1f800000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0RSR0_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0RSR3_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0RSR1_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0RSR2_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GSR3_ADDRESS + mc_base_addr)); // 0x02300000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR0_ADDRESS + mc_base_addr)); // 0x007f1fa0 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR1_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR2_ADDRESS + mc_base_addr)); // 0x1f800000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1RSR0_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1RSR3_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1RSR1_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1RSR2_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR3_ADDRESS + mc_base_addr)); // 0x02300000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GCR0_ADDRESS + mc_base_addr)); // 0x40900204 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR0_ADDRESS + mc_base_addr)); // 0x40900204 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_VTDR_ADDRESS + mc_base_addr)); // 0x7f001d10 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR2_ADDRESS + mc_base_addr)); // 0x00f05ff0 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GSR0_ADDRESS + mc_base_addr)); // 0x007f1f80 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GSR1_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GSR2_ADDRESS + mc_base_addr)); // 0x1f800000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0RSR0_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0RSR3_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0RSR1_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0RSR2_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GSR3_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR0_ADDRESS + mc_base_addr)); // 0x007f1f80 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR1_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR2_ADDRESS + mc_base_addr)); // 0x1f800000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1RSR0_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1RSR3_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1RSR1_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1RSR2_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR3_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr)); // 0x80000017 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACMDLR0_ADDRESS + mc_base_addr)); // 0x003f003f 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACMDLR1_ADDRESS + mc_base_addr)); // 0x003f003f 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0MDLR0_ADDRESS + mc_base_addr)); // 0x003f003f 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0MDLR1_ADDRESS + mc_base_addr)); // 0x00000080 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1MDLR0_ADDRESS + mc_base_addr)); // 0x003f003f 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1MDLR1_ADDRESS + mc_base_addr)); // 0x00000040 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GSR0_ADDRESS + mc_base_addr)); // 0x007f1f80 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR0_ADDRESS + mc_base_addr)); // 0x007f1f80 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GSR2_ADDRESS + mc_base_addr)); // 0x1f800000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GSR2_ADDRESS + mc_base_addr)); // 0x1f800000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACLCDLR_ADDRESS + mc_base_addr)); // 0x00360036 
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS + mc_base_addr), 0x00000000 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR0_ADDRESS + mc_base_addr)); // 0x0000003d 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS + mc_base_addr)); // 0x00000039 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR2_ADDRESS + mc_base_addr)); // 0x00000009 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR3_ADDRESS + mc_base_addr)); // 0x00000026 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR4_ADDRESS + mc_base_addr)); // 0x00000026 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR5_ADDRESS + mc_base_addr)); // 0x00000007 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR0_ADDRESS + mc_base_addr)); // 0x0000003d 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR1_ADDRESS + mc_base_addr)); // 0x00000039 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR2_ADDRESS + mc_base_addr)); // 0x00000009 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR3_ADDRESS + mc_base_addr)); // 0x00000026 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR4_ADDRESS + mc_base_addr)); // 0x00000026 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR5_ADDRESS + mc_base_addr)); // 0x00000007 
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS + mc_base_addr), 0x00010000 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR0_ADDRESS + mc_base_addr)); // 0x0000003d 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR1_ADDRESS + mc_base_addr)); // 0x00000039 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR2_ADDRESS + mc_base_addr)); // 0x00000009 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR3_ADDRESS + mc_base_addr)); // 0x00000026 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR4_ADDRESS + mc_base_addr)); // 0x00000026 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0LCDLR5_ADDRESS + mc_base_addr)); // 0x00000007 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR0_ADDRESS + mc_base_addr)); // 0x0000003d 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR1_ADDRESS + mc_base_addr)); // 0x00000039 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR2_ADDRESS + mc_base_addr)); // 0x00000009 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR3_ADDRESS + mc_base_addr)); // 0x00000026 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR4_ADDRESS + mc_base_addr)); // 0x00000026 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1LCDLR5_ADDRESS + mc_base_addr)); // 0x00000007 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR0_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR1_ADDRESS + mc_base_addr)); // 0x00040404 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR2_ADDRESS + mc_base_addr)); // 0x00040404 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR3_ADDRESS + mc_base_addr)); // 0x00000406 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR4_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR5_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR6_ADDRESS + mc_base_addr)); // 0x06060606 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR7_ADDRESS + mc_base_addr)); // 0x04040606 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR8_ADDRESS + mc_base_addr)); // 0x04040404 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR9_ADDRESS + mc_base_addr)); // 0x04040404 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR15_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACBDLR16_ADDRESS + mc_base_addr)); // 0x00000000 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR0_ADDRESS + mc_base_addr)); // 0x01010101 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR1_ADDRESS + mc_base_addr)); // 0x01010101 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR2_ADDRESS + mc_base_addr)); // 0x01010101 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR3_ADDRESS + mc_base_addr)); // 0x07070707 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR4_ADDRESS + mc_base_addr)); // 0x07070707 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR5_ADDRESS + mc_base_addr)); // 0x00000007 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0BDLR6_ADDRESS + mc_base_addr)); // 0x00070700 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR0_ADDRESS + mc_base_addr)); // 0x01010101 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR1_ADDRESS + mc_base_addr)); // 0x01010101 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR2_ADDRESS + mc_base_addr)); // 0x01010101 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR3_ADDRESS + mc_base_addr)); // 0x07070707 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR4_ADDRESS + mc_base_addr)); // 0x07070707 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR5_ADDRESS + mc_base_addr)); // 0x00000007 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1BDLR6_ADDRESS + mc_base_addr)); // 0x00070700 
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS + mc_base_addr), 0x00000000 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_ADDRESS + mc_base_addr)); // 0x02020009 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GTR0_ADDRESS + mc_base_addr)); // 0x02020009 
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_RANKIDR_ADDRESS + mc_base_addr), 0x00010000 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GTR0_ADDRESS + mc_base_addr)); // 0x02020009 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GTR0_ADDRESS + mc_base_addr)); // 0x02020009 
    
    //27. Host enables dfi_init_complete generation PGCR1[17](DISDIC) = 1’b0
    hw_status("KAHALU_SIM_INIT: 27. Host enables dfi_init_complete generation PGCR1[17](DISDIC) = 1’b0\n");
    hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR1_ADDRESS + mc_base_addr), 0x02000a80 );
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR4_ADDRESS + mc_base_addr)); // 0x001a3fc3 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_ACMDLR0_ADDRESS + mc_base_addr)); // 0x003f003f 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0GCR7_ADDRESS + mc_base_addr)); // 0x00000100 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX0MDLR0_ADDRESS + mc_base_addr)); // 0x003f003f 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1GCR7_ADDRESS + mc_base_addr)); // 0x00000100 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX1MDLR0_ADDRESS + mc_base_addr)); // 0x003f003f 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL0DXCTL2_ADDRESS + mc_base_addr)); // 0x00281810 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DX8SL1DXCTL2_ADDRESS + mc_base_addr)); // 0x00281810 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGSR0_ADDRESS + mc_base_addr)); // 0x8000001f 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DSGCR_ADDRESS + mc_base_addr)); // 0x002047a8 
    rd_data = hw_phy_read32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_DSGCR_ADDRESS + mc_base_addr)); // 0x002047a8 
    
    //28. Controller performs memory reads and writes at frequency FA.
    hw_status("KAHALU_SIM_INIT: PHY INITIALIZATION COMPLETE\n");

    return 0;
    
}

#endif    

