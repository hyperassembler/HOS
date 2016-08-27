/*-------------------------------------------------------
 |
 |     init_helper.h
 |
 |     helper functions definition.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2013  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */


#ifndef __INIT_HELPER_H__
#define __INIT_HELPER_H__

#include <xtensa/config/core.h>
#include <stdint.h>

#include "init_helper_common.h"

#define PCIE_RESET_RELEASE_ADDR          (0x04080090)
#define PCIE_RESET_RELEASE_DATA          (0x00000003)
#define HUP_CN_MISC_INTC_P0_0_DATA           (0x04000000)
#define HUP_CN_MISC_INTC_P0_0_CLEAR          (0x04000008)
#define HUP_CN_MISC_INTC_P1_0_SET            (0x04000084)
#define HUP_CN_MISC_INTC_P1_0_CLEAR          (0x04000088)
#define HUP_CN_MISC_INTC_SCRATCH_REG0        (0x04005000)
#define HUP_CN_MISC_INTC_SCRATCH_REG1        (0x04005010)
#define HUP_CN_MISC_POR_BITS4CONTORLCORE     (0x04080800)

#define DRAMC_READY                      (0x00000002)

//
// C-portable version of extern "C"
//
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif



#ifndef MC_INIT_BFTEST
// Bifrost compatibility typedefs and memory access functions
typedef uint8_t UINT8, *PUINT8;
typedef uint32_t UINT32, *PUINT32;

static inline UINT32 hw_read32(volatile UINT32* addr)
{
    return *addr;
}

static inline void hw_write32(volatile UINT32* addr, UINT32 data)
{
    *addr = data;
}

#endif //MC_INIT_BFTEST

typedef volatile unsigned int r32;
typedef volatile unsigned long long r64;
typedef volatile unsigned short r16;
typedef volatile unsigned char r8;
typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef int s32;
typedef short s16;
typedef char s8;

typedef enum{
    PLAT_CHIP                            = 0,
    PLAT_MVP                             = 1, 
    PLAT_PAL                             = 2,
    PLAT_RPP                             = 3,
    PLAT_ISS                             = 4,
    PLAT_SIM                             = 5,
    PLAT_VEL                             = 6,
    PLAT_ZEBU                            = 7,
}PLATFORM_TYPE;

typedef enum{
    OPT_MASK_NO_DRAMC_RESET              = 0x00000001,
    OPT_MASK_NO_DRAMC_INIT               = 0x00000002,
    OPT_MASK_DRAMC_INIT_FOR_BASRAM_MODE  = 0x00000004,
    OPT_MASK_SIM_SILICON_MODE            = 0x00000008,
    OPT_MASK_DDR_MODE                    = 0x00000030,
    OPT_MASK_FREQUENCY                   = 0x00000F00,
    OPT_MASK_UPCTL                       = 0x0000F000,
    OPT_MASK_SCHEDULER                   = 0x000F0000,
    OPT_MASK_PHY                         = 0x3FF00000,
    OPT_MASK_PHY_MR3                     = 0x00C00000,
    OPT_MASK_PHY_ZQ0PR                   = 0x03000000,
    OPT_MASK_PHY_ZQ1PR                   = 0x0C000000,
    OPT_MASK_PHY_PIR                     = 0x10000000,
    OPT_MASK_PHY_DTCR                    = 0x20000000,
}DRAMC_INIT_OPTION_MASK;

typedef enum{
    OPT_MASK_LSB_NO_DRAMC_RESET              = 0,
    OPT_MASK_LSB_NO_DRAMC_INIT               = 1,
    OPT_MASK_LSB_DRAMC_INIT_FOR_BASRAM_MODE  = 2,
    OPT_MASK_LSB_SIM_SILICON_MODE            = 3,
    OPT_MASK_LSB_DDR_MODE                    = 4,
    OPT_MASK_LSB_FREQUENCY                   = 8,
    OPT_MASK_LSB_UPCTL                       = 12,
    OPT_MASK_LSB_SCHEDULER                   = 16,
    OPT_MASK_LSB_PHY                         = 20,
    OPT_MASK_LSB_PHY_MR3                     = 22,
    OPT_MASK_LSB_PHY_ZQ0PR                   = 24,
    OPT_MASK_LSB_PHY_ZQ1PR                   = 26,
    OPT_MASK_LSB_PHY_PIR                     = 28,
    OPT_MASK_LSB_PHY_DTCR                    = 29,   
}DRAMC_INIT_OPTION_MASK_LSB;

typedef enum{
    LPDDR_DEFAULT                            = 0,
    LPDDR_2                                  = 2,
    LPDDR_3                                  = 3,
}DRAMC_INIT_OPTION_DDR_MODE;

typedef enum{
    CHANNEL_BIT9_DEFAULT                     = 1,
    CHANNEL_BIT10                            = 2,
    CHANNEL_BIT11                            = 4,
    CHANNEL_BIT26                            = 8,
}POR_INIT_DRAMC_INTERLEAVE_MODE;

typedef enum{
    LPDDR_SIZE_DEFAULT                       = 0,
    LPDDR_SIZE_4Gb                           = 4,
    LPDDR_SIZE_8Gb                           = 8,
}DRAMC_INIT_OPTION_DDR_SIZE;

typedef enum{
    LPDDR_RANKS_DEFAULT                      = 0,
    LPDDR_RANKS_SINGLE                       = 1,
    LPDDR_RANKS_DOUBLE                       = 2,
}DRAMC_INIT_OPTION_DDR_RANKS;

typedef enum{
    FREQ_DEFAULT                              = 1600,
    FREQ_400MHZ                               = 400,
    FREQ_800MHZ                               = 800,
    FREQ_1066MHZ                              = 1066,
    FREQ_1333MHZ                              = 1333,
}DRAMC_INIT_OPTION_FREQUENCY;

typedef enum{
    UPCTL_MASK_TREFI                     = 0x00000001,
}DRAMC_INIT_OPTION_UPCTL_MASK;


typedef enum{
    DRAMC_INIT_BEGIN                     = 0x00050000,
    DRAMC_INIT_POR                       = 0x00051000,
    DRAMC_INIT_SCHEDULER                 = 0x00052000,
    DRAMC_INIT_FREQUENCY                 = 0x00053000,
    DRAMC_INIT_PHY_STARTED               = 0x00054000,
    DRAMC_INIT_PHY_BEFORE_TRAINING       = 0x00054001,
    DRAMC_INIT_PHY_AFTER_TRAINING        = 0x00054002,
    DRAMC_INIT_DFI                       = 0x00055000,
    DRAMC_INIT_POWER_UP                  = 0x00056000,
    DRAMC_INIT_TIMING_REGS               = 0x00057000,
    DRAMC_INIT_DFI_TIMING_REGS           = 0x00058000,
    DRAMC_INIT_MEMORY_STARTED            = 0x00059000,
    DRAMC_INIT_MEMORY_MRW_RESET          = 0x00059001,
    DRAMC_INIT_MEMORY_MRW_ZQ             = 0x00059002,
    DRAMC_INIT_MEMORY_MRW_MR2            = 0x00059003,
    DRAMC_INIT_MEMORY_MRW_MR1            = 0x00059004,
    DRAMC_INIT_MEMORY_MRW_MR3            = 0x00059005,
    DRAMC_INIT_MEMORY_MRW_REF            = 0x00059006,  
    DRAMC_INIT_DFICTRLUPD                = 0x0005A000,
    DRAMC_INIT_MOVE_STATE_CONFIG         = 0x0005B000,
    DRAMC_INIT_OVERRIDE_SEQ_STARTED      = 0x0005C000,
    DRAMC_INIT_OVERRIDE_SEQ_CONFIG       = 0x0005C001,
    DRAMC_INIT_OVERRIDE_SEQ_MCMD         = 0x0005C002,  
    DRAMC_INIT_MOVE_STATE_ACCESS         = 0x0005D000,
}DRAMC_INIT_STATUS;

#define reg_write32(addr, val) ((*(volatile unsigned int*)(addr)) = val)
#define reg_read32(addr, val)  (val = (*(volatile unsigned int*)addr))

#define get_prid() xthal_get_prid()
#define get_ccount() xthal_get_ccount()

static inline void delay_cycles(unsigned int cycles)
{
    unsigned int start = get_ccount();
    while((unsigned int)(get_ccount() - start) < cycles);
}

#define delay_ns(ns) delay_cycles(ns / 2)
#define delay_us(us) delay_cycles(us * 1000 / 2)
#define delay_ms(ms) delay_cycles(ms * 1000000 / 2)

static inline PLATFORM_TYPE get_platform_type()
{
#ifdef SIM_PLAT
    return PLAT_SIM;
#else
    unsigned int reg_data = 0;
    reg_data = *(volatile unsigned int*)(HUP_CN_MISC_POR_BITS4CONTORLCORE);
    return (PLATFORM_TYPE)((reg_data & 0xFF000000) >> 24);
#endif
}

static inline unsigned int get_dramc_init_option(DRAMC_INIT_OPTION_MASK option_mask)
{
    unsigned int reg_data = *(volatile unsigned int*)(HUP_CN_MISC_INTC_SCRATCH_REG1);
    unsigned int ret = 0;
    PLATFORM_TYPE plat = get_platform_type();
    switch(option_mask)
    {
        case OPT_MASK_DRAMC_INIT_FOR_BASRAM_MODE:
            ret = (reg_data & OPT_MASK_DRAMC_INIT_FOR_BASRAM_MODE) >> OPT_MASK_LSB_DRAMC_INIT_FOR_BASRAM_MODE;
            break;
            
        case OPT_MASK_NO_DRAMC_INIT:
            ret = (reg_data & OPT_MASK_DRAMC_INIT_FOR_BASRAM_MODE) >> OPT_MASK_LSB_DRAMC_INIT_FOR_BASRAM_MODE; 
            break;
            
        case OPT_MASK_NO_DRAMC_RESET:
            ret = (reg_data & OPT_MASK_NO_DRAMC_RESET) >> OPT_MASK_LSB_NO_DRAMC_RESET;
            break;
        
        case OPT_MASK_SIM_SILICON_MODE:
            ret = (reg_data & OPT_MASK_SIM_SILICON_MODE) >> OPT_MASK_LSB_SIM_SILICON_MODE;
            break;

        case OPT_MASK_DDR_MODE:
            if((plat == PLAT_PAL) || (plat == PLAT_RPP))
            {
                ret = LPDDR_3;
            }
            else
            {
                ret = (reg_data & OPT_MASK_DDR_MODE) >> OPT_MASK_LSB_DDR_MODE;
            }
            break;

        case OPT_MASK_FREQUENCY:
            if((plat == PLAT_PAL) || (plat == PLAT_RPP))
            {
                ret = FREQ_800MHZ;
            }
            else
            {
                ret = (reg_data & OPT_MASK_FREQUENCY) >> OPT_MASK_LSB_FREQUENCY;    
            }
            break;
        
        case OPT_MASK_UPCTL:
            ret = (reg_data & OPT_MASK_UPCTL) >> OPT_MASK_LSB_UPCTL;
            break;
            
        case OPT_MASK_SCHEDULER:
            ret = (reg_data & OPT_MASK_SCHEDULER) >> OPT_MASK_LSB_SCHEDULER;
            break;

        case OPT_MASK_PHY:
            ret = (reg_data & OPT_MASK_PHY) >> OPT_MASK_LSB_PHY;
            break;

        case OPT_MASK_PHY_MR3:
            ret = (reg_data & OPT_MASK_PHY_MR3) >> OPT_MASK_LSB_PHY_MR3;
            break;

        case OPT_MASK_PHY_ZQ0PR:
            ret = (reg_data & OPT_MASK_PHY_ZQ0PR) >> OPT_MASK_LSB_PHY_ZQ0PR;
            break;

        case OPT_MASK_PHY_ZQ1PR:
            ret = (reg_data & OPT_MASK_PHY_ZQ1PR) >> OPT_MASK_LSB_PHY_ZQ1PR;
            break;

        case OPT_MASK_PHY_PIR:
            ret = (reg_data & OPT_MASK_PHY_PIR) >> OPT_MASK_LSB_PHY_PIR;
            break;

        case OPT_MASK_PHY_DTCR:
            ret = (reg_data & OPT_MASK_PHY_DTCR) >> OPT_MASK_LSB_PHY_DTCR;
            break;

        default:
            ret = 0;
            break;
    }
    return ret;
}
    
static inline void set_dramc_ready()
{
    delay_us(10);
    *(unsigned int*)(HUP_CN_MISC_INTC_P1_0_SET) = 0x2;
}

static inline void wait_for_dramc_ready()
{
    unsigned int reg_data = 0;
    do
    {
        reg_data = *(volatile unsigned int*)(HUP_CN_MISC_INTC_P1_0_CLEAR);
    }while((reg_data & 0x2) == 0);

    *(unsigned int*)(HUP_CN_MISC_INTC_P1_0_CLEAR) = 0x2;
}
static void inline post_status(unsigned int status )
{
    *(unsigned int *)(HUP_CN_MISC_INTC_SCRATCH_REG0) = status;
}

#endif // __INIT_HELPER_H__

