#ifndef __KAHALU_SIM_INIT_HEADER_INC
#define __KAHALU_SIM_INIT_HEADER_INC


#include <kahalu_utils.h>


//DPI imports
//extern void bfm_read32(uint32_t* addr, uint32_t* data);
//extern void bfm_write32(uint32_t* addr, uint32_t data);
//extern void simulation_tb_wait(uint32_t cycles, char * clock); 
//extern void simulation_wait_ns(uint32_t ns);

// MTC extern functions  from mtc_train.c 
extern void program_non_default_regs_for_mtc_write_training_init();

// DPI export
//extern int simulation_init_seq (const int dram_mode, const int active_ranks, const int dram_cfg, const int autoref_int, const int training_test_num, const int dfi_training_en, const int ddc_param_mode, const int wr_dbi, const int rd_dbi, const int use_real_init_seq, const int dram_frequency, int delay_model, const int enable_pmb, const int phy_init_train, const int dump_phy_regs);
extern int release_global_hold_seq(const int mc, const int cycles, char* clk);



//DDC timing routines
void update_ddc_fields(const int mc, const int dram_frequency);
void update_all_ddc_param(const int mc, int val, const int dram_frequency);
#ifdef MCU_IP_VERIF
void update_ddc_params_from_yml();
#endif //MCU_IP_VERIF
void update_ddc_params_frequency(int frequency);

//Init routines
void write_mr_reg(const int mc, uint32_t mr_addr, uint32_t rank, uint32_t data);
void pmb_enable_function(const int mc);
void configure_dram(const int mc, int config_num);
void mc_setup(const int mc, const int mode, const int active_ranks, const int dram_cfg);
void init_dram(const int mc, const int mode, const int active_ranks, const int dram_cfg);
void wait_for_dfi_init(const int mc);
uint32_t read_mr_reg(const int mc, uint32_t mr_addr, uint32_t rank);
void send_mr_commands(const int mc, const int dram_mode, const int active_ranks, const int wr_dbi_dis, const int rd_dbi_dis, const int ddc_mode, const int frequency);
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
     );

void perf_regs_init(const int mc);
#ifdef MCU_IP_VERIF
extern int simulation_init_seq
#else
EXTERN_C int simulation_init_seq
#endif //MCU_IP_VERIF
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
    );


void do_yml_reg_writes(char *fname);
void set_autorefresh(const int mc, int interval, int dram_mode, const int dram_frequency);
void program_ddc_phy_params(const int mc, const int rd_dbi_dis, const int dram_frequency);

#endif
