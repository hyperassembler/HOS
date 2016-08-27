/*-------------------------------------------------------
 |
 |    spi.inc
 |
 |    routines for SPI operation.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2015  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#include "sboot.inc"

    // From hup_chip.h
    // gpio_regs[15].gpioafsel.afsel
    // Set GPIO 123-127 to hardware control
    .equiv  HUP_GPIO_15_GPIOAFSEL_DATA,     0xF8
    
    // From hup_chip.h
    // por_regs.GPIO_IE_GP127_GP96 GPIO_IE_GP127_GP96
    // Enable receiver on GPIO125
    .equiv  HUP_POR_GPIO_IE3_DATA,          0x20000000
    .equiv  HUP_POR_GPIO_IE3_MASK,          0x20000000
    
    // From hup_chip.h
    // por_regs.GPIO_reg_override
    // Enable register overrides
    .equiv  HUP_POR_GPIO_REG_OVERRIDE_DATA, 0x1
    
    // From hup_chip.h
    // por_regs.spi_div16
    // set divider to 1/N
    .equiv  HUP_POR_SPI_DIV_DATA,           0xF
    
    // SCPH:   0x1 (clock toggles at start of first data bit)
    // SCPOL:  0x1 (inactive state is high) 
    // TMOD:   0x3 (EEPROM read)
    // DFS_32: 0x7 (8 bit frame size)
    .equiv  SPI_REGS_CTRLR0_DATA,           0x000703C0
    // Always use slave line 0
    .equiv  SPI_REGS_SER_DATA_LO,           0x00000000
    .equiv  SPI_REGS_SER_DATA_HI,           0x00000001
    .equiv  SPI_REGS_BAUD_DATA,             0x00000002
    
    // From Synopsys databook: https://iebhwdev/sites/Silicon/Makena/Shared%20Documents/Vendor/Synopsys/Databooks/LowSpeedIO/dw_apb_ssi_db_4p00a.pdf
    .equiv  SPI_STS_BSY_BIT,                0
    // Transmit FIFO Not Full
    .equiv  SPI_STS_TNF_BIT,                1
    // Transmit FIFO Empty
    .equiv  SPI_STS_TFE_BIT,                2
    // Receive FIFO Not Empty
    .equiv  SPI_STS_RNE_BIT,                3
    // Receive FIFO Full
    .equiv  SPI_STS_RFF_BIT,                4
    
    .equiv  SPI_FLASH_READ_CMD,             0x03
    .equiv  SPI_FLASH_READ_HDR_CMD,         (0x000000 << 8) | SPI_FLASH_READ_CMD
    .equiv  SPI_FLASH_HEADER_SIZE,          12
    
    .equiv  SPI_FRAME_MASK,                 0x000000FF
    
    // Shift loop counter. Starts at 32 and decrements
    // because Tensilica left shifting instruction shifts
    // by 32 - <reg value>
    .equiv READ_SHIFT_LOOP_START,           32
    .equiv READ_SHIFT_LOOP_END,             0
    .equiv READ_SHIFT_LOOP_INCR,            -8
    
    
    // Stall until the SPI is idle.
    // Arguments:
    // pSr:    Value will be clobbered. Used to point to the status register
    // pDr:    Value will be clobbered. Used to point to the data register
    // SrData: Value will be clobbered. Used to store value read from status register
    // DrData: Value will be clobbered. Used to store value read from data register
    .macro  SPI_IDLE pSr, pDr, SrData, DrData
    LOCAL .WaitWhileBusy
    LOCAL .RxFifoEmpty
    
    movi            \pSr, HUP_CHIP_SPI5_SR_ADDRESS
    movi            \pDr, HUP_CHIP_SPI5_DR0_ADDRESS
.WaitWhileBusy:
    // Read status register
    l32i            \SrData, \pSr, 0
    // Branch down if Receive FIFO Not Empty flag not set
    bbci            \SrData, SPI_STS_RNE_BIT, .RxFifoEmpty
    // Otherwise read from the data register
    l32i            \DrData, \pDr, 0
.RxFifoEmpty:
    // Branch back if Transmit FIFO Empty flag not set
    bbci            \SrData, SPI_STS_TFE_BIT, .WaitWhileBusy
    // Branch back if Busy flag is set
    bbsi            \SrData, SPI_STS_BSY_BIT, .WaitWhileBusy
    .endm
    
    
    // Read/modify/write a register
    // Arguments:
    // Temp: Value will be clobbered. Used to store temporary values
    // ReadValue: Value will be clobbered. Used to store the value read from the register
    // AddrReg: Value will be clobbered. Used to store the address value
    // MaskReg: Value will be clobbered. Used to store the mask value
    // Addr: address of the register
    // Data: data to write
    // Mask: data to write
    .macro  REG_RMW_BITS Temp, ReadValue, AddrReg, MaskReg, Addr, Data, Mask
    movi            \MaskReg, \Mask
    movi            \AddrReg, \Addr
    // Read register value
    l32i            \ReadValue, \AddrReg, 0
    // Invert mask and use it to zero
    // out the relevant bits in the reg value
    movi            \Temp, -1
    xor             \Temp, \Temp, \MaskReg
    and             \ReadValue, \Temp, \ReadValue
    // Mask the data and OR it with the
    // remaining reg value
    movi            \Temp, \Data
    and             \Temp, \Temp, \MaskReg 
    or              \ReadValue, \Temp, \ReadValue
    // Write the register
    s32i            \ReadValue, \AddrReg, 0
    .endm
    
    
    // Read 32 bits from the SPI data register
    // Arguments:
    // Ret:     The register in which to store the return value
    // pSr:     Value will be clobbered. Used to point to the status register
    // pDr:     Value will be clobbered. Used to point to the data register
    // RegData: Value will be clobbered. Used to store value read from registers
    // Ctr:     Value will be clobbered. Used as a counter for the read loop
    .macro SPI_RECV32 Ret, pSr, pDr, RegData, Ctr
    LOCAL .CheckRxAvail
    
    movi            \pSr, HUP_CHIP_SPI5_SR_ADDRESS
    movi            \pDr, HUP_CHIP_SPI5_DR0_ADDRESS
    
    // Zero out the return register
    movi            \Ret, 0
    // Shift loop counter.
    movi            \Ctr, READ_SHIFT_LOOP_START
.CheckRxAvail:
    // Read status register
    l32i            \RegData, \pSr, 0
    // Branch back if Receive FIFO Not Empty flag not set (meaning Receive FIFO is empty)
    bbci            \RegData, SPI_STS_RNE_BIT, .CheckRxAvail
    // Read from Receive FIFO
    l32i            \RegData, \pDr, 0
    // Shift left as necessary and OR
    // into the return register
    wsr.sar         \Ctr
    sll             \RegData, \RegData
    or              \Ret, \RegData, \Ret
    
    addi            \Ctr, \Ctr, READ_SHIFT_LOOP_INCR
    // If not done, branch back and read another byte
    bnei            \Ctr, READ_SHIFT_LOOP_END, .CheckRxAvail
    .endm
    
    
    // Read 8 bits from the SPI data register
    // Arguments:
    // Ret:     The register in which to store the return value
    // pSr:     Value will be clobbered. Used to point to the status register
    // pDr:     Value will be clobbered. Used to point to the data register
    // RegData: Value will be clobbered. Used to store value read from registers
    // Ctr:     Unused. Added to maintain same footprint as SPI_RECV32
    .macro SPI_RECV8 Ret, pSr, pDr, RegData, Ctr
    LOCAL .CheckRxAvail
    
    movi            \pSr, HUP_CHIP_SPI5_SR_ADDRESS
    movi            \pDr, HUP_CHIP_SPI5_DR0_ADDRESS
    
.CheckRxAvail:
    // Read status register
    l32i            \RegData, \pSr, 0
    // Branch back if Receive FIFO Not Empty flag not set (meaning Receive FIFO is empty)
    bbci            \RegData, SPI_STS_RNE_BIT, .CheckRxAvail
    // Read from Receive FIFO
    l32i            \Ret, \pDr, 0
    
    .endm
    
    // Initiate a Flash transfer by writing a
    // command over SPI.
    // Arguments:
    // Cmd:     The register containing the command to write
    // Cnt:     The number of bytes to be transferred
    // RegAddr: Value will be clobbered. Used to store addresses of various registers
    // RegData: Value will be clobbered. Used to store data which is written to or read from registers
    // TempCmd: Value will be clobbered. Stores intermediate values of the command as it is serialized
    // Ctr:     Value will be clobbered. Used as a counter for the write loop
    .macro SPI_TRANSFER Cmd, Cnt, RegAddr, RegData, TempCmd, Ctr
    LOCAL .WaitTxAvail
    LOCAL .EndTx
    
    // Barrier before disable
    memw
    
    // Disable while configuring CTRLR1
    movi            \RegAddr, HUP_CHIP_SPI5_SSIENR_ADDRESS
    //movi            \RegAddr, SPI_REGS_SSIENR_ADDR
    movi            \RegData, 0
    s32i            \RegData, \RegAddr, 0
    
    // Barrier after disable
    memw
    
    // Set up CTRLR1 with the number of bytes to be written
    movi            \RegAddr, HUP_CHIP_SPI5_CTRLR1_ADDRESS
    addi            \RegData, \Cnt, -1
    s32i            \RegData, \RegAddr, 0
    
    // Disable slave select until
    // we've finished writing the command
    // into the TXFIFO
    movi            \RegAddr, HUP_CHIP_SPI5_SER_ADDRESS
    movi            \RegData, SPI_REGS_SER_DATA_LO
    s32i            \RegData, \RegAddr, 0
    
    // Barrier before enable
    memw
    
    // Re-enable
    movi            \RegAddr, HUP_CHIP_SPI5_SSIENR_ADDRESS
    movi            \RegData, 1
    s32i            \RegData, \RegAddr, 0
    
    // Barrier after enable
    memw
    
    // Store off the command value to a temp register
    mov             \TempCmd, \Cmd
    movi            \Ctr, 0
.WaitTxAvail:
    // Read status register into RegData
    //movi            \RegAddr, SPI_REGS_SSPSR_ADDR
    movi            \RegAddr, HUP_CHIP_SPI5_SR_ADDRESS
    l32i            \RegData, \RegAddr, 0
    // If Transmit FIFO Not Full flag is not set (meaning FIFO is full), branch back
    bbci            \RegData, SPI_STS_TNF_BIT, .WaitTxAvail
    // Mask data for first byte
    movi            \RegData, SPI_FRAME_MASK
    and             \RegData, \RegData, \TempCmd
    // Write to data register
    //movi            \RegAddr, SPI_REGS_SSPDR_ADDR
    movi            \RegAddr, HUP_CHIP_SPI5_DR0_ADDRESS
    s32i            \RegData, \RegAddr, 0
    // Barrier to ensure bytes arrive
    // at the SPI in order
    memw
    // Shift data for next byte
    srli            \TempCmd, \TempCmd, 8
    
    // As long as there is data left to send, branch back and transmit next byte
    addi            \Ctr, \Ctr, 8
    blti            \Ctr, 32, .WaitTxAvail
    
    // Barrier before reenabling slave select
    memw
    
    // Reenable slave select to actually begin the transfer
    movi            \RegAddr, HUP_CHIP_SPI5_SER_ADDRESS
    movi            \RegData, SPI_REGS_SER_DATA_HI
    s32i            \RegData, \RegAddr, 0
    
    // Barrier after reenabling slave select
    memw
    
    .endm

