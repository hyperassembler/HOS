/*-------------------------------------------------------
 |
 |      bifrost_system_constants_x86.h
 |
 |      Contains project-specific Bifrost information,
 |      such as timer, interrupt, and exception types
 |      for the 'x86' archtype
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2016  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#ifndef _BIFROST_SYSTEM_CONSTANTS_X86_H
#define _BIFROST_SYSTEM_CONSTANTS_X86_H

//
// Bifrost architecture types
//
typedef enum
{
HW_X86_64,
HW_ARCHTYPE_CNT
} HW_ARCHTYPE;


//
// Interrupt
//

#define HW_TS_NUM_INTVECTORS           (256)
#define HW_TS_NUM_EXCVECTORS             (0)

#define HW_INT_TASK_SWITCH (0)
#define HW_TD_SUSPEND_TIMER (HW_INT_VECTOR)(0)

//
// Interrupt/pagetable stuff
//

typedef enum {
//
// Architecturally-defined interrupts: 0 through 31
//

HW_INT_DIVIDE_ERROR         = 0,
// Vector 1 is reserved     = 1,
HW_INT_NMI                  = 2,
HW_INT_BREAKPOINT           = 3,
HW_INT_OVERFLOW             = 4,
HW_INT_BOUND_RANGE_INTEEDED = 5,
HW_INT_INVALID_OPCODE       = 6,
HW_INT_DEVICE_NOT_AVAILABLE = 7,
HW_INT_DOUBLE_FAULT         = 8,
// Vector 9 is reserved     = 9,
HW_INT_INVALID_TSS         = 10,
HW_INT_SEGMENT_NOT_PRESENT = 11,
HW_INT_STACK_SEGMENT_FAULT = 12,
HW_INT_GENERAL_PROTECTION  = 13,
HW_INT_PAGE_FAULT          = 14,
// Vector 15 is reserved   = 15,
HW_INT_MATH_FAULT          = 16,
HW_INT_ALIGNMENT_CHECK     = 17,
HW_INT_MACHINE_CHECK       = 18,
HW_INT_SIMD_FP_INTEPTION   = 19,
// Vectors 20 through 31 are reserved

//
// Software-defined interrupts: 32 through 255.
// Reserve vectors for devices first.
//
HW_INT_DEVICE_START          = 32,
HW_INT_ORION_USB3_0          = HW_INT_DEVICE_START,
HW_INT_ORION_USB3_1,
HW_INT_ORION_USB3_2,
HW_INT_ORION_SATA_0,
HW_INT_ORION_SATA_1,
HW_INT_ORION_IRTE_START,                                // USB2, USB1, and SFC use a different setup method
HW_INT_ORION_USB2            = HW_INT_ORION_IRTE_START, // They are the first 3 IRTE vectors
HW_INT_ORION_EHCI            = HW_INT_ORION_USB2,       // The USB2 IRTE vector is also known as EHCI
HW_INT_ORION_USB1,
HW_INT_ORION_OHCI            = HW_INT_ORION_USB1,       // The USB1 IRTE vector is also known as OHCI
HW_INT_ORION_SFC,
HW_INT_ORION_UART,
HW_INT_ORION_VIB_HDMI,
HW_INT_ORION_VIB_DP,
HW_INT_ORION_RF8,
HW_INT_ORION_RF10,
HW_INT_ORION_RF11,
HW_INT_ORION_SCI,
HW_INT_ORION_HPET0,
HW_INT_ORION_HPET1,
HW_INT_ORION_HPET2,
HW_INT_ORION_HPET3,
HW_INT_ORION_PMU,
HW_INT_ORION_GPIO0,
HW_INT_ORION_GPIO1,
HW_INT_ORION_GPIO2,
HW_INT_ORION_GPIO3,
HW_INT_ORION_USB3_0_WAKE,
HW_INT_ORION_USB3_1_WAKE,
HW_INT_ORION_USB3_2_WAKE,
HW_INT_ORION_USB2_WAKE,
HW_INT_ORION_TMDS_WAKE,
HW_INT_ORION_RF10_WAKE,
HW_INT_ORION_RF11_WAKE,
HW_INT_ORION_RSVD0,
HW_INT_ORION_RSVD1,
HW_INT_ORION_RSVD2,
HW_INT_ORION_RSVD3,
HW_INT_ORION_SMC,
HW_INT_ORION_HBIB,
HW_INT_ORION_IRTE_END        = HW_INT_ORION_HBIB,
HW_INT_GNB_CB000,
HW_INT_GNB_CB001,
HW_INT_GNB_CB100,
HW_INT_GNB_CB101,
HW_INT_GNB_DB000,
HW_INT_GNB_DB001,
HW_INT_GNB_DB100,
HW_INT_GNB_DB101,
HW_INT_GNB_TCCA00,
HW_INT_GNB_TCCA01,
HW_INT_GNB_TCCA10,
HW_INT_GNB_TCCA11,
HW_INT_GNB_HDP,
HW_INT_GNB_XDP,
HW_INT_GNB_UVD,
HW_INT_GNB_UMC,
HW_INT_GNB_VCE,
HW_INT_GNB_VCEU,
HW_INT_GNB_IA,
HW_INT_GNB_SDMA0,
HW_INT_GNB_SDMA1,
HW_INT_GNB_SDMA2,
HW_INT_GNB_SDMA3,
HW_INT_GNB_CPC,
HW_INT_GNB_CPF,
HW_INT_GNB_CPG,
HW_INT_DEVICE_END         =  HW_INT_GNB_CPG,

//
// User vectors
//
HW_INT_USER_START,
HW_INT_DUMMY_INTERRUPT    = HW_INT_USER_START,
HW_INT_LOGAN_INTERRUPT,

HW_INT_TLB_FLUSH          =  240,
HW_INT_HALT               =  241,
HW_INT_WAKE               =  242,
HW_INT_WRITEMSR           =  243,
HW_INT_UART_THRE          =  244,

HW_INT_CNT                =  256
} HW_INT_VECTOR;

typedef enum {
HW_EXC_CNT
} HW_EXC_VECTOR;

#define HW_INT_PROTECTION_MASK   (0xFFFFFFFF)

#define HW_INT_INITIALIZED       (0xBAFF00BA)
#define HW_EXC_INITIALIZED       (0xBAFF00BA)


#endif // _BIFROST_SYSTEM_CONSTANTS_X86_H

