
#include "kahalu_sim_init.h"
#include "conf_defs.h"
#include "snps_lp4_phy_bringup.h"
#include "snps_lp4_phy_full_init.h"
#include "mtc_train.h"
#include "mtc_init.h"
#include "mtc_init.c"

#define MCU_DDC_CMD_MRR         0x9
#define MCU_DDC_CMD_MRW         0xA

void program_ddc_phy_params(const int mc, const int rd_dbi_dis, const int dram_frequency) {
   uint32_t wr_data;
   uint32_t mc_base_addr = get_mcu_baseaddr(mc);

#ifndef MCU_IP_VERIF 
    if(get_platform_type() == PLAT_PAL)
    {      
        hw_write32((UINT32*)(MCU_REGS_DDC_REGS_TPHY_0_ADDRESS + mc_base_addr), 0x080c01);
        if(rd_dbi_dis == 1)
	        hw_write32((UINT32*)(MCU_REGS_DDC_REGS_TPHY_1_ADDRESS + mc_base_addr), 0x1c1e); //RL = 14x2
        else
	        hw_write32((UINT32*)(MCU_REGS_DDC_REGS_TPHY_1_ADDRESS + mc_base_addr), 0x2022); //RL = 16x2

        return;
    }

#endif //MCUP_IP_VERIF 

   //FREQ VARIATION -- Program TPHY0 for WL and MANUALLY edit WL value in database to corresond to WL value
   if(dram_frequency == 400) wr_data = 0x020401;         //WL = 6,  WRLAT=(WL+1-3)= 4,  WRCSLAT = WRLAT-2 = 2
   else if (dram_frequency == 800)  wr_data =  0x040601; //WL = 8,  WRLAT=(WL+1-3)= 6,  WRCSLAT = WRLAT-2 = 4
   else if (dram_frequency == 1066) wr_data =  0x060801; //WL = 10, WRLAT=(WL+1-3)= 8,  WRCSLAT = WRLAT-2 = 6
   else if (dram_frequency == 1333) wr_data =  0x080a01; //WL = 12, WRLAT=(WL+1-3)= 10, WRCSLAT = WRLAT-2 = 8
   else wr_data = 0x0a0c01;                              //WL = 14, WRLAT=(WL+1-3)= 12, WRCSLAT = WRLAT-2 = 10

   hw_write32((uint32_t*)(MCU_REGS_DDC_REGS_TPHY_0_ADDRESS + mc_base_addr), wr_data);

   //FREQ VARIATION -- Program TPHY1 for RL and MANUALLY edit RL value in database to corresond to WL value
   if(rd_dbi_dis == 1)
   {
    
      if(dram_frequency == 400) wr_data = 0x0608;        //RL = 10, DQSCK = 2, RDEN=(RL+DQSCK-4) = 8 , RDCSLAT = RDEN-2 = 6  
      else if (dram_frequency == 800)  wr_data = 0x0b0d; //RL = 14, DQSCK = 3, RDEN=(RL+DQSCK-4) = 13, RDCSLAT = RDEN-2 = 11
      else if (dram_frequency == 1066) wr_data = 0x1214; //RL = 20, DQSCK = 4, RDEN=(RL+DQSCK-4) = 20, RDCSLAT = RDEN-2 = 18
      else if (dram_frequency == 1333) wr_data = 0x1719; //RL = 24, DQSCK = 5, RDEN=(RL+DQSCK-4) = 25, RDCSLAT = RDEN-2 = 23
      else wr_data = 0x1c1e;                             //RL = 28, DQSCK = 6, RDEN=(RL+DQSCK-4) = 30, RDCSLAT = RDEN-2 = 28

      hw_write32((uint32_t*)(MCU_REGS_DDC_REGS_TPHY_1_ADDRESS + mc_base_addr), wr_data); //RL = 14x2
   }
   //FREQ VARIATION -- Program TPHY1 for RL and MANUALLY edit RL value in database to corresond to WL value
   else         //12, 28, 32
   {
      //  
      if(dram_frequency == 400) wr_data = 0x080a;        //RL = 12, DQSCK = 2, RDEN=(RL+DQSCK-4) = 10, RDCSLAT = RDEN-2 = 8 
      else if (dram_frequency == 800)  wr_data = 0x0d0f; //RL = 16, DQSCK = 3, RDEN=(RL+DQSCK-4) = 15, RDCSLAT = RDEN-2 = 13
      else if (dram_frequency == 1066) wr_data = 0x1416;//RL = 16, DQSCK = 3, RDEN=(RL+DQSCK-4) = 15, RDCSLAT = RDEN-2 = 13
      else if (dram_frequency == 1333) wr_data = 0x1b1d; //RL = 28, DQSCK = 5, RDEN=(RL+DQSCK-4) = 29, RDCSLAT = RDEN-2 = 27
      else wr_data = 0x2022;                             //RL = 32, DQSCK = 6, RDEN=(RL+DQSCK-4) = 34, RDCSLAT = RDEN-2 = 32      
      hw_write32((uint32_t*)(MCU_REGS_DDC_REGS_TPHY_1_ADDRESS + mc_base_addr), wr_data); //RL = 16x2
   }

}

void set_autorefresh(const int mc, int interval, int dram_mode, const int dram_frequency){

    uint32_t wr_data = 0;
    uint32_t rd_data = 0;
    
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);
    //get default value
    rd_data = hw_read32((uint32_t*)(MCU_REGS_SCH_REGS_SCH_REF_CTRL_ADDRESS + mc_base_addr));     

    //clear trefi field
    wr_data = rd_data & ~((uint32_t)SCH_REG_SCH_REF_CTRL_TREFI_FIELD_MASK);
    
    if(interval != -1)	//user specific number is given
    {

         hw_status("KAHALU_SIM_INIT:SCH refresh cont = %d\n", interval);

         //if(interval > 80) {
         //need to account for 200ns delay in DDC.
         //if(dram_frequency == 1333) interval = interval - 66; 		// 200ns/3ns
         //else if(dram_frequency == 400) interval = interval - 20;	// 200ns/10ns
         //else interval = interval - 80;					//200ns/2.5ns = 80
         //}
         wr_data |= (interval & (uint32_t)SCH_REG_SCH_REF_CTRL_TREFI_FIELD_MASK);
    }
    else
   //FREQ VARIATION -- Program SCH value
    {// use default refresh value
        /*************************************
        if(dram_mode == 1){ //LP4
            wr_data |= lp4_param_db.refi[DDC_TYP];
            hw_status("TREFI = %0d\n", lp4_param_db.refi[DDC_TYP]);
        }else{//LP3
            wr_data |= lp3_param_db.refi[DDC_TYP];
            hw_status("TREFI = %0d\n", lp3_param_db.refi[DDC_TYP]);
        }
        *************************************/

        //---- wr_data |= 1560;	//3900ns, dram freq=1600Mhz, sch = 400Mhz, 3904/2.5 = 1561.6 sch clk cycles

        if(dram_frequency == 1333){
            wr_data |= 1301; 	//1301
            interval = 1301;
        }
        else if (dram_frequency == 1066){
            wr_data |= 1041; 	//3904ns/3.75ns = 1041 cycles. 
            interval = 1041;
        }
        else if (dram_frequency == 800){
            wr_data |= 780; 	//3904ns/5ns = 780 cycles. 
            interval = 780;
        }
        else if (dram_frequency == 400){
            wr_data |= 390; 	//3904ns/10ns = 390.4 cycles. 
            interval = 390;
        }
        else{			//defualt is 1600Mhz, SCH is 400Mhz
            wr_data |= 1561;
            interval = 1561;
        }
    }
    
    hw_write32((uint32_t*)(MCU_REGS_SCH_REGS_SCH_REF_CTRL_ADDRESS + mc_base_addr), wr_data);  

    //need to set up tREFI_rate (base) to be the same as tREFI
    rd_data = hw_read32((uint32_t*)(MCU_REGS_SCH_REGS_SCH_REF_RATE_CTRL_ADDRESS + mc_base_addr));     
    rd_data = rd_data & ~((uint32_t)SCH_REG_SCH_REF_RATE_CTRL_TREFI_RATE_FIELD_MASK);
    rd_data = rd_data & ~((uint32_t)SCH_REG_SCH_REF_RATE_CTRL_HIGH_TEMP_SET_FIELD_MASK);
    wr_data = rd_data & ~((uint32_t)SCH_REG_SCH_REF_RATE_CTRL_LOW_TEMP_SET_FIELD_MASK);
    wr_data |= (interval | (3 << SCH_REG_SCH_REF_RATE_CTRL_LOW_TEMP_SET_LSB) | (3 << SCH_REG_SCH_REF_RATE_CTRL_HIGH_TEMP_SET_LSB) );

    //TFS 4651: low/high temp derate reset value is not allowed to change so we need to set them up to 0s.
    wr_data = wr_data & ~((uint32_t)SCH_REG_SCH_REF_RATE_CTRL_HIGH_TEMP_DERATE_FIELD_MASK);
    wr_data = wr_data & ~((uint32_t)SCH_REG_SCH_REF_RATE_CTRL_LOW_TEMP_DERATE_FIELD_MASK);

    hw_write32((uint32_t*)(MCU_REGS_SCH_REGS_SCH_REF_RATE_CTRL_ADDRESS + mc_base_addr), wr_data);  
   
    wr_data = (rand() % 30) + 8;
    hw_status("KAHALU_SIM_INIT:SCH MR4 read interval = %d\n", wr_data);
    hw_write32((uint32_t*)(MCU_REGS_SCH_REGS_SCH_REF_RATE_INTERVAL_ADDRESS + mc_base_addr), wr_data);  
}

void configure_dram(const int mc, int config_num){
    
    uint32_t coladdr, rowaddr, rankaddr, bankaddr;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);

    hw_status("KAHALU_SIM_INIT: configure ADDR_MAP registers using config_num = %0d\n", config_num);
    if(config_num == -1) {
        hw_status("KAHALU_SIM_INIT: Leaving the ADDR_MAP registers at default value.\n", config_num);
        return;
    }
    
    switch(config_num){
        case 1:{
            coladdr = 0xffc;
            rowaddr = 0x1fff8000;
            bankaddr = 0x7000;
            rankaddr = 0x0;
            break;
        }
        case 2:{
            coladdr = 0x63fc;
            rowaddr = 0x1fff8000;
            bankaddr = 0x1c00;
            rankaddr = 0x0;
            break;
        }
        case 3:{
            coladdr = 0x71fc;
            rowaddr = 0x1fff8000;
            bankaddr = 0xe00;
            rankaddr = 0x0;
            break;
        }
        case 4:{
            coladdr = 0x78fc;
            rowaddr = 0x1fff8000;
            bankaddr = 0x700;
            rankaddr = 0x0;
            break;
        }
        case 5:{
            coladdr = 0x7c7c;
            rowaddr = 0x1fff8000;
            bankaddr = 0x380;
            rankaddr = 0x0;
            break;
        }
        case 6:{
            coladdr = 0xffc;
            rowaddr = 0x3fff8000;
            bankaddr = 0x7000;
            rankaddr = 0x0;
            break;
        }
        case 7:{
            coladdr = 0x63fc;
            rowaddr = 0x1fff8000;
            bankaddr = 0x1c00;
            rankaddr = 0x0;
            break;
        }
        case 8:{
            coladdr = 0x71fc;
            rowaddr = 0x1fff8000;
            bankaddr = 0xe00;
            rankaddr = 0x0;
            break;
        }
        case 9:{
            coladdr = 0x78fc;
            rowaddr = 0x3fff8000;
            bankaddr = 0x700;
            rankaddr = 0x0;
            break;
        }
        case 10:{
            coladdr = 0x7c7c;
            rowaddr = 0x3fff8000;
            bankaddr = 0x380;
            rankaddr = 0x0;
            break;
        }
        case 11:{
            coladdr = 0xffc;
            rowaddr = 0x1fff8000;
            bankaddr = 0x7000;
            rankaddr = 0x20000000;
            break;
        }
        case 12:{
            coladdr = 0xffc;
            rowaddr = 0x3fff0000;
            bankaddr = 0x7000;
            rankaddr = 0x8000;
            break;
        }
        case 13:{
            coladdr = 0x63fc;
            rowaddr = 0x1fff8000;
            bankaddr = 0x1c00;
            rankaddr = 0x20000000;
            break;
        }
        case 14:{
            coladdr = 0x63fc;
            rowaddr = 0x3fff0000;
            bankaddr = 0x1c00;
            rankaddr = 0x8000;
            break;
        }
        case 15:{
            coladdr = 0xc3fc;
            rowaddr = 0x3fff0000;
            bankaddr = 0x1c00;
            rankaddr = 0x2000;
            break;
        }
        case 16:{
            coladdr = 0x71fc;
            rowaddr = 0x1fff8000;
            bankaddr = 0xe00;
            rankaddr = 0x20000000;
            break;
        }
        case 17:{
            coladdr = 0x71fc;
            rowaddr = 0x3fff0000;
            bankaddr = 0xe00;
            rankaddr = 0x8000;
            break;
        }
        case 18:{
            coladdr = 0xe1fc;
            rowaddr = 0x3fff0000;
            bankaddr = 0xe00;
            rankaddr = 0x1000;
            break;
        }
        case 19:{
            coladdr = 0x78fc;
            rowaddr = 0x1fff8000;
            bankaddr = 0x700;
            rankaddr = 0x20000000;
            break;
        }
        case 20:{
            coladdr = 0x78fc;
            rowaddr = 0x3fff0000;
            bankaddr = 0x700;
            rankaddr = 0x8000;
            break;
        }
        case 21:{
            coladdr = 0xf0fc;
            rowaddr = 0x3fff0000;
            bankaddr = 0x700;
            rankaddr = 0x800;
            break;
        }
        case 22:{
            coladdr = 0x7c7c;
            rowaddr = 0x1fff8000;
            bankaddr = 0x380;
            rankaddr = 0x20000000;
            break;
        }
        case 23:{
            coladdr = 0x7c7c;
            rowaddr = 0x3fff0000;
            bankaddr = 0x380;
            rankaddr = 0x8000;
            break;
        }
        case 24:{
            coladdr = 0xf87c;
            rowaddr = 0x3fff0000;
            bankaddr = 0x380;
            rankaddr = 0x400;
            break;
        }
        case 25:{
            coladdr = 0xffc;
            rowaddr = 0x3fff8000;
            bankaddr = 0x7000;
            rankaddr = 0x0;
            break;
        }
        case 26:{
            coladdr = 0x63fc;
            rowaddr = 0x3fff8000;
            bankaddr = 0x1c00;
            rankaddr = 0x0;
            break;
        }
        case 27:{
            coladdr = 0x71fc;
            rowaddr = 0x3fff8000;
            bankaddr = 0xe00;
            rankaddr = 0x0;
            break;
        }
        case 28:{
            coladdr = 0x78fc;
            rowaddr = 0x3fff8000;
            bankaddr = 0x700;
            rankaddr = 0x0;
            break;
        }
        case 29:{
            coladdr = 0x7c7c;
            rowaddr = 0x3fff8000;
            bankaddr = 0x380;
            rankaddr = 0x0;
            break;
        }
        case 30:{
            coladdr = 0xffc;
            rowaddr = 0x3fff8000;
            bankaddr = 0x7000;
            rankaddr = 0x40000000;
            break;
        }
        case 31:{
            coladdr = 0xffc;
            rowaddr = 0x7fff0000;
            bankaddr = 0x7000;
            rankaddr = 0x8000;
            break;
        }
        case 32:{
            coladdr = 0x63fc;
            rowaddr = 0x3fff8000;
            bankaddr = 0x1c00;
            rankaddr = 0x40000000;
            break;
        }
        case 33:{
            coladdr = 0x63fc;
            rowaddr = 0x7fff0000;
            bankaddr = 0x1c00;
            rankaddr = 0x8000;
            break;
        }
        case 34:{
            coladdr = 0xc3fc;
            rowaddr = 0x7fff0000;
            bankaddr = 0x1c00;
            rankaddr = 0x2000;
            break;
        }
        case 35:{
            coladdr = 0x71fc;
            rowaddr = 0x3fff8000;
            bankaddr = 0xe00;
            rankaddr = 0x40000000;
            break;
        }
        case 36:{
            coladdr = 0x71fc;
            rowaddr = 0x7fff0000;
            bankaddr = 0xe00;
            rankaddr = 0x8000;
            break;
        }
        case 37:{
            coladdr = 0xe1fc;
            rowaddr = 0x7fff0000;
            bankaddr = 0xe00;
            rankaddr = 0x1000;
            break;
        }
        case 38:{
            coladdr = 0x78fc;
            rowaddr = 0x3fff8000;
            bankaddr = 0x700;
            rankaddr = 0x40000000;
            break;
        }
        case 39:{
            coladdr = 0x78fc;
            rowaddr = 0x7fff0000;
            bankaddr = 0x700;
            rankaddr = 0x8000;
            break;
        }
        case 40:{
            coladdr = 0xf0fc;
            rowaddr = 0x7fff0000;
            bankaddr = 0x700;
            rankaddr = 0x800;
            break;
        }
        case 41:{
            coladdr = 0x7c7c;
            rowaddr = 0x3fff8000;
            bankaddr = 0x380;
            rankaddr = 0x40000000;
            break;
        }
        case 42:{
            coladdr = 0x7c7c;
            rowaddr = 0x7fff0000;
            bankaddr = 0x380;
            rankaddr = 0x8000;
            break;
        }
        case 43:{
            coladdr = 0xf87c;
            rowaddr = 0x7fff0000;
            bankaddr = 0x380;
            rankaddr = 0x400;
            break;
        }
        case 44:{
            coladdr = 0xffc;
            rowaddr = 0x3fff8000;
            bankaddr = 0x7000;
            rankaddr = 0x40000000;
            break;
        }
        case 45:{
            coladdr = 0xffc;
            rowaddr = 0x7fff0000;
            bankaddr = 0x7000;
            rankaddr = 0x8000;
            break;
        }
        case 46:{
            coladdr = 0x63fc;
            rowaddr = 0x3fff8000;
            bankaddr = 0x1c00;
            rankaddr = 0x40000000;
            break;
        }
        case 47:{
            coladdr = 0x63fc;
            rowaddr = 0x7fff0000;
            bankaddr = 0x1c00;
            rankaddr = 0x8000;
            break;
        }
        case 48:{
            coladdr = 0xc3fc;
            rowaddr = 0x7fff0000;
            bankaddr = 0x1c00;
            rankaddr = 0x2000;
            break;
        }
        case 49:{
            coladdr = 0x71fc;
            rowaddr = 0x3fff8000;
            bankaddr = 0xe00;
            rankaddr = 0x40000000;
            break;
        }
        case 50:{
            coladdr = 0x71fc;
            rowaddr = 0x7fff0000;
            bankaddr = 0xe00;
            rankaddr = 0x8000;
            break;
        }
        case 51:{
            coladdr = 0xe1fc;
            rowaddr = 0x7fff0000;
            bankaddr = 0xe00;
            rankaddr = 0x1000;
            break;
        }
        case 52:{
            coladdr = 0x78fc;
            rowaddr = 0x3fff8000;
            bankaddr = 0x700;
            rankaddr = 0x40000000;
            break;
        }
        case 53:{
            coladdr = 0x78fc;
            rowaddr = 0x7fff0000;
            bankaddr = 0x700;
            rankaddr = 0x8000;
            break;
        }
        case 54:{
            coladdr = 0xf0fc;
            rowaddr = 0x7fff0000;
            bankaddr = 0x700;
            rankaddr = 0x800;
            break;
        }
        case 55:{
            coladdr = 0x7c7c;
            rowaddr = 0x3fff8000;
            bankaddr = 0x380;
            rankaddr = 0x40000000;
            break;
        }
        case 56:{
            coladdr = 0x7c7c;
            rowaddr = 0x7fff0000;
            bankaddr = 0x380;
            rankaddr = 0x8000;
            break;
        }
        case 57:{
            coladdr = 0xf87c;
            rowaddr = 0x7fff0000;
            bankaddr = 0x380;
            rankaddr = 0x400;
            break;
        }
        case 58:{
            coladdr = 0x7fe;
            rowaddr = 0x1fffc000;
            bankaddr = 0x3800;
            rankaddr = 0x0;
            break;
        }
        case 59:{
            coladdr = 0x23fe;
            rowaddr = 0x1fffc000;
            bankaddr = 0x1c00;
            rankaddr = 0x0;
            break;
        }
        case 60:{
            coladdr = 0x31fe;
            rowaddr = 0x1fffc000;
            bankaddr = 0xe00;
            rankaddr = 0x0;
            break;
        }
        case 61:{
            coladdr = 0x38fe;
            rowaddr = 0x1fffc000;
            bankaddr = 0x700;
            rankaddr = 0x0;
            break;
        }
        case 62:{
            coladdr = 0x3c7e;
            rowaddr = 0x1fffc000;
            bankaddr = 0x380;
            rankaddr = 0x0;
            break;
        }
        case 63:{
            coladdr = 0x7fe;
            rowaddr = 0x1fffc000;
            bankaddr = 0x3800;
            rankaddr = 0x0;
            break;
        }
        case 64:{
            coladdr = 0x23fe;
            rowaddr = 0x1fffc000;
            bankaddr = 0x1c00;
            rankaddr = 0x0;
            break;
        }
        case 65:{
            coladdr = 0x31fe;
            rowaddr = 0x1fffc000;
            bankaddr = 0xe00;
            rankaddr = 0x0;
            break;
        }
        case 66:{
            coladdr = 0x38fe;
            rowaddr = 0x1fffc000;
            bankaddr = 0x700;
            rankaddr = 0x0;
            break;
        }
        case 67:{
            coladdr = 0x3c7e;
            rowaddr = 0x1fffc000;
            bankaddr = 0x380;
            rankaddr = 0x0;
            break;
        }
        case 68:{
            coladdr = 0x7fe;
            rowaddr = 0x1fffc000;
            bankaddr = 0x3800;
            rankaddr = 0x20000000;
            break;
        }
        case 69:{
            coladdr = 0x7fe;
            rowaddr = 0x3fff8000;
            bankaddr = 0x3800;
            rankaddr = 0x4000;
            break;
        }
        case 70:{
            coladdr = 0x23fe;
            rowaddr = 0x1fffc000;
            bankaddr = 0x1c00;
            rankaddr = 0x20000000;
            break;
        }
        case 71:{
            coladdr = 0x23fe;
            rowaddr = 0x3fff8000;
            bankaddr = 0x1c00;
            rankaddr = 0x4000;
            break;
        }
        case 72:{
            coladdr = 0x43fe;
            rowaddr = 0x3fff8000;
            bankaddr = 0x1c00;
            rankaddr = 0x2000;
            break;
        }
        case 73:{
            coladdr = 0x31fe;
            rowaddr = 0x1fffc000;
            bankaddr = 0xe00;
            rankaddr = 0x20000000;
            break;
        }
        case 74:{
            coladdr = 0x31fe;
            rowaddr = 0x3fff8000;
            bankaddr = 0xe00;
            rankaddr = 0x4000;
            break;
        }
        case 75:{
            coladdr = 0x61fe;
            rowaddr = 0x3fff8000;
            bankaddr = 0xe00;
            rankaddr = 0x1000;
            break;
        }
        case 1000:{
            coladdr = 0x180001fe;
            rowaddr = 0x00fffe00;
            bankaddr = 0x07000000;
            rankaddr = 0x20000000;
            break;
        }
        case 76:{
            coladdr = 0x38fe;
            rowaddr = 0x1fffc000;
            bankaddr = 0x700;
            rankaddr = 0x20000000;
            break;
        }
        case 77:{
            coladdr = 0x38fe;
            rowaddr = 0x3fff8000;
            bankaddr = 0x700;
            rankaddr = 0x4000;
            break;
        }
        case 78:{
            coladdr = 0x70fe;
            rowaddr = 0x3fff8000;
            bankaddr = 0x700;
            rankaddr = 0x800;
            break;
        }
        case 79:{
            coladdr = 0x3c7e;
            rowaddr = 0x1fffc000;
            bankaddr = 0x380;
            rankaddr = 0x20000000;
            break;
        }
        case 80:{
            coladdr = 0x3c7e;
            rowaddr = 0x3fff8000;
            bankaddr = 0x380;
            rankaddr = 0x4000;
            break;
        }
        case 81:{
            coladdr = 0x787e;
            rowaddr = 0x3fff8000;
            bankaddr = 0x380;
            rankaddr = 0x400;
            break;
        }
        case 82:{
            coladdr = 0x7fe;
            rowaddr = 0x3fffc000;
            bankaddr = 0x3800;
            rankaddr = 0x0;
            break;
        }
        case 83:{
            coladdr = 0x23fe;
            rowaddr = 0x3fffc000;
            bankaddr = 0x1c00;
            rankaddr = 0x0;
            break;
        }
        case 84:{
            coladdr = 0x31fe;
            rowaddr = 0x3fffc000;
            bankaddr = 0xe00;
            rankaddr = 0x0;
            break;
        }
        case 2000:{
            coladdr  = 0x300001fe;
            rowaddr  = 0x01fffe00;
            bankaddr = 0x0e000000;
            rankaddr = 0x0;
            break;
        }
        case 85:{
            coladdr = 0x38fe;
            rowaddr = 0x3fffc000;
            bankaddr = 0x700;
            rankaddr = 0x0;
            break;
        }
        case 86:{
            coladdr = 0x3c7e;
            rowaddr = 0x3fffc000;
            bankaddr = 0x380;
            rankaddr = 0x0;
            break;
        }
        case 87:{
            coladdr = 0x7fe;
            rowaddr = 0x1fffc000;
            bankaddr = 0x3800;
            rankaddr = 0x20000000;
            break;
        }
        case 88:{
            coladdr = 0x7fe;
            rowaddr = 0x3fff8000;
            bankaddr = 0x3800;
            rankaddr = 0x4000;
            break;
        }
        case 89:{
            coladdr = 0x23fe;
            rowaddr = 0x1fffc000;
            bankaddr = 0x1c00;
            rankaddr = 0x20000000;
            break;
        }
        case 90:{
            coladdr = 0x23fe;
            rowaddr = 0x3fff8000;
            bankaddr = 0x1c00;
            rankaddr = 0x4000;
            break;
        }
        case 91:{
            coladdr = 0x43fe;
            rowaddr = 0x3fff8000;
            bankaddr = 0x1c00;
            rankaddr = 0x2000;
            break;
        }
        case 92:{
            coladdr = 0x31fe;
            rowaddr = 0x1fffc000;
            bankaddr = 0xe00;
            rankaddr = 0x20000000;
            break;
        }
        case 93:{
            coladdr = 0x31fe;
            rowaddr = 0x3fff8000;
            bankaddr = 0xe00;
            rankaddr = 0x4000;
            break;
        }
        case 94:{
            coladdr = 0x61fe;
            rowaddr = 0x3fff8000;
            bankaddr = 0xe00;
            rankaddr = 0x1000;
            break;
        }
        case 95:{
            coladdr = 0x38fe;
            rowaddr = 0x1fffc000;
            bankaddr = 0x700;
            rankaddr = 0x20000000;
            break;
        }
        case 96:{
            coladdr = 0x38fe;
            rowaddr = 0x3fff8000;
            bankaddr = 0x700;
            rankaddr = 0x4000;
            break;
        }
        case 97:{
            coladdr = 0x70fe;
            rowaddr = 0x3fff8000;
            bankaddr = 0x700;
            rankaddr = 0x800;
            break;
        }
        case 98:{
            coladdr = 0x3c7e;
            rowaddr = 0x1fffc000;
            bankaddr = 0x380;
            rankaddr = 0x20000000;
            break;
        }
        case 99:{
            coladdr = 0x3c7e;
            rowaddr = 0x3fff8000;
            bankaddr = 0x380;
            rankaddr = 0x4000;
            break;
        }
        case 100:{
            coladdr = 0x787e;
            rowaddr = 0x3fff8000;
            bankaddr = 0x380;
            rankaddr = 0x400;
            break;
        }
        case 101:{
            coladdr = 0x7fe;
            rowaddr = 0x3fffc000;
            bankaddr = 0x3800;
            rankaddr = 0x0;
            break;
        }
        case 102:{
            coladdr = 0x23fe;
            rowaddr = 0x3fffc000;
            bankaddr = 0x1c00;
            rankaddr = 0x0;
            break;
        }
        case 103:{
            coladdr = 0x31fe;
            rowaddr = 0x3fffc000;
            bankaddr = 0xe00;
            rankaddr = 0x0;
            break;
        }
        case 104:{
            coladdr = 0x38fe;
            rowaddr = 0x3fffc000;
            bankaddr = 0x700;
            rankaddr = 0x0;
            break;
        }
        case 105:{
            coladdr = 0x3c7e;
            rowaddr = 0x3fffc000;
            bankaddr = 0x380;
            rankaddr = 0x0;
            break;
        }
        case 106:{
            coladdr = 0x7fe;
            rowaddr = 0x3fffc000;
            bankaddr = 0x3800;
            rankaddr = 0x40000000;
            break;
        }
        case 107:{
            coladdr = 0x7fe;
            rowaddr = 0x7fff8000;
            bankaddr = 0x3800;
            rankaddr = 0x4000;
            break;
        }
        case 108:{
            coladdr = 0x23fe;
            rowaddr = 0x3fffc000;
            bankaddr = 0x1c00;
            rankaddr = 0x40000000;
            break;
        }
        case 109:{
            coladdr = 0x23fe;
            rowaddr = 0x7fff8000;
            bankaddr = 0x1c00;
            rankaddr = 0x4000;
            break;
        }
        case 110:{
            coladdr = 0x43fe;
            rowaddr = 0x7fff8000;
            bankaddr = 0x1c00;
            rankaddr = 0x2000;
            break;
        }
        case 111:{
            coladdr = 0x31fe;
            rowaddr = 0x3fffc000;
            bankaddr = 0xe00;
            rankaddr = 0x40000000;
            break;
        }
        case 112:{
            coladdr = 0x31fe;
            rowaddr = 0x7fff8000;
            bankaddr = 0xe00;
            rankaddr = 0x4000;
            break;
        }
        case 113:{
            coladdr = 0x61fe;
            rowaddr = 0x7fff8000;
            bankaddr = 0xe00;
            rankaddr = 0x1000;
            break;
        }
        case 114:{
            coladdr = 0x38fe;
            rowaddr = 0x3fffc000;
            bankaddr = 0x700;
            rankaddr = 0x40000000;
            break;
        }
        case 115:{
            coladdr = 0x38fe;
            rowaddr = 0x7fff8000;
            bankaddr = 0x700;
            rankaddr = 0x4000;
            break;
        }
        case 116:{
            coladdr = 0x70fe;
            rowaddr = 0x7fff8000;
            bankaddr = 0x700;
            rankaddr = 0x800;
            break;
        }
        case 117:{
            coladdr = 0x3c7e;
            rowaddr = 0x3fffc000;
            bankaddr = 0x380;
            rankaddr = 0x40000000;
            break;
        }
        case 118:{
            coladdr = 0x3c7e;
            rowaddr = 0x7fff8000;
            bankaddr = 0x380;
            rankaddr = 0x4000;
            break;
        }
        case 119:{
            coladdr = 0x787e;
            rowaddr = 0x7fff8000;
            bankaddr = 0x380;
            rankaddr = 0x400;
            break;
        }
        case 120:{
            coladdr = 0x7fe;
            rowaddr = 0x3fffc000;
            bankaddr = 0x3800;
            rankaddr = 0x40000000;
            break;
        }
        case 121:{
            coladdr = 0x7fe;
            rowaddr = 0x7fff8000;
            bankaddr = 0x3800;
            rankaddr = 0x4000;
            break;
        }
        case 122:{
            coladdr = 0x23fe;
            rowaddr = 0x3fffc000;
            bankaddr = 0x1c00;
            rankaddr = 0x40000000;
            break;
        }
        case 123:{
            coladdr = 0x23fe;
            rowaddr = 0x7fff8000;
            bankaddr = 0x1c00;
            rankaddr = 0x4000;
            break;
        }
        case 124:{
            coladdr = 0x43fe;
            rowaddr = 0x7fff8000;
            bankaddr = 0x1c00;
            rankaddr = 0x2000;
            break;
        }
        case 125:{
            coladdr = 0x31fe;
            rowaddr = 0x3fffc000;
            bankaddr = 0xe00;
            rankaddr = 0x40000000;
            break;
        }
        case 126:{
            coladdr = 0x31fe;
            rowaddr = 0x7fff8000;
            bankaddr = 0xe00;
            rankaddr = 0x4000;
            break;
        }
        case 127:{
            coladdr = 0x61fe;
            rowaddr = 0x7fff8000;
            bankaddr = 0xe00;
            rankaddr = 0x1000;
            break;
        }
        case 128:{
            coladdr = 0x38fe;
            rowaddr = 0x3fffc000;
            bankaddr = 0x700;
            rankaddr = 0x40000000;
            break;
        }
        case 129:{
            coladdr = 0x38fe;
            rowaddr = 0x7fff8000;
            bankaddr = 0x700;
            rankaddr = 0x4000;
            break;
        }
        case 130:{
            coladdr = 0x70fe;
            rowaddr = 0x7fff8000;
            bankaddr = 0x700;
            rankaddr = 0x800;
            break;
        }
        case 131:{
            coladdr = 0x3c7e;
            rowaddr = 0x3fffc000;
            bankaddr = 0x380;
            rankaddr = 0x40000000;
            break;
        }
        case 132:{
            coladdr = 0x3c7e;
            rowaddr = 0x7fff8000;
            bankaddr = 0x380;
            rankaddr = 0x4000;
            break;
        }
        case 133:{
            coladdr = 0x787e;
            rowaddr = 0x7fff8000;
            bankaddr = 0x380;
            rankaddr = 0x400;
            break;
        }
    }
    
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_COLADDR_ADDRESS + mc_base_addr), coladdr);
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_ROWADDR_ADDRESS + mc_base_addr), rowaddr);
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_BANKADDR_ADDRESS + mc_base_addr), bankaddr);
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_RANKADDR_ADDRESS + mc_base_addr), rankaddr);
    hw_status("KAHALU_SIM_INIT: Wrote COLADDR [31:0] register with = 0x%0x; COLADDR [31:5] = 0x%0x\n", coladdr, (coladdr/32));
    hw_status("KAHALU_SIM_INIT: Wrote ROWADDR [31:0] register with = 0x%0x; ROWADDR [31:5] = 0x%0x\n", rowaddr, (rowaddr/32));
    hw_status("KAHALU_SIM_INIT: Wrote BANKADDR[31:0] register with = 0x%0x; BANKADDR[31:5] = 0x%0x\n", bankaddr, (bankaddr/32));
    hw_status("KAHALU_SIM_INIT: Wrote RANKADDR[31:0] register with = 0x%0x; RANKADDR[31:5] = 0x%0x\n", rankaddr, (rankaddr/32));
}

void pmb_enable_function(const int mc) {
    uint32_t rd_data = 0;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);

    rd_data = hw_read32((uint32_t*)(MCU_REGS_PMB_REGS_CONTROL_ADDRESS + mc_base_addr));
    rd_data = rd_data | 0x1;
    hw_write32((uint32_t*)(MCU_REGS_PMB_REGS_CONTROL_ADDRESS + mc_base_addr), rd_data);
}

void default_mtc_setup
(
    const int mc, 
    const int training_en,
    const int mov_window_size,
    const int mtc_adjust_step,
    const int active_ranks,
    const int mtc_use_read_methodB
) 
{
    uint32_t rd_wr_num;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);

    if (training_en) {

        // write training with a small window size 
        program_mtc_training_adj_step(mtc_adjust_step);
        hw_status("KAHALU_SIM_INIT: %s: Programming mov_window_size = %0d\n", __func__, mov_window_size);
        program_mtc_wr_training_window_size(mov_window_size);
        program_mtc_rd_training_window_size(mov_window_size);

        program_non_default_regs_for_mtc_init(training_en, active_ranks, mtc_use_read_methodB);
    }
}

void default_pmg_setup(const int mc, const int pmg_setup_en) 
{
    uint32_t rd_data = 0;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);

    hw_write32((uint32_t*)(MCU_REGS_PMG_REGS_PPD_THRESHOLD_ADDRESS + mc_base_addr), 50);

    hw_status("KAHALU_SIM_INIT: pmg_setup_en = %0d.\n", pmg_setup_en);
    rd_data = hw_read32((uint32_t *)(MCU_REGS_PMG_REGS_PHY_LP_CONFIG_ADDRESS + mc_base_addr));
    if (pmg_setup_en < 2)
    {
        hw_status("KAHALU_SIM_INIT: ***CAUTION*** Disabling PHY LP; to be enabled in future.\n");
        rd_data &= (~PMG_REGS_PHY_LP_CONFIG_PD_FIELD_MASK);
    }
    rd_data &= (~PMG_REGS_PHY_LP_CONFIG_PD_WAKEUP_FIELD_MASK);
    rd_data |= PMG_REGS_PHY_LP_CONFIG_PD_WAKEUP_SET(0x5);
    hw_write32((uint32_t *)(MCU_REGS_PMG_REGS_PHY_LP_CONFIG_ADDRESS + mc_base_addr), rd_data);

    hw_write32((uint32_t *)(MCU_REGS_PMG_REGS_PHYLP_PPD_THRESHOLD_ADDRESS + mc_base_addr), 5);

    /*
    rd_data = hw_read32((uint32_t *)MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR4_ADDRESS);
    rd_data &= (~PHY_REGS_DWC_DDRPHY_PUB_PGCR4_LPWAKEUP_THRSH_FIELD_MASK);
    rd_data |= PHY_REGS_DWC_DDRPHY_PUB_PGCR4_LPWAKEUP_THRSH_SET(0x5);
    hw_write32((uint32_t *)MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_PGCR4_ADDRESS, rd_data);
    */

    //Using a typical value of 256 for simulations;
    //FIXME: Need to change this for emu and silicon
    hw_write32((uint32_t *)(MCU_REGS_PMG_REGS_TLP_RESP_ADDRESS + mc_base_addr), 0x100);

    if (pmg_setup_en)
    {
        hw_status("KAHALU_SIM_INIT: Enabling PMG\n");
        hw_write32((uint32_t *)(MCU_REGS_PMG_REGS_PMG_ENABLE_ADDRESS + mc_base_addr), 0x1);
    }
    else
    {
        hw_status("KAHALU_SIM_INIT: Disabling PMG\n");
        hw_write32((uint32_t *)(MCU_REGS_PMG_REGS_PMG_ENABLE_ADDRESS + mc_base_addr), 0x0);
    }
}

void power_down_rank(const int mc, const uint32_t rank) {
    uint32_t wr_data;
    uint32_t rd_data;
    uint32_t cmd_status;
    uint32_t timeout;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);

    wr_data = (rank<<3) | //Rank
              (0);     //Bank 0
    hw_write32((uint32_t *)(MCU_REGS_CRB_HIER_REGS_ADDRESS_ADDRESS + mc_base_addr), wr_data);

    wr_data = (7 << 8) | //Command == PDE
              (0);       //SubCommand == 0
    hw_write32((uint32_t *)(MCU_REGS_CRB_HIER_REGS_COMMAND_ADDRESS + mc_base_addr), wr_data);

    //Wait for CMD_STATUS busy to be de-asserted
    timeout = 0;
    do
    {
        cmd_status = hw_read32((uint32_t *)(MCU_REGS_CRB_HIER_REGS_CMD_STATUS_ADDRESS + mc_base_addr));
        cmd_status = cmd_status & CRB_HIER_REGS_CMD_STATUS_BUSY_FIELD_MASK;
        timeout++;
    } while (cmd_status > 0 && timeout < 100);
    if (timeout >= 100)
    {
        hw_status("KAHALU_SIM_INIT: ERROR: Timeout waiting of CMD_STATUS_BUSY to get de-asserted in %s\n", __func__);
        return;
    }

    //Make sure the command status doesn't indicate error.
    cmd_status = hw_read32((uint32_t *)(MCU_REGS_CRB_HIER_REGS_CMD_STATUS_ADDRESS + mc_base_addr));
    cmd_status = cmd_status & CRB_HIER_REGS_CMD_STATUS_CMD_ERROR_FIELD_MASK;
    if (cmd_status)
    {
        hw_status("KAHALU_SIM_INIT: ERROR: CMD_STATUS_CMD_ERROR field got asserted in %s\n", __func__);
    } else {
        hw_status("KAHALU_SIM_INIT: Successfully powered-down Rank%0d\n", rank);
    }
}

void mc_setup(const int mc, const int mode, const int active_ranks, const int dram_cfg) {
    uint32_t rd_data = 0;
    uint32_t i;
    uint32_t sch_ref_align_ctrl;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);
    
    if(mode == -1){
        return;
    } else if(mode == 1) {//LP4

        //Firstly make sure the Global Hold is set; it will be set by default;
        //hence no need to write for cold-boot; for warm-boot it will be
        //required. FIXME: TODO

        //Disable Auto-Refresh
        hw_status("KAHALU_SIM_INIT: Disabling Auto-Refresh\n");
        set_autorefresh(mc, 0, mode, 1600);

        //Disable Power-Mgmt; by default it is disabled; hence no need to
        //write for cold-boot; for warm-boot it will be required.
        hw_status("KAHALU_SIM_INIT: Disabling Power-Mgmt\n");
        hw_write32((uint32_t*)(MCU_REGS_PMG_REGS_PMG_ENABLE_ADDRESS + mc_base_addr), 0x0);

        hw_write32((uint32_t*)(MCU_REGS_DDC_REGS_TRAINING_ENABLE_ADDRESS + mc_base_addr), 0x0);

        hw_status("KAHALU_SIM_INIT: Basic MC Setup\n");
        hw_write32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_DRAM_DATA_WIDTH_ADDRESS + mc_base_addr), (uint32_t)CRB_HIER_REGS_DRAM_DATA_WIDTH_DATA_WIDTH_FIELD_MASK & 1);                                
        //set dram type to LP4
        hw_write32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_DRAM_TYPE_ADDRESS + mc_base_addr), (uint32_t)CRB_HIER_REGS_DRAM_TYPE_WRITE_MASK & 1);
        
        //set default CS and CA to lpddr4    
        rd_data = hw_read32((uint32_t*)(MCU_REGS_DDC_REGS_CONFIG_ADDRESS + mc_base_addr));
        rd_data &= ~((uint32_t)DDC_REGS_CONFIG_DEFAULT_CS_FIELD_MASK);
        rd_data &= ~((uint32_t)DDC_REGS_CONFIG_DEFAULT_CA_FIELD_MASK);
        hw_write32((uint32_t*)(MCU_REGS_DDC_REGS_CONFIG_ADDRESS + mc_base_addr), rd_data);

        //set pmg count to defualt value of 0x3000
        hw_write32((uint32_t*)(MCU_REGS_PMG_REGS_PPD_THRESHOLD_ADDRESS + mc_base_addr), (uint32_t)(0x3000)); 

        //set clock ratio (SCH : DDC) t- 1:2 for lpddr4
        hw_write32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_DFI_FREQ_RATIO_ADDRESS + mc_base_addr), (uint32_t)CRB_HIER_REGS_DFI_FREQ_RATIO_WRITE_MASK & 1);             

        //set up default address mapping 
        hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_ROWADDR_ADDRESS + mc_base_addr), (uint32_t)(0x3fffc000)); 
        hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_RANKADDR_ADDRESS + mc_base_addr),(uint32_t)(0x40000000)); 
        hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_BANKADDR_ADDRESS + mc_base_addr),(uint32_t)(0x00003800)); 
        hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_COLADDR_ADDRESS + mc_base_addr), (uint32_t)(0x000007e0)); 

        if (active_ranks == 1) {
            configure_dram(mc, dram_cfg);
            hw_write32((uint32_t*)(MCU_REGS_SCH_REGS_SCH_REF_ALIGN_CTRL_ADDRESS + mc_base_addr), 0x0); 

            //Set SCH_REF_RATE_CTRL.type to 0
            rd_data = hw_read32((uint32_t*)(MCU_REGS_SCH_REGS_SCH_REF_RATE_CTRL_ADDRESS + mc_base_addr));
            rd_data &= ~((uint32_t)SCH_REG_SCH_REF_RATE_CTRL_TYPE_FIELD_MASK);
            hw_write32((uint32_t*)(MCU_REGS_SCH_REGS_SCH_REF_RATE_CTRL_ADDRESS + mc_base_addr), rd_data);
        } else {
            sch_ref_align_ctrl = (rand() % 2) + 2;
            hw_write32((uint32_t*)(MCU_REGS_SCH_REGS_SCH_REF_ALIGN_CTRL_ADDRESS + mc_base_addr), sch_ref_align_ctrl); 
        }

        //Setup active ranks
        hw_write32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_ACTIVE_RANKS_ADDRESS + mc_base_addr), (uint32_t)active_ranks); 

    }
}

void init_dram(const int mc, const int mode, const int active_ranks, const int dram_cfg){
    uint32_t rd_data = 0;
    uint32_t i;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);
    
    mc_setup(mc, mode, active_ranks, dram_cfg);

    //Wait 20ns  
    hw_sleep(20);

    //Set DFI reset to low, resetting DRAM
    rd_data = hw_read32((uint32_t*)(MCU_REGS_DDC_REGS_CONFIG_ADDRESS + mc_base_addr)); 
    hw_write32((uint32_t*)(MCU_REGS_DDC_REGS_CONFIG_ADDRESS + mc_base_addr), (uint32_t) (rd_data |(DDC_REGS_CONFIG_DFI_RESET_N_SET(0)))); 

    //Wait 1500ns (tINIT1 period) 
    hw_sleep(1500);
    
    //Set DFI reset to high, releasing DRAM from reset
    rd_data = hw_read32((uint32_t*)(MCU_REGS_DDC_REGS_CONFIG_ADDRESS + mc_base_addr)); 
    hw_write32((uint32_t*)(MCU_REGS_DDC_REGS_CONFIG_ADDRESS + mc_base_addr), (uint32_t) (rd_data |(DDC_REGS_CONFIG_DFI_RESET_N_SET(1)))); 

    hw_sleep(1500);
    //Wait 1500ns  (tINIT3 period)
    simulation_tb_wait(100, "fab");
    
    //Activate CKE
    rd_data = hw_read32((uint32_t*)(MCU_REGS_DDC_REGS_CONFIG_ADDRESS + mc_base_addr));
        
    rd_data |= (uint32_t)DDC_REGS_CONFIG_DEFAULT_CKE_ACTIVATE_FIELD_MASK;

#ifndef CONF_HAS___PHY_RTL
    //FING enable enable_phy_initated_update 
    rd_data |= (uint32_t)DDC_REGS_CONFIG_ENABLE_PHY_INITIATED_UPDATE_FIELD_MASK;
#endif
            
    hw_write32((uint32_t*)(MCU_REGS_DDC_REGS_CONFIG_ADDRESS + mc_base_addr), rd_data);

    //init the DISABLE_TREFI_CHECK bit
    rd_data = hw_read32((uint32_t*)(MCU_REGS_DDC_REGS_CONFIG_EXT_ADDRESS + mc_base_addr));
    rd_data |= (uint32_t)DDC_REGS_CONFIG_EXT_DISABLE_TREFI_CHECK_FIELD_MASK;
    hw_write32((uint32_t*)(MCU_REGS_DDC_REGS_CONFIG_EXT_ADDRESS + mc_base_addr), rd_data);
}

void write_mr_reg(const int mc, uint32_t mr_addr, uint32_t rank, uint32_t data){
    uint32_t timeout = 0;
    uint32_t rd_data = 0;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);
    
    hw_status("KAHALU_SIM_INIT: Sending MRW with ADDRESS= %0d, DATA= %0x\n", mr_addr, data);
    rd_data = hw_read32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_ADDRESS_ADDRESS + mc_base_addr));
    rd_data = rd_data & ~(1<<3);
        
    hw_write32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_ADDRESS_ADDRESS + mc_base_addr), rd_data | rank<<3);
    
    //write data into crb_hier_regs.WR_DATA
    hw_write32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_WR_DATA_ADDRESS + mc_base_addr), data);
    
    //write mr_addr into crb_hier_regs.MODE_REG_ADDR   
    hw_write32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_MODE_REG_ADDR_ADDRESS + mc_base_addr), mr_addr);
    
    //Issue MRW command to CRB
    hw_write32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_COMMAND_ADDRESS + mc_base_addr), MCU_DDC_CMD_MRW<<8);
    
    //Wait for command done
    while(hw_read32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_CMD_STATUS_ADDRESS + mc_base_addr)) & ((uint32_t)CRB_HIER_REGS_CMD_STATUS_BUSY_FIELD_MASK)){
        hw_status("KAHALU_SIM_INIT: timeout = %0d, Waiting for MRW Command done", timeout);
        timeout++;
        if(timeout>100){
            hw_status("KAHALU_SIM_INIT: ERROR: MRW command timed out.  Busy signal high for %0d CMD_STATUS reads\n", timeout);
            break;
        }
    }
}

uint32_t read_mr_reg(const int mc, uint32_t mr_addr, uint32_t rank){
    uint32_t rdata = 0;
    uint32_t timeout = 0;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);
    
    hw_status("KAHALU_SIM_INIT: Sending MRR with ADDRESS= %0d\n", mr_addr);
    
    rdata = hw_read32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_ADDRESS_ADDRESS + mc_base_addr));
    rdata = rdata & ~(1<<3);      
    hw_write32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_ADDRESS_ADDRESS + mc_base_addr), rdata | rank<<3);
    
    //write mr_addr into crb_hier_regs.MODE_REG_ADDR   
    hw_write32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_MODE_REG_ADDR_ADDRESS + mc_base_addr), mr_addr);
    
    //Issue MRR command to CRB
    hw_write32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_COMMAND_ADDRESS + mc_base_addr), MCU_DDC_CMD_MRR<<8);
    
    //Wait for command done
    while(hw_read32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_CMD_STATUS_ADDRESS + mc_base_addr)) & ((uint32_t)CRB_HIER_REGS_CMD_STATUS_BUSY_FIELD_MASK)){
        timeout++;
        if(timeout>100){
            hw_status("KAHALU_SIM_INIT: ERROR: MRR command timed out.  Busy signal high for %0d CMD_STATUS reads\n", timeout);
            break;
        }
    }
    
    //Return data from crb_hier_regs.RD_DATA    
    return hw_read32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_RD_DATA_ADDRESS + mc_base_addr));
}

void wait_for_dfi_init(const int mc){
    uint32_t rd_data;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);
    
    //wait for phy/dram init
    hw_status("KAHALU_SIM_INIT: Waiting for DFI_INIT_COMPLETE\n");

    rd_data = 0;
    do{      
        rd_data = hw_read32((uint32_t*)(MCU_REGS_CRB_HIER_REGS_DFI_INIT_COMPLETE_ADDRESS + mc_base_addr));            
    }while(rd_data != 1);

    hw_status("KAHALU_SIM_INIT: Saw DFI_INIT_COMPLETE\n");
}

int assert_global_hold_seq(const int mc, const int cycles, char* clk){

    uint32_t rd_data = 0;
    uint32_t wr_data = 0;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);
    
    if(cycles != -1) {
    
        simulation_tb_wait(cycles, clk);
        
        rd_data = hw_read32((uint32_t*)(MCU_REGS_MCP_REGS_CONTROL_ADDRESS + mc_base_addr)); 
        
        //set the hold field
        wr_data = rd_data | ((uint32_t)MCP_REGS_CONTROL_GLOBAL_HOLD_FIELD_MASK);    

        hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_CONTROL_ADDRESS + mc_base_addr), wr_data);

    } else {
        hw_status("KAHALU_SIM_INIT: NOT asserting global hold\n");
    }
    return 0;
}

int release_global_hold_seq(const int mc, const int cycles, char* clk){

    uint32_t rd_data = 0;
    uint32_t wr_data = 0;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);
    
    if(cycles != -1) {
    
        simulation_tb_wait(cycles, clk);
        
        rd_data = hw_read32((uint32_t*)(MCU_REGS_MCP_REGS_CONTROL_ADDRESS + mc_base_addr)); 
        
        //zero out the hold field
        wr_data = rd_data & ~((uint32_t)MCP_REGS_CONTROL_GLOBAL_HOLD_FIELD_MASK);    

        hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_CONTROL_ADDRESS + mc_base_addr), wr_data);

    } else {
        hw_status("KAHALU_SIM_INIT: NOT releasing global hold\n");
    }
    return 0;
}

void send_mr_commands(const int mc, const int dram_mode, const int active_ranks, const int wr_dbi_dis, const int rd_dbi_dis, const int ddc_mode, const int frequency){
    uint32_t wl_set = 0;
    uint32_t wl_val = 0;
    uint32_t rl_val = 0;
    uint32_t rtp_val = 0;
    uint32_t wr_data = 0;
    uint32_t rd_data = 0;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);

    if(dram_mode == 1){//LP4
        //MR1
        //OP[1:0] BL 16 or 32 Sequential (on-the-fly)
        wr_data = 2;
        
        //OP[2] 1B: WR Pre-amble = 2*tCK
        wr_data |= (1<<2);
        
        //OP[3] 0B: RD Pre-amble = Static (default)
        wr_data |= (0<<3);
        
        //OP[6:4] nWR:
        //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.wr = %d\n",lp4_param_db.wr[DDC_REG]);
        //FREQ VARIATION -- WR needs to be MANUALLY edit in database so the switch here
        switch(lp4_param_db.wr[DDC_MICRON]){
            case 10: wr_data |= (1<<4); break;
            case 16: wr_data |= (2<<4); break;
            case 20: wr_data |= (3<<4); break;
            case 24: wr_data |= (4<<4); break;
            case 30: wr_data |= (5<<4); break;
            case 34: wr_data |= (6<<4); break;
            case 40: wr_data |= (7<<4); break;
            default: wr_data |= (0<<4); break;
       }
           
       write_mr_reg(mc, 1, 0, wr_data);
       if (active_ranks == 3) {
           write_mr_reg(mc, 1, 1, wr_data);
       }

#ifdef CONF_HAS___PHY_RTL
       hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR1_ADDRESS + mc_base_addr), wr_data); 	//0x00000056 nWR=30, wr-preable =2 nclk, BL=2'b10
#endif    
 
       
        //MR2
        //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.wl = %d\n",lp4_param_db.wl[DDC_REG]);
        wr_data = 0;
        wl_val = lp4_param_db.wl[DDC_MICRON];
        
       if(ddc_mode == DDC_MICRON){
            wl_val = lp4_param_db.wl[DDC_TYP];
       }
        
        
        //FREQ VARIATION -- WL needs to be MANUALLY edit in database so the switch here
        if((wl_val == 6) || (wl_val == 8) || (wl_val == 10) || (wl_val == 12) || (wl_val == 14) || (wl_val == 16) || (wl_val == 18))
        {
            wl_set = 0;
            switch(wl_val){
                case 6: wr_data |= (1<<3); break;           
                case 8: wr_data |= (2<<3); break;
                case 10: wr_data |= (3<<3); break;
                case 12: wr_data |= (4<<3); break;
                case 14: wr_data |= (5<<3); break;
                case 16: wr_data |= (6<<3); break;
                case 18: wr_data |= (7<<3); break;
            }
        }else if((wl_val == 8) || (wl_val == 12) || (wl_val == 18) || (wl_val == 22) || (wl_val == 26) || (wl_val == 30) || (wl_val == 34)){
            wl_set = 1;
            switch(wl_val){
                case 8: wr_data |= (1<<3); break;            
                case 12: wr_data |= (2<<3); break;
                case 18: wr_data |= (3<<3); break;
                case 22: wr_data |= (4<<3); break;
                case 26: wr_data |= (5<<3); break;
                case 30: wr_data |= (6<<3); break;
                case 34: wr_data |= (7<<3); break;
            }
        }else{
            wl_set = 0;
            wr_data |= (0<<3);
        }
        wr_data |= (wl_set <<6);
        

       //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.rl = %d\n",lp4_param_db.rl[DDC_REG]);
       //please don't delete. Needed for SoC porting - hw_status("lp4_param_db.rtp = %d\n",lp4_param_db.rtp[DDC_REG]);
        
       rl_val = lp4_param_db.rl[DDC_MICRON];
       rtp_val = lp4_param_db.rtp[DDC_MICRON];

        
        //FREQ VARIATION -- RL/RTP needs to be MANUALLY edit in database so the switch here
        if((rl_val == 6) && (rtp_val == 8)){
            wr_data |= 0;
        }else if(((rl_val == 10)||(rl_val == 12)) && (rtp_val == 8)){
            wr_data |= 1;
        }else if(((rl_val == 14)||(rl_val == 16)) && (rtp_val == 8)){
            wr_data |= 2;
        }else if(((rl_val == 20)||(rl_val == 22)) && (rtp_val == 8)){
            wr_data |= 3;
        }else if(((rl_val == 24)||(rl_val == 28)) && (rtp_val == 10)){
            wr_data |= 4;
        }else if(((rl_val == 28)||(rl_val == 32)) && (rtp_val == 12)){
            wr_data |= 5;
        }else if(((rl_val == 32) ||(rl_val == 36)) && (rtp_val == 14)){
            wr_data |= 6;
        }else if(((rl_val == 36)||(rl_val == 40)) && (rtp_val == 16)){
            wr_data |= 7;
        }else{
            wr_data |= 0;
        }
            
        write_mr_reg(mc, 2, 0, wr_data);  
        if (active_ranks == 3) {
            write_mr_reg(mc, 2, 1, wr_data);    
        }

#ifdef CONF_HAS___PHY_RTL
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR2_ADDRESS + mc_base_addr), wr_data);
#endif    

        //write MR3 to enable rd and wr dbi except when dbi is disabled by test 
        if( (wr_dbi_dis ==1) && (rd_dbi_dis == 1) )
          wr_data = 0x31;       //00110001  wr_dbi + rd_dbi cleared
        else if(wr_dbi_dis) 
          wr_data = 0x71;	//01110001  wr_dbi cleared 
        else if(rd_dbi_dis) 
          wr_data = 0xb1;	//10110001  rd_dbi cleared 
        else
          wr_data = 0xf1;	//enable  both 

        write_mr_reg(mc, 3, 0, wr_data);
        if (active_ranks == 3) {
            write_mr_reg(mc, 3, 1, wr_data);
        }
#ifdef CONF_HAS___PHY_RTL
        hw_phy_write32((uint32_t*)(MCU_REGS_PHY_REGS_DWC_DDRPHY_PUB_MR3_ADDRESS + mc_base_addr), wr_data);
#endif    


       //setting up MR11 DQ ODT[2:0] and CA ODT[6:4]
       if(frequency != 400){
           wr_data = 0x44;
           write_mr_reg(mc, 11, 0, wr_data);
            if (active_ranks == 3) {
                write_mr_reg(mc, 11, 1, wr_data);
            }
        }
    }else{//LP3
        //mr[1] <= 8'h63;   // 1 0x81 Device feature W nWR (for AP) BL
        write_mr_reg(mc, 1, 0, 0x43);
        if (active_ranks == 3) {
            write_mr_reg(mc, 1, 1, 0x43);
        }
                
        //mr[2] <= 8'h1b;   // 2 0x02 Device feature W RFU RL & WL for 800Mhz, RL=12, WL=6
        write_mr_reg(mc, 2, 0, 0x1a);
        if (active_ranks == 3) {
            write_mr_reg(mc, 2, 1, 0x1a);
        }
    }
}


#ifdef CONF_HAS___PHY_RTL
int mcu_reset_init_seq
     (
         const int mc,
         const int dram_mode, 
         const int active_ranks, 
         const int dram_cfg, 
         const int autoref_int, 
         const int ddc_param_mode, 
         const int wr_dbi_dis, 
         const int rd_dbi_dis, 
         const int dram_frequency,
         const int enable_pmb,
         const int skip_part_phy_init
     )
{
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);
    int ret;
    //FIXME: TODO: init_dram does activate CKE in DDC config
    //Make sure if this is ok or not.
    init_dram(mc, dram_mode, active_ranks, dram_cfg);

    //PHY Initialization at low frequency and then switch to operating
    //frequency.
    ret = phy_real_init_synps_lp4(mc, ddc_param_mode, active_ranks, dram_frequency, wr_dbi_dis, rd_dbi_dis, skip_part_phy_init);
    if(ret!=0) {
       return ret;
    }  

    wait_for_dfi_init(mc);
    
    update_ddc_fields(mc, dram_frequency);
    
    set_autorefresh(mc, autoref_int, dram_mode, dram_frequency);

    hw_status("KAHALU_SIM_INIT: send_mr_commands\n");
#ifdef MCU_IP_VERIF
        send_mr_commands(mc, dram_mode, active_ranks, wr_dbi_dis, rd_dbi_dis,ddc_param_mode, dram_frequency);
#else
        send_mr_commands(mc, dram_mode, active_ranks, wr_dbi_dis, rd_dbi_dis,DDC_MICRON, dram_frequency);
#endif //MCU_IP_VERIF    
    
    if(enable_pmb) {
        hw_status("KAHALU_SIM_INIT: Enabling PMB\n");
        pmb_enable_function(mc);   
    }

    perf_regs_init(mc);

    do_yml_reg_writes("rand_reg_data.bin");   
    
    do_yml_reg_writes("reg_data.bin"); 

    configure_dram(mc, dram_cfg);
    
    set_autorefresh(mc, autoref_int, dram_mode, dram_frequency);
    
    return 0;
}

#endif    

//
// Function to program the PERF related registers with values from
// makena_1c.reg perf replay test register file.
// These settings will be used by most of the tests (functional, perf replay,
// perf non-replay tests).  However some of the functional tests will
// randomize these registers for covering other possibilities.  Also the
// perf replay and perf non-replay tests will override the target BW
// registers as needed for those tests.
// NOTE: The TargetBW registers may need to be adjusted for the silicon
// validation and the final product.
//
void perf_regs_init(const int mc) 
{
    uint32_t rd_data;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);

    //config.col_has_higher_priority : 0
    rd_data = hw_read32((uint32_t*)(MCU_REGS_DDC_REGS_CONFIG_ADDRESS + mc_base_addr));
    rd_data = rd_data & (~DDC_REGS_CONFIG_COL_HAS_HIGHER_PRIORITY_FIELD_MASK);
    hw_write32((uint32_t*)(MCU_REGS_DDC_REGS_CONFIG_ADDRESS + mc_base_addr), rd_data);

    //qos_config.hardenab : 127
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_QOS_CONFIG_ADDRESS + mc_base_addr), 0x7f);

    //qoswrlazy.timevalue : 54
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_QOSWRLAZY_ADDRESS + mc_base_addr), 54);

    //qosrdlazy.timevalue : 4
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_QOSRDLAZY_ADDRESS + mc_base_addr), 4);

    //nqoswrlazy.timevalue : 54
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_NQOSWRLAZY_ADDRESS + mc_base_addr), 54);

    //nqosrdlazy.timevalue : 4
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_NQOSRDLAZY_ADDRESS + mc_base_addr), 4);

    //c0agetimeout.timevalue : 40
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C0AGETIMEOUT_ADDRESS + mc_base_addr), 40);

    //c1agetimeout.timevalue : 30
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C1AGETIMEOUT_ADDRESS + mc_base_addr), 30);

    //c2agetimeout.timevalue : 40
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C2AGETIMEOUT_ADDRESS + mc_base_addr), 40);

    //c3agetimeout.timevalue : 30
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C3AGETIMEOUT_ADDRESS + mc_base_addr), 30);

    //c4agetimeout.timevalue : 30
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C4AGETIMEOUT_ADDRESS + mc_base_addr), 30);

    //c5agetimeout.timevalue : 30
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C5AGETIMEOUT_ADDRESS + mc_base_addr), 30);

    //c6agetimeout.timevalue : 40
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C6AGETIMEOUT_ADDRESS + mc_base_addr), 40);

    //c7agetimeout.timevalue : 32
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C7AGETIMEOUT_ADDRESS + mc_base_addr), 32);

    //c8agetimeout.timevalue : 32
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C8AGETIMEOUT_ADDRESS + mc_base_addr), 32);

    //c9agetimeout.timevalue : 32
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C9AGETIMEOUT_ADDRESS + mc_base_addr), 32);

    //c10agetimeout.timevalue : 32
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C10AGETIMEOUT_ADDRESS + mc_base_addr), 32);

    //c11agetimeout.timevalue : 32
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C11AGETIMEOUT_ADDRESS + mc_base_addr), 32);

    //maxbw.cred : 64000
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_MAXBW_ADDRESS + mc_base_addr), 64000);

    //minbw.cred : 63999
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_MINBW_ADDRESS + mc_base_addr), 63999);

    //c0targwbw.rate : 961
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C0TARGWBW_ADDRESS + mc_base_addr), 961);

    //c0urgwbw.thresh : 500
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C0URGWBW_ADDRESS + mc_base_addr), 500);

    //c0fatalwbw.thresh : 1000
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C0FATALWBW_ADDRESS + mc_base_addr), 1000);

    //c0emptysatwbw.thresh : 800
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C0EMPTYSATWBW_ADDRESS + mc_base_addr), 800);

    //c1targwbw.rate : 897
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C1TARGWBW_ADDRESS + mc_base_addr), 897);

    //c1urgwbw.thresh : 500
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C1URGWBW_ADDRESS + mc_base_addr), 500);

    //c1fatalwbw.thresh : 1000
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C1FATALWBW_ADDRESS + mc_base_addr), 1000);

    //c1emptysatwbw.thresh : 800
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C1EMPTYSATWBW_ADDRESS + mc_base_addr), 800);

    //c2targwbw.rate : 641
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C2TARGWBW_ADDRESS + mc_base_addr), 641);

    //c2urgwbw.thresh : 500
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C2URGWBW_ADDRESS + mc_base_addr), 500);

    //c2fatalwbw.thresh : 1000
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C2FATALWBW_ADDRESS + mc_base_addr), 1000);

    //c2emptysatwbw.thresh : 800
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C2EMPTYSATWBW_ADDRESS + mc_base_addr), 800);

    //c3targwbw.rate : 1179
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C3TARGWBW_ADDRESS + mc_base_addr), 1179);

    //c3urgwbw.thresh : 500
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C3URGWBW_ADDRESS + mc_base_addr), 500);

    //c3fatalwbw.thresh : 1000
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C3FATALWBW_ADDRESS + mc_base_addr), 1000);

    //c3emptysatwbw.thresh : 800
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C3EMPTYSATWBW_ADDRESS + mc_base_addr), 800);

    //c6targwbw.rate : 21
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C6TARGWBW_ADDRESS + mc_base_addr), 21);

    //c6urgwbw.thresh : 500
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C6URGWBW_ADDRESS + mc_base_addr), 500);

    //c6fatalwbw.thresh : 1000
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C6FATALWBW_ADDRESS + mc_base_addr), 1000);

    //c6emptysatwbw.thresh : 800
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C6EMPTYSATWBW_ADDRESS + mc_base_addr), 800);

    //c4targrbw.rate : 833
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C4TARGRBW_ADDRESS + mc_base_addr), 833);

    //c4urgrbw.thresh : 500
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C4URGRBW_ADDRESS + mc_base_addr), 500);

    //c4fatalrbw.thresh : 1000
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C4FATALRBW_ADDRESS + mc_base_addr), 1000);

    //c4emptysatrbw.thresh : 800
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C4EMPTYSATRBW_ADDRESS + mc_base_addr), 800);

    //c5targrbw.rate : 833
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C5TARGRBW_ADDRESS + mc_base_addr), 833);

    //c5urgrbw.thresh : 500
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C5URGRBW_ADDRESS + mc_base_addr), 500);

    //c5fatalrbw.thresh : 1000
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C5FATALRBW_ADDRESS + mc_base_addr), 1000);

    //c5emptysatrbw.thresh : 800
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C5EMPTYSATRBW_ADDRESS + mc_base_addr), 800);

    //c6targrbw.rate : 8
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C6TARGRBW_ADDRESS + mc_base_addr), 8);

    //c6urgrbw.thresh : 500
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C6URGRBW_ADDRESS + mc_base_addr), 500);

    //c6fatalrbw.thresh : 1000
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C6FATALRBW_ADDRESS + mc_base_addr), 1000);

    //c6emptysatrbw.thresh : 800
    hw_write32((uint32_t*)(MCU_REGS_MCP_REGS_C6EMPTYSATRBW_ADDRESS + mc_base_addr), 800);

    //sch_ctrl.trcd_check_en : 1
    //sch_ctrl.rr_limt : 3
    rd_data = hw_read32((uint32_t*)(MCU_REGS_SCH_REGS_SCH_CTRL_ADDRESS + mc_base_addr));
    rd_data = (rd_data & (~SCH_REG_SCH_CTRL_TRCD_CHECK_EN_FIELD_MASK)) | 
              SCH_REG_SCH_CTRL_TRCD_CHECK_EN_SET(1);
    rd_data = (rd_data & (~SCH_REG_SCH_CTRL_RR_LIMT_FIELD_MASK)) | 
              SCH_REG_SCH_CTRL_RR_LIMT_SET(3);
    hw_write32((uint32_t*)(MCU_REGS_SCH_REGS_SCH_CTRL_ADDRESS + mc_base_addr), rd_data);

    rd_data = hw_read32((uint32_t*)(MCU_REGS_SCH_REGS_SCH_STREAK_ADDRESS + mc_base_addr));
    //sch_streak.rd_min : 1
    rd_data = (rd_data & (~SCH_REG_SCH_STREAK_RD_MIN_FIELD_MASK)) | 
              SCH_REG_SCH_STREAK_RD_MIN_SET(1);
    //sch_streak.rd_max : 8
    rd_data = (rd_data & (~SCH_REG_SCH_STREAK_RD_MAX_FIELD_MASK)) | 
              SCH_REG_SCH_STREAK_RD_MAX_SET(8);
    //sch_streak.wr_min : 1
    rd_data = (rd_data & (~SCH_REG_SCH_STREAK_WR_MIN_FIELD_MASK)) | 
              SCH_REG_SCH_STREAK_WR_MIN_SET(1);
    //sch_streak.wr_max : 4
    rd_data = (rd_data & (~SCH_REG_SCH_STREAK_WR_MAX_FIELD_MASK)) | 
              SCH_REG_SCH_STREAK_WR_MAX_SET(4);
    //sch_streak.rank_min : 2
    rd_data = (rd_data & (~SCH_REG_SCH_STREAK_RANK_MIN_FIELD_MASK)) | 
              SCH_REG_SCH_STREAK_RANK_MIN_SET(2);
    //sch_streak.rank_max : 8
    rd_data = (rd_data & (~SCH_REG_SCH_STREAK_RANK_MAX_FIELD_MASK)) | 
              SCH_REG_SCH_STREAK_RANK_MAX_SET(8);
    hw_write32((uint32_t*)(MCU_REGS_SCH_REGS_SCH_STREAK_ADDRESS + mc_base_addr), rd_data);

    rd_data = hw_read32((uint32_t*)(MCU_REGS_SCH_REGS_SCH_PAGE_STREAK_ADDRESS + mc_base_addr));
    //sch_page_streak.qos_min : 4
    rd_data = (rd_data & (~SCH_REG_SCH_PAGE_STREAK_QOS_MIN_FIELD_MASK)) | 
              SCH_REG_SCH_PAGE_STREAK_QOS_MIN_SET(4);
    //sch_page_streak.qos_max : 16
    rd_data = (rd_data & (~SCH_REG_SCH_PAGE_STREAK_QOS_MAX_FIELD_MASK)) | 
              SCH_REG_SCH_PAGE_STREAK_QOS_MAX_SET(16);
    //sch_page_streak.nonqos_min : 1
    rd_data = (rd_data & (~SCH_REG_SCH_PAGE_STREAK_NONQOS_MIN_FIELD_MASK)) | 
              SCH_REG_SCH_PAGE_STREAK_NONQOS_MIN_SET(1);
    //sch_page_streak.nonqos_max : 16
    rd_data = (rd_data & (~SCH_REG_SCH_PAGE_STREAK_NONQOS_MAX_FIELD_MASK)) | 
              SCH_REG_SCH_PAGE_STREAK_NONQOS_MAX_SET(16);
    hw_write32((uint32_t*)(MCU_REGS_SCH_REGS_SCH_PAGE_STREAK_ADDRESS + mc_base_addr), rd_data);
} //perf_regs_init()

//----------------------------------------------------------//
// Entry: main function
//----------------------------------------------------------//
int simulation_init_seq
    (
        const int mc,
        const int dram_mode, 
        const int active_ranks, 
        const int dram_cfg, 
        const int autoref_int, 
        const int training_test_num, 
        const int phy_training_mode, 
        const int ddc_param_mode, 
        const int wr_dbi_dis, 
        const int rd_dbi_dis, 
        const int use_real_init_seq, 
        const int dram_frequency, 
        const int delay_model, 
        const int enable_pmb, 
        const int phy_init_train, 
        const int dump_phy_regs,
        const int seed,
        const int skip_mcu_init,
        const int pmg_setup_en,
        const int training_en,
        const int skip_part_phy_init,
        const int mov_window_size,
        const int mtc_adjust_step,
        const int mtc_use_read_methodB,
        const int set_bdlr_val
    )
{     
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);
    int ret;
    
    if (skip_mcu_init) 
    {
        return 0;
    }

    dram_type = dram_mode;//for DDC lib
     
    //Seed the random generator
    srand(seed);

    hw_status("KAHALU_SIM_INIT: Entering %s\n", __func__);     

    hw_status("KAHALU_SIM_INIT: dram_mode = %0d, dram_cfg= %0d, autoref_int = %0d, training_test_num = %0d, phy_training_mode = %0d, ddc_param_mode = %0d, wr_dbi_dis = %0d, rd_dbi_dis = %0d, use_real_init_seq = %0d, dram_frequency= %0d, delay_model = %0d, enable_pmb = %0d, pmg_setup_en = %0d training_en=%0d mov_window_size=%0d, mtc_adjust_step=%0d, mtc_use_read_methodB=%0d, set_bdlr_val=0x%0x\n", dram_mode, dram_cfg, autoref_int, training_test_num, phy_training_mode, ddc_param_mode, wr_dbi_dis, rd_dbi_dis, use_real_init_seq, dram_frequency, delay_model, enable_pmb, pmg_setup_en, training_en, mov_window_size, mtc_adjust_step, mtc_use_read_methodB, set_bdlr_val);
#ifdef MCU_IP_VERIF        
    get_current_dir();    
#endif //MCU_IP_VERIF      
    update_ddc_params_frequency(dram_frequency); 
#ifndef CONF_HAS___PHY_RTL
    lp4_param_db.dqss[DDC_MIN] = lp4_param_db.dqss[DDC_MICRON];
    lp4_param_db.dqss[DDC_TYP] = lp4_param_db.dqss[DDC_MICRON];
    lp4_param_db.dqss[DDC_MAX] = lp4_param_db.dqss[DDC_MICRON];
    lp4_param_db.dqss[DDC_REG] = lp4_param_db.dqss[DDC_MICRON];
    lp4_param_db.dqss[DDC_PROG] = lp4_param_db.dqss[DDC_MICRON];

    lp4_param_db.dqs2dq[DDC_MIN] = lp4_param_db.dqs2dq[DDC_MICRON];
    lp4_param_db.dqs2dq[DDC_TYP] = lp4_param_db.dqs2dq[DDC_MICRON];
    lp4_param_db.dqs2dq[DDC_MAX] = lp4_param_db.dqs2dq[DDC_MICRON];
    lp4_param_db.dqs2dq[DDC_REG] = lp4_param_db.dqs2dq[DDC_MICRON];
    lp4_param_db.dqs2dq[DDC_PROG] = lp4_param_db.dqs2dq[DDC_MICRON];
    
    lp4_param_db.dqsck[DDC_MIN] = lp4_param_db.dqsck[DDC_MICRON];
    lp4_param_db.dqsck[DDC_TYP] = lp4_param_db.dqsck[DDC_MICRON];
    lp4_param_db.dqsck[DDC_MAX] = lp4_param_db.dqsck[DDC_MICRON];
    lp4_param_db.dqsck[DDC_REG] = lp4_param_db.dqsck[DDC_MICRON];
    lp4_param_db.dqsck[DDC_PROG] = lp4_param_db.dqsck[DDC_MICRON];    
#endif
        
    update_all_ddc_param(mc,ddc_param_mode, dram_frequency);
      
#ifdef MCU_IP_VERIF        
    update_ddc_params_from_yml();
#endif //MCU_IP_VERIF    
 
    if (use_real_init_seq == 1) {

      #ifdef CONF_HAS___PHY_RTL
      if (ddc_param_mode == -1) {
        ret = mcu_reset_init_seq(mc, dram_mode, active_ranks, dram_cfg, autoref_int, DDC_MICRON, wr_dbi_dis, rd_dbi_dis, dram_frequency, enable_pmb, skip_part_phy_init );
      } else {  
        ret = mcu_reset_init_seq(mc, dram_mode, active_ranks, dram_cfg, autoref_int, ddc_param_mode, wr_dbi_dis, rd_dbi_dis, dram_frequency, enable_pmb, skip_part_phy_init );
      }
      if(ret!=0) {
        return ret;
      }  
      #else
        hw_status("KAHALU_SIM_INIT: ERROR: use_real_init_seq cannot be used without real PHY instantiated\n");
      #endif    
      
      #ifdef MCU_IP_VERIF      
      hw_status("calling update post train\n");  
      update_ddr4_post_train();
      #endif

    } else {

        init_dram(mc, dram_mode, active_ranks, dram_cfg);

        #ifdef CONF_HAS___PHY_RTL
        if (ddc_param_mode == -1) {
            phy_init_synps_lp4(mc, active_ranks, DDC_MICRON, wr_dbi_dis, rd_dbi_dis, dram_frequency, phy_init_train, dump_phy_regs, set_bdlr_val);    
        }   else {
            phy_init_synps_lp4(mc, active_ranks, ddc_param_mode, wr_dbi_dis, rd_dbi_dis, dram_frequency, phy_init_train, dump_phy_regs, set_bdlr_val);
        }
        #endif           
         
        #ifndef MCU_IP_VERIF
            #ifndef SIM_PLAT
                program_ddc_phy_params(mc, rd_dbi_dis, dram_frequency);
            #endif
        #endif

        #ifdef MCU_IP_VERIF      
        hw_status("calling update post train\n");  
        update_ddr4_post_train();
        #endif
          
        hw_status("KAHALU_SIM_INIT: Wait_for_dfi_init\n");
        wait_for_dfi_init(mc);
     
        hw_status("KAHALU_SIM_INIT: update_ddc_fields\n");
        update_ddc_fields(mc, dram_frequency);
 
        set_autorefresh(mc, autoref_int, dram_mode, dram_frequency);
 
        hw_status("KAHALU_SIM_INIT: send_mr_commands\n");
#ifdef MCU_IP_VERIF
        send_mr_commands(mc, dram_mode, active_ranks, wr_dbi_dis, rd_dbi_dis,ddc_param_mode, dram_frequency);
#else
        send_mr_commands(mc, dram_mode, active_ranks, wr_dbi_dis, rd_dbi_dis,DDC_MICRON, dram_frequency);
#endif //MCU_IP_VERIF        
     
        if(enable_pmb) {
            hw_status("KAHALU_SIM_INIT: Enabling PMB\n");
            pmb_enable_function(mc);   
        }
    
        perf_regs_init(mc);

        if ( phy_training_mode > 2) {
            // configure MTC 
            default_mtc_setup(mc, training_en,mov_window_size,mtc_adjust_step, active_ranks,mtc_use_read_methodB);
        }

        hw_status("KAHALU_SIM_INIT: do_yml_reg_writes for rand_reg_data.bin\n");
        do_yml_reg_writes("rand_reg_data.bin");   
        
        hw_status("KAHALU_SIM_INIT: do_yml_reg_writes for reg_data.bin\n");
        do_yml_reg_writes("reg_data.bin"); 

        configure_dram(mc, dram_cfg);
  
#ifdef MCU_IP_VERIF        
        if ( phy_training_mode == 1) {
           hw_status("KAHALU_SIM_INIT: snps_phy_training_tests\n");
           snps_phy_training_tests(training_test_num,dram_mode,dram_frequency);
        } else if ( phy_training_mode == 2) {
           hw_status("KAHALU_SIM_INIT: mtc_training_tests\n");
           mtc_training_tests(training_test_num,dram_mode,autoref_int,delay_model,dram_frequency, active_ranks); 
        } else {
            // enable MTC trainings
            en_mtc_trainings(training_en);
        }
#endif //MCU_IP_VERIF        

        // Power-down the rank1 for single rank mode
        if (active_ranks == 1)
        {
            power_down_rank(0, 1);
        }

        //PMG Tests will program the required settings later;
        //Hence don't do default_pmg_setup if it is a PMG test.
        //if (phy_training_mode != 3)
        if (pmg_setup_en != 3 && pmg_setup_en != -1)
        {
            default_pmg_setup(mc, pmg_setup_en);
        }

#ifndef MCU_IP_VERIF
           release_global_hold_seq(mc, 1, "sch");
#endif //MCU_IP_VERIF            
        
#ifdef MCU_IP_VERIF        
        dbg_test();
#endif //MCU_IP_VERIF        
    }

    hw_status("KAHALU_SIM_INIT: Exiting %s\n", __func__);
    
    return 0;
}
