/*-------------------------------------------------------
 |
 |      bifrost_cfg_utils.h
 |
 |      Contains all enums, typedef primitives, and
 |      defined constants for reader/writing x86 cpu
 |      configuration registers
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2011  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
*/

#ifndef _BIFROST_CFG_UTILS_H
#define _BIFROST_CFG_UTILS_H

#define PCICFG_LEGACY_ADDR 0xCF8
#define PCICFG_LEGACY_DATA 0xCFC
#define MSR_MMCFG 0xC0010058
#define MSR_PSTATE_CNTL 0xC0010062
#define MSR_PSTATE_STATUS 0xC0010063
#define MSR_PSTATE_0 0xC0010064
#define MSR_PSTATE_1 0xC0010065
#define MSR_PSTATE_2 0xC0010066
#define MSR_PSTATE_3 0xC0010067
#define MSR_PSTATE_4 0xC0010068
#define MSR_PSTATE_5 0xC0010069
#define MSR_PSTATE_6 0xC001006A
#define MSR_PSTATE_7 0xC001006B

#define MMCFG_VAL 0xf8000009
#define LEGACY_PORT_BASE_ADDRESS 0xFDFC000000ULL

// Define COM port info
#define COM1_ADDR 0x3f8

#define COM_DATA_REGISTER 0x0000
#define COM_INTERRUPT_ENABLE_REGISTER 0x0001
#define COM_INTERRUPT_IDENTIFICATION_REGISTER 0x0002
#define COM_FIFO_CONTROL_REGISTER 0x0002
#define COM_LINE_CONTROL_REGISTER 0x0003
#define COM_MODEM_CONTROL_REGISTER 0x0004
#define COM_LINE_STATUS_REGISTER 0x0005
#define COM_MODEM_STATUS_REGISTER 0x0006
#define COM_SCRATCH_REGISTER 0x0007

#define COM_DIVISOR_LATCH_REGISTER_LOW 0x0000
#define COM_DIVISOR_LATCH_REGISTER_HIGH 0x0001

#define COM_CLOCK_RATE 0x1C200

#define COM_LINE_CONTROL_8_BITS_1_STOP 0x03
#define COM_LINE_CONTROL_DIVISOR_ACCESS 0x80

#define COM_MODEM_CONTROL_DATA_TERMINAL_READY 0x01
#define COM_MODEM_CONTROL_REQUEST_TO_SEND 0x02

#define COM_LINE_STATUS_DATA_READY 0x01
#define COM_LINE_STATUS_OVERRUN_ERROR 0x02
#define COM_LINE_STATUS_PARITY_ERROR 0x04
#define COM_LINE_STATUS_FRAMING_ERROR 0x08
#define COM_LINE_STATUS_SEND_BUFFER_EMPTY 0x20

// Defines for APIC registers
#define MSR_APIC_BASE 0x1B
#define MSR_APIC_BASE_ENABLE (1<<11)
#define MSR_APIC_BASE_ADDRESS 0xFEE00000
#define APIC_SIR 0xF0
#define APIC_ID 0x20
#define APIC_VERSION 0x30
#define APIC_TPR 0x80
#define APIC_APR 0x90
#define APIC_PPR 0xA0
#define APIC_EOI 0xB0     // APIC End of Interrupt
#define APIC_ICR_LO 0x300
#define APIC_ICR_HI 0x310
#define APIC_IRR0 0x200   // APIC Interrupt Request Registers
#define APIC_IRR1 0x210
#define APIC_IRR2 0x220
#define APIC_IRR3 0x230
#define APIC_IRR4 0x240
#define APIC_IRR5 0x250
#define APIC_IRR6 0x260
#define APIC_IRR7 0x270
#define APIC_ISR0 0x100   // APIC Interrupt Service Registers
#define APIC_ISR1 0x110
#define APIC_ISR2 0x120
#define APIC_ISR3 0x130
#define APIC_ISR4 0x140
#define APIC_ISR5 0x150
#define APIC_ISR6 0x160
#define APIC_ISR7 0x170
#define APIC_DCR 0x3E0    // APIC Divide Configuration Register
#define APIC_TLVTR 0x320  // APIC Timer Local Vector Table Register
#define APIC_TICR 0x380   // APIC Timer Initial Count Register
#define APIC_TCCR 0x390   // APIC Timer Current Count Register
#define APIC_EAFR 0x400
#define APIC_EAC 0x410
#define APIC_SEOI 0x420   // APIC Specific End of Interrupt
#define APIC_IER0 0x480   // APIC Interrupt Enable Registers
#define APIC_IER1 0x490
#define APIC_IER2 0x4A0
#define APIC_IER3 0x4B0
#define APIC_IER4 0x4C0
#define APIC_IER5 0x4D0
#define APIC_IER6 0x4E0
#define APIC_IER7 0x4F0

// Bit offsets
#define APIC_TLVTR_TMM (1 << 17)          // Set periodic interrupt mode in Timer Local Vector Table Register
#define APIC_TLVTR_M (1 << 16)            // Disables APIC timer interrupts in Timer Local Vector Table Register
#define APIC_TLVTR_TGM (1 << 15)          // Set trigger mode to level-sensitive in Timer Local Vector Table Register
#define APIC_SIR_SOFTWARE_ENABLE (1 << 8) // Set software enable in Spurious Interrupt Register


EXTERN_C VOID set_cr3(UINT64 val);
EXTERN_C UINT64 get_cr3();
EXTERN_C VOID set_cr2(UINT64 val);
EXTERN_C UINT64 get_cr2();
EXTERN_C VOID set_cr4(UINT64 val);
EXTERN_C UINT64 get_cr4();
EXTERN_C VOID set_cr0(UINT64 val);
EXTERN_C UINT64 get_cr0();
EXTERN_C VOID set_dr7(UINT64 val);
EXTERN_C UINT64 get_dr7();
EXTERN_C VOID set_dr6(UINT64 val);
EXTERN_C UINT64 get_dr6();
EXTERN_C VOID set_rflags(UINT64 val);
EXTERN_C UINT64 get_rflags();

EXTERN_C UINT64 readmsr(UINT64 reg);
EXTERN_C VOID writemsr(UINT64 reg, UINT64 val);

EXTERN_C UINT64 getrsp();

EXTERN_C UINT8 ReadPort8(int port);
EXTERN_C VOID WritePort8(int port, UINT8 val);
EXTERN_C UINT32 ReadPort32(int port);
EXTERN_C VOID WritePort32(int port, UINT32 val);

EXTERN_C UINT32 *PciCfgPtr(int bus, int dev, int func, int reg);

EXTERN_C VOID PciCfgWrite32Raw(UINT32 bdf_addr, UINT32 val);
EXTERN_C UINT32 PciCfgRead32Raw(UINT32 bdf_addr);
EXTERN_C VOID PciLegacyCfgWriteRaw(UINT32 bdf_addr, UINT32 val);
EXTERN_C UINT32 PciLegacyCfgReadRaw(UINT32 bdf_addr);

EXTERN_C VOID PciCfgWrite32Indirect(UINT32 bdf_addr, UINT32 reg, int val);
EXTERN_C UINT32 PciCfgRead32Indirect(UINT32 bdf_addr, UINT32 reg);

EXTERN_C VOID PciLegacyCfgWrite(int bus, int dev, int func, int reg, UINT32 val);
EXTERN_C UINT32 PciLegacyCfgRead(int bus, int dev, int func, int reg);
EXTERN_C VOID PciCfgWrite32(int bus, int dev, int func, int reg, UINT32 val);
EXTERN_C UINT32 PciCfgRead32(int bus, int dev, int func, int reg);
EXTERN_C VOID PciCfgWrite16(int bus, int dev, int func, int reg, UINT16 val);
EXTERN_C UINT16 PciCfgRead16(int bus, int dev, int func, int reg);
EXTERN_C VOID PciDumpCfg(int bus, int dev, int func);

EXTERN_C VOID WriteAPIC(int reg, UINT32 val);
EXTERN_C UINT32 ReadAPIC(int reg);

#endif
