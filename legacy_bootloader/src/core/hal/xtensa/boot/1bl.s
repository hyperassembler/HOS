/*-------------------------------------------------------
 |
 |    1BL.S
 |
 |    1BL for 'xtensa' architecture.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2014  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */
.equiv  PCIE_RESET_RELEASE_ADDR,   0x04080090
.equiv  PCIE_RESET_RELEASE_DATA,   0x00000003
.equiv  INTC_P0_0_CLEAR_ADDR,      0x04000008
.equiv  INTC_P0_0_CLEAR_BIT,       0
.equiv  INTC_P0_0_CLEAR_BIT_MASK,  0x1
.equiv  INTC_P1_0_CLEAR_ADDR,      0x04000088
.equiv  INTC_P1_0_CLEAR_BIT,       1
.equiv  DRAM_RESET_TABLE_BASE,     0xA1000000
#ifdef USE_ALT_VECTOR
.equiv  SRAM_RESET_VECTOR,         0x00100020
#else
.equiv  SRAM_RESET_VECTOR,         0x00100000
#endif
.equiv  SBOOT_RESET_VECTOR,        0x07108000

.equiv  SCRPAD0_ADDR,              0x02000030
.equiv  SCRPAD0_DATA_BIT,          1

    .begin  no-absolute-literals
    .section .SharedResetVector.text, "ax"

    .align  4
    .global         _SharedResetVector
_SharedResetVector:
    j               .SharedResetHandler

    .align  4
    .literal_position

    .align  4
.SharedResetHandler:
    rsr.prid        a0
    bltui           a0, 2, .ControlNode

.CoreN:
    movi            a1, DRAM_RESET_TABLE_BASE
    addx4           a1, a0, a1
    l32i            a1, a1, 0
    jx              a1

.ControlNode:
    beqi            a0, 0, .Core0   // dispatch for core0

.Core1:
#ifndef L2BOOT
    movi            a1, INTC_P1_0_CLEAR_ADDR
.Poll:
    l32ai           a2, a1, 0
    bbci            a2, INTC_P1_0_CLEAR_BIT, .Poll
    s32i            a2, a1, 0
#endif
    j               .CoreN

.Core0:
#ifndef L2BOOT
    movi            a0, PCIE_RESET_RELEASE_ADDR
    movi            a1, PCIE_RESET_RELEASE_DATA
    l32i            a2, a0, 0
    beq             a1, a2, .PCIE_Initialized
    s32i            a1, a0, 0
.PCIE_Initialized:
#ifdef PCIE_SYNC 
    movi            a1, INTC_P0_0_CLEAR_ADDR
.Poll_2bl:
    l32ai           a2, a1, 0
    bbci            a2, INTC_P0_0_CLEAR_BIT, .Poll_2bl
    s32i            a2, a1, 0
#endif

#ifdef SBOOT
    movi            a0, SBOOT_RESET_VECTOR
#else
    movi            a0, SRAM_RESET_VECTOR
    l32i            a0, a0, 0
#endif    
    jx              a0
#else // L2BOOT
    movi            a0, DRAM_RESET_TABLE_BASE
    l32i            a0, a0, 0
    jx              a0
#endif

    .size   _SharedResetVector, . - _SharedResetVector
    .end    no-absolute-literals

