#ifndef __KAHALU_UTILS_HEADER_INC
#define __KAHALU_UTILS_HEADER_INC

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <unistd.h>
#include <errno.h>

#ifdef MCU_IP_VERIF
    #include <svdpi.h>
    #include <bist.h>
    #include "conf_defs.h"
    #define hw_status(...) printf (__VA_ARGS__)
#else
    #ifdef REAL_MCPHY
        #define CONF_HAS___PHY_RTL
    #endif //REAL_MCPHY
    #include "init_helper.h"
    #ifndef MC_INIT_BFTEST
        #define hw_status(...)
    #else
        #include "bifrost.h"
    #endif
    #include "hup_chip.h"
#endif //MCU_IP_VERIF


#include <ddc_params.h>
#include <mcu_regs.h>
#include <kahalu_global_defines.h>


//Reg Model
static PTR_Mcu_regs chip_regs = (PTR_Mcu_regs)(MCU_REGS_MCP_REGS_ADDRESS);
extern uint32_t* g_last_regacc_addr;
extern REGACC_TYPE g_last_regacc_type;
extern int dram_type;
extern lp4_ddc_params lp4_param_db;
extern lp3_ddc_params lp3_param_db;

extern void simulation_tb_wait(uint32_t cycles, const char * clock);
#ifdef MCU_IP_VERIF
//DPI imports
extern void bfm_read32(uint32_t* addr, uint32_t* data);
extern void bfm_write32(uint32_t* addr, uint32_t data);
extern void update_ddc_params_from_yml();
#define hw_sleep(ns) simulation_wait_ns(ns);
extern void simulation_wait_ns(uint32_t t_ns);
extern void print_sim_time();
extern char* get_sim_time();
extern int sv_get_wire_delay_reg_wr(uint32_t rank, uint32_t slice);
extern int sv_get_wire_delay_reg_rd(uint32_t rank, uint32_t slice);
#else

#ifndef MC_INIT_BFTEST
#define hw_sleep(ns) delay_ns(ns)
#endif
#endif //MCU_IP_VERIF
// DPI export
//extern int simulation_init_seq (const int dram_mode, const int active_ranks, const int dram_cfg, const int autoref_int, const int training_test_num, const int dfi_training_en, const int ddc_param_mode, const int wr_dbi, const int rd_dbi, const int use_real_init_seq, const int dram_frequency, int delay_model, const int enable_pmb, const int phy_init_train, const int dump_phy_regs);
//extern void release_global_hold_seq(const int cycles, char* clk);


static inline void hw_phy_regacc_post_wait(uint32_t is_read)
{
    //The SNPS LPDDR4 PHY requires spacing between register accesses.
    //Assuming if there was an access to a non-LPDDR4-PHY register
    //that would take more time than the required spacing,
    //this delay is added only when there are consecutive accesses 
    //to LPDDR4-PHY registers, from the single thread that manages
    //the LPDDR4-PHY; it is also assumed that only one thread will
    //be managing the LPDDR4-PHY at the SoC level.
     //TODO: FIXME: The tb_wait routines should be replaced with appropriate
     //wait routines for SoC level
     if ( is_read )
     {
         //Command Spacing from Read to a following Read/Write: 2 pclk cycles.
         simulation_tb_wait(2, "apb");
     }
     else
     {
         //Command spacing from Write to a following Write/Read: 
         // 1 pclk + (`DWC_AFIFO_SYNC_STAGES+ 4) ctl_clk cycles
         // DWC_AFIFO_SYNC_STAGES = 4; to be safe making it 10 ctl_clk cycles
         simulation_tb_wait(10, "mem"); 
         simulation_tb_wait(1, "apb");
     }
}


static inline uint32_t hw_phy_read32(uint32_t* addr)
{
    uint32_t ret;
    //hw_phy_regacc_wait(addr);
#ifdef MCU_IP_VERIF
    bfm_read32(addr, &ret);
#else
    ret = hw_read32((uint32_t*)addr);
#endif //MCU_IP_VERIF    
    hw_phy_regacc_post_wait(1);
    return ret;}

static inline void hw_phy_write32(uint32_t* addr, uint32_t data)
{
    //hw_phy_regacc_wait(addr);
#ifdef MCU_IP_VERIF
    bfm_write32(addr, data);
#else
    hw_write32((uint32_t*)addr, data);
#endif //MCU_IP_VERIF
    hw_phy_regacc_post_wait(0);
}


#ifdef MCU_IP_VERIF
static inline uint32_t hw_read32(uint32_t* addr)
{
    uint32_t ret;
    //hw_phy_regacc_wait(addr);
    bfm_read32(addr, &ret);
    return ret;}

static inline void hw_write32(uint32_t* addr, uint32_t data)
{
    //hw_phy_regacc_wait(addr);
    bfm_write32(addr, data);
}

#endif //MCU_IP_VERIF

static inline void print_with_time(const char *line)
{
#ifdef MCU_IP_VERIF
    print_sim_time();
#endif //MCU_IP_VERIF
    hw_status("%s", line);
}

static inline uint32_t get_mcu_baseaddr(const int mc)
{
    
    uint32_t base_addr = 0;

#ifndef MCU_IP_VERIF
    base_addr = HUP_CHIP_MCU0_ADDRESS + (HUP_CHIP_MCU1_ADDRESS-HUP_CHIP_MCU0_ADDRESS)*mc;
#endif //MCU_IP_VERIF 
   
    return base_addr;
}

//Helper functions
//extern int dram_type;
//extern lp4_ddc_params lp4_param_db;
//extern lp3_ddc_params lp3_param_db;
uint32_t getRandInterval(uint32_t begin, uint32_t end);
#ifdef MCU_IP_VERIF
void get_current_dir();
void print_addr(volatile uint32_t* address);
void do_yml_reg_writes(char *fname);
#endif //MCU_IP_VERIF

void set_reg(const int mc, uintptr_t addr, uint32_t field_mask, uint32_t data);
void set_reg_val(const int mc, uintptr_t addr, uint32_t field_mask, uint32_t data);


#endif
