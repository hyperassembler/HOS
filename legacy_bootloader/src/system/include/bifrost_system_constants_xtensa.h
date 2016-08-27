/*-------------------------------------------------------
 |
 |      bifrost_system_constants_xtensa.h
 |
 |      Contains project-specific Bifrost information,
 |      such as timer, interrupt, and exception types
 |      for the 'xtensa' archtype
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2016  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#ifndef _BIFROST_SYSTEM_CONSTANTS_XTENSA_H
#define _BIFROST_SYSTEM_CONSTANTS_XTENSA_H

// Xtensa processors are 32-bit
#define ENV_32BIT

#include "conf_defs.h"
//
// Bifrost architecture types
//
typedef enum
{
HW_XTENSA_SFP,
HW_XTENSA_VFP,
HW_ARCHTYPE_CNT
} HW_ARCHTYPE;


//
// Large datatypes
//
#if defined(xtensa_vfp_TIE)
typedef vx32                            UINT128;
#elif defined(xtensa_sfp_TIE)
typedef simd128                         UINT128;
#else
typedef struct {
    uint32_t data[4];
} UINT128;
#endif

//
// Interrupt
//

#define HW_TS_NUM_INTVECTORS            (32)
#define HW_TS_NUM_EXCVECTORS            (64)

typedef enum {
    HW_INT_TIMER_0                  = 0,
    HW_INT_TIMER_1                  = 1, HW_TD_SUSPEND_TIMER = 1,
    HW_INT_TIMER_2                  = 2,
    HW_INT_INTC_0                   = 3,
    HW_INT_INTC_1                   = 4,
    HW_INT_INTC_2                   = 5,
    HW_INT_INTC_3                   = 6,
    HW_INT_CUSTOM_PERF_COUNT_BLOCK  = 7,
    HW_INT_CLASSIFIER               = 8,
    HW_INT_PIF_WRITE_ERR            = 9,
    HW_INT_PERF_COUNTER_OVERFLOW    = 10,
    HW_INT_CDMA_LOW_WTRMARK         = 11,
    HW_INT_SW_0                     = 16,
    HW_INT_SW_1                     = 17,
    HW_INT_SW_2                     = 18,
    HW_INT_SW_3                     = 19,
    HW_INT_SW_4                     = 20,
    HW_INT_SW_5                     = 21, HW_INT_TASK_SWITCH              = 21,
    HW_INT_SW_6                     = 22,
    HW_INT_SW_7                     = 23,
    HW_INT_SW_8                     = 24,
    HW_INT_SW_9                     = 25,
    HW_INT_SW_10                    = 26,
    HW_INT_SW_11                    = 27,
    HW_INT_SW_12                    = 28,
    HW_INT_SW_13                    = 29,
    HW_INT_SW_14                    = 30,
    HW_INT_SW_15                    = 31,
    HW_INT_CNT
} HW_INT_VECTOR;

//
// Exception
//

typedef enum {
    HW_EXC_ILLEGAL                  = 0,
    HW_EXC_SYSCALL                  = 1,
    HW_EXC_INSTR_ERROR              = 2,
    HW_EXC_LOAD_STORE_ERROR         = 3,
    HW_EXC_LEVEL1_INTERRUPT         = 4,
    HW_EXC_ALLOCA                   = 5,
    HW_EXC_DIVIDE_BY_ZERO           = 6,
    HW_EXC_RESERVED_7               = 7,
    HW_EXC_PRIVILEGED               = 8,
    HW_EXC_UNALIGNED                = 9,
    HW_EXC_RESERVED_10              = 10,
    HW_EXC_RESERVED_11              = 11,
    HW_EXC_INSTR_DATA_ERROR         = 12,
    HW_EXC_LOAD_STORE_DATA_ERROR    = 13,
    HW_EXC_INSTR_ADDR_ERROR         = 14,
    HW_EXC_LOAD_STORE_ADDR_ERROR    = 15,
    HW_EXC_ITLB_MISS                = 16,
    HW_EXC_ITLB_MULTIHIT            = 17,
    HW_EXC_INSTR_RING               = 18,
    HW_EXC_RESERVED_19              = 19,
    HW_EXC_INSTR_PROHIBITED         = 20,
    HW_EXC_RESERVED_21              = 21,
    HW_EXC_RESERVED_22              = 22,
    HW_EXC_RESERVED_23              = 23,
    HW_EXC_DTLB_MISS                = 24,
    HW_EXC_DTLB_MULTIHIT            = 25,
    HW_EXC_LOAD_STORE_RING          = 26,
    HW_EXC_RESERVED_27              = 27,
    HW_EXC_LOAD_PROHIBITED          = 28,
    HW_EXC_STORE_PROHIBITED         = 29,
    HW_EXC_RESERVED_30              = 30,
    HW_EXC_RESERVED_31              = 31,
    HW_EXC_CP_DISLAVBLED_N          = 32,
    HW_EXC_CN_N_DISABLEWD           = 33,
    HW_EXC_RESERVED_34              = 34,
    HW_EXC_RESERVED_35              = 35,
    HW_EXC_RESERVED_36              = 36,
    HW_EXC_RESERVED_37              = 37,
    HW_EXC_RESERVED_38              = 38,
    HW_EXC_RESERVED_39              = 39,
    HW_EXC_RESERVED_40              = 40,
    HW_EXC_RESERVED_41              = 41,
    HW_EXC_RESERVED_42              = 42,
    HW_EXC_RESERVED_43              = 43,
    HW_EXC_RESERVED_44              = 44,
    HW_EXC_RESERVED_45              = 45,
    HW_EXC_RESERVED_46              = 46,
    HW_EXC_RESERVED_47              = 47,
    HW_EXC_RESERVED_48              = 48,
    HW_EXC_RESERVED_49              = 49,
    HW_EXC_RESERVED_50              = 50,
    HW_EXC_RESERVED_51              = 51,
    HW_EXC_RESERVED_52              = 52,
    HW_EXC_RESERVED_53              = 53,
    HW_EXC_RESERVED_54              = 54,
    HW_EXC_RESERVED_55              = 55,
    HW_EXC_RESERVED_56              = 56,
    HW_EXC_RESERVED_57              = 57,
    HW_EXC_RESERVED_58              = 58,
    HW_EXC_RESERVED_59              = 59,
    HW_EXC_RESERVED_60              = 60,
    HW_EXC_RESERVED_61              = 61,
    HW_EXC_RESERVED_62              = 62,
    HW_EXC_RESERVED_63              = 63,
    HW_EXC_CNT
} HW_EXC_VECTOR;


#define HW_INT_LEVEL3_MASK       (0x00000684)
#define HW_INT_LEVEL2_MASK       (0x00000062)
#define HW_INT_LEVEL1_MASK       (0xFFFFF919)
#define HW_INT_PROTECTION_MASK   (0xFFFFFFFE)

#define HW_INT_INITIALIZED       (0xBAFF00BA)
#define HW_EXC_INITIALIZED       (0xBAFF00BA)

//
// Timer
//
typedef enum {
HW_TIMER_0    = 0,
HW_TIMER_1    = 1,
HW_TIMER_2    = 2,
HW_TIMER_CNT
} HW_TIMER;

//
// Misc
//

// The base stack offset where the exception return PC
// is stored, relative to an interrupt handler's stack pointer
#define EPC_STACK_OFFSET (160)

#endif // _BIFROST_SYSTEM_CONSTANTS_XTENSA_H

