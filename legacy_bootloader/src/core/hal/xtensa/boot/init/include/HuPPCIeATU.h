//---------------------------------------------------------------------------
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  Abstract:
//      Contains hardware data structure definitions of the internal
//      address translation unit (iATU) of the PCIe endpoint on the HuP.
//
//---------------------------------------------------------------------------

#pragma once

static const UINT32 HUP_ADDRESS_pcie = 0x04400000;
static const UINT32 XTENSA_PCIE_BASE = 0x3E000000;
static const UINT32 XTENSA_PCIE_SIZE = 0x02000000;
static const UINT32 XTENSA_PCIE_END = 0x40000000;

// These constants are configured in silicon and cannot be read by
// software.
//
static const UINT32  PCIE_ATU_CX_INTERNAL_ATU_ENABLE      = 1;                   // Indicates that the ATU is enabled
static const UINT32  PCIE_ATU_CX_ATU_NUM_OUTBOUND_REGIONS = 32;                  // Maximum number of outbound translation regions that can be defined
static const UINT32  PCIE_ATU_CX_ATU_NUM_INBOUND_REGIONS  = 32;                  // Maximum number of inbound translation regions that can be defined
static const UINT32  PCIE_ATU_CX_ATU_MIN_REGION_SIZE      = (UINT32)(4 * 1024);  // Minimum size and alignment of a single translation region

// Starting byte and DWORD offsets of ATU registers in PCIe config space
static const UINT32 PCIE_ATU_STARTING_BYTE_OFFSET = 0x900;
static const UINT32 PCIE_ATU_STARTING_DWORD_OFFSET = 0x900 >> 2;


// Byte and DWORD offsets of the last ATU register in PCIe config space.
static const UINT32 PCIE_ATU_LAST_REGISTER_BYTE_OFFSET = 0x920;
static const UINT32 PCIE_ATU_LAST_REGISTER_DWORD_OFFSET = 0x920 >> 2;

//static const UINT32 PCIE_ATU_REGION_DIR_OUTBOUND = 0;
//static const UINT32 PCIE_ATU_REGION_DIR_INBOUND = 1;
#define PCIE_ATU_REGION_DIR_OUTBOUND 0;
#define PCIE_ATU_REGION_DIR_INBOUND 1;


//////////////////////////////////////////////////////
// ATU Register Definitions.
// Register byte offsets are in comments.
// Where the spec defines inbound and outbound registers at the same
// offset, if they are identical then only one version is defined here.
// If they are not identical then the register contains a union of 2
// bitfield structs (one for each direction).
//
// The viewport register selects a region and direction for subsequent
// register read/write operations.
//

//////////////////////////////////////
// Example 1:
// ASIC-side code to set outbound region 2 to map the first 4K
// of the HuP PCIe 1GB data window to a specific SoC address
/*

PPCIE_ATU_REGISTERS pRegisters = (PPCIE_ATU_REGISTERS)(0x4400000 + PCIE_ATU_STARTING_BYTE_OFFSET); // 0x4400000 is start of PCIe config space in HuP memory map

pRegisters->VIEWPORT.Bits.REGION_DIR = PCIE_ATU_REGION_DIR_OUTBOUND;
pRegisters->VIEWPORT.Bits.REGION_INDEX = 2;


pRegisters->LWR_BASE.Raw = 0x40000000;                                                          // 0x400000000 is start of 1GB PCIe data window in HuP memory map. This must be 4K-aligned.
pRegisters->UPPER_BASE = 0;
pRegisters->LIMIT_ADDR.Raw = 0x40000FFF;                                                        // create a 4K mapping. This is the lower 32-bits of the address of the last byte in the mapping. (Upper bits are defined by UPPER_BASE).
pRegisters->LWR_TARGET_ADDR.Raw = SocAddress.LowPart;
pRegisters->UPPER_TARGET_ADDR = SocAddress.HighPart;

pRegisters->REGION_CTRL1.Raw = 0;
pRegisters->REGION_CTRL3.Raw = 0; 
pRegisters->REGION_CTRL2.Raw = 0;
pRegisters->REGION_CTRL2.OutboundBits.REGION_EN = 1;                                            // Indicates that the region mapping should be used. Always set this register last after all other registers for the region have been programmed.


////////////////////////////
// Example 2:
// SoC-side code to set inbound region 3 to map the INTC register set
// on BAR 0.

PPCIE_ATU_REGISTERS pRegisters = (PPCIE_ATU_REGISTERS)(pBAR1 + PCIE_ATU_STARTING_BYTE_OFFSET);  // Assumes that pBAR1 was set to the system virtual address of the start of BAR 1 from a prior call to MmMapIoSpace and that BAR 1 maps to the PCIe core register set.

pRegisters->VIEWPORT.Bits.REGION_DIR = PCIE_ATU_REGION_DIR_INBOUND;
pRegisters->VIEWPORT.Bits.REGION_INDEX = 3;

pRegisters->LWR_BASE.Raw = pBAR0Physical;                                                       // Assumes that pBAR0Physical was set to the SoC's 4K-aligned starting physical address for BAR 0 assigned by Windows and received during IRP_MN_START_DEVICE or PrepareHardware
pRegisters->UPPER_BASE = 0;                                                                     // Assumes 32-bit Windows. If running on 64-bit set UPPER_BASE to upper DWORD of pBAR0Physical
pRegisters->LIMIT_ADDR.Raw = pBAR0Physical + 0xFFF;                                             // Create a 4K mapping. This is the lower 32-bits of the address of the last byte in the mapping. (Upper bits are defined by UPPER_BASE).
pRegisters->LWR_TARGET_ADDR.Raw = 0x4000000;                                                    // Start of INTC registers in HuP memory map. Must be 4K-aligned.
pRegisters->UPPER_TARGET_ADDR = 0;                                                              // HuP uses 32-bit addressing so high word is zero

pRegisters->REGION_CTRL1.Raw = 0;
pRegisters->REGION_CTRL2.Raw = 0;
pRegisters->REGION_CTRL3.Raw = 0; 
pRegisters->REGION_CTRL2.InboundBits.BAR_NUM = 0;                                               // Shown for completeness. Setting Raw to zero earlier already set this bit to zero. For other BARs set this field to the BAR number.
pRegisters->REGION_CTRL2.InboundBits.REGION_EN = 1;


*/


typedef struct PCIeATUViewPortBits
{
    UINT32 REGION_INDEX:5;
    UINT32 VP_RSVD:26;
    UINT32 REGION_DIR:1;
} PCIE_ATU_VIEWPORT_BITS;

typedef union PCIeATUViewport
{
    PCIE_ATU_VIEWPORT_BITS Bits;
    UINT32                 Raw;

} PCIE_ATU_VIEWPORT;



// Per-direction, per-channel registers
//
typedef struct PCIeATURegionCtrl1Bits
{
    UINT32 TYPE:5;
    UINT32 TC:3;
    UINT32 TD:1;
    UINT32 ATTR:2;
    UINT32 IDO:1;
    UINT32 TH:1;
    UINT32 RSVD_P_13:3;
    UINT32 AT:2;
    UINT32 PH:2;
    UINT32 CTRL_1_FUNC_NUM:5;
    UINT32 RSVDP_25:7;
} PCIE_ATU_REGION_CTRL1_BITS;

typedef union PCIeATURegionCtrl1
{
    PCIE_ATU_REGION_CTRL1_BITS Bits;
    UINT32                     Raw;
} PCIE_ATU_REGION_CTRL1;



typedef struct PCIeATURegionCtrl2OutboundBits
{
    UINT32 MSG_CODE:8;
    UINT32 RSVDP_8:11;
    UINT32 FUNC_BYPASS:1;
    UINT32 RSVDP_20:7;
    UINT32 DMA_BYPASS:1;
    UINT32 CFG_SHIFT_MODE:1;
    UINT32 INVERT_MODE:1;
    UINT32 RSVDP_30:1;
    UINT32 REGION_EN:1;
} PCIE_ATU_REGION_CTRL2_OUTBOUND_BITS;

typedef struct PCIeATURegionCtrl2InboundBits
{
    UINT32 MSG_CODE:8;
    UINT32 BAR_NUM:3;
    UINT32 RSVDP_11:3;
    UINT32 TC_MATCH_EN:1;
    UINT32 TD_MATCH_EN:1;
    UINT32 ATTR_MATCH_EN:1;
    UINT32 TH_MATCH_EN:1;
    UINT32 AT_MATCH_EN:1;
    UINT32 FUNC_MATCH_EN:1;
    UINT32 VF_MATCH_EN:1;
    UINT32 MSG_CODE_MATCH_EN:1;
    UINT32 PH_MATCH_EN:1;
    UINT32 RSVDP_23:1;
    UINT32 RESPONSE_CODE:2;
    UINT32 RSVDP_26:1;
    UINT32 FUZZY_TYPE_MATCH_CODE:1;
    UINT32 CFG_SHIFT_MODE:1;
    UINT32 INVERT_MODE:1;
    UINT32 MATCH_MODE:1;
    UINT32 REGION_EN:1;
} PCIE_ATU_REGION_CTRL2_INBOUND_BITS;


typedef union PCIeATURegionCtrl2
{
    PCIE_ATU_REGION_CTRL2_OUTBOUND_BITS OutboundBits;
    PCIE_ATU_REGION_CTRL2_INBOUND_BITS  InboundBits;
    UINT32                              Raw;
} PCIE_ATU_REGION_CTRL2;


typedef struct PCIeATULwrBaseBits
{
    UINT32 LWR_BASE_HW:12;
    UINT32 LWR_BASE_RW:20;
} PCIE_ATU_LWR_BASE_BITS;


typedef union PCIeATULwrBase
{
    PCIE_ATU_LWR_BASE_BITS  Bits;
    UINT32                  Raw;
} PCIE_ATU_LWR_BASE;


typedef struct PCIeATULimitAddrBits
{
    UINT32 LIMIT_ADDR_HW:12;
    UINT32 LIMIT_ADDR_RW:20;
} PCIE_ATU_LIMIT_ADDR_BITS;

typedef union PCIeATULimitAddr
{
    PCIE_ATU_LIMIT_ADDR_BITS Bits;
    UINT32                   Raw;
} PCIE_ATU_LIMIT_ADDR;

typedef struct PCIeATULwrTargetAddrBits
{
    UINT32 LWR_TARGET_ADDR_HW:12;
    UINT32 LWR_TARGET_ADDR_RW:20;
} PCIE_ATU_LWR_TARGET_ADDR_BITS;

typedef union PCIeATULwrTargetAddr
{
    PCIE_ATU_LWR_TARGET_ADDR_BITS Bits;
    UINT32                        Raw;
} PCIE_ATU_LWR_TARGET_ADDR;


typedef struct PCIeATURegionCtrl3Bits
{
    UINT32 VF_NUMBER:1;
    UINT32 RSVDP_1:30;
    UINT32 VF_ACTIVE:1;
} PCIE_ATU_REGION_CTRL3_BITS;


typedef union PCIeATURegionCtrl3
{
    PCIE_ATU_REGION_CTRL3_BITS Bits;
    UINT32                     Raw;
} PCIE_ATU_REGION_CTRL3;



// Full register set definition
//
typedef struct PCIeATURegisters
{
    PCIE_ATU_VIEWPORT           VIEWPORT;               // 0x900
    PCIE_ATU_REGION_CTRL1       REGION_CTRL1;           // 0x904
    PCIE_ATU_REGION_CTRL2       REGION_CTRL2;           // 0x908
    PCIE_ATU_LWR_BASE           LWR_BASE;               // 0x90C
    UINT32                      UPPER_BASE;             // 0x910
    PCIE_ATU_LIMIT_ADDR         LIMIT_ADDR;             // 0x914
    PCIE_ATU_LWR_TARGET_ADDR    LWR_TARGET_ADDR;        // 0x918
    UINT32                      UPPER_TARGET_ADDR;      // 0x91C
    PCIE_ATU_REGION_CTRL3       REGION_CTRL3;           // 0x920

} PCIE_ATU_REGISTERS, *PPCIE_ATU_REGISTERS;

//C_ASSERT(sizeof(PCIE_ATU_VIEWPORT) == sizeof(UINT32));
//C_ASSERT(sizeof(PCIE_ATU_REGION_CTRL1) == sizeof(UINT32));
//C_ASSERT(sizeof(PCIE_ATU_REGION_CTRL2) == sizeof(UINT32));
//C_ASSERT(sizeof(PCIE_ATU_LWR_BASE) == sizeof(UINT32));
//C_ASSERT(sizeof(PCIE_ATU_LIMIT_ADDR) == sizeof(UINT32));
//C_ASSERT(sizeof(PCIE_ATU_LWR_TARGET_ADDR) == sizeof(UINT32));
//C_ASSERT(sizeof(PCIE_ATU_REGION_CTRL3) == sizeof(UINT32));
//C_ASSERT(sizeof(PCIE_ATU_REGISTERS) == (9 * sizeof(UINT32)));

