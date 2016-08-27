/*-------------------------------------------------------
 |
 |    SBOOT.S
 |
 |    1BL for standalone boot.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2015  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

    .equiv  SRAM_RESET_VECTOR,         0x00100000

.altmacro
#include "spi.inc"

    .begin  no-absolute-literals
    .section .StandaloneBootVector.text, "ax"

    .align    4
    .global    _StandaloneBootVector
_StandaloneBootVector:
    j          _StandaloneBootHandler
    .size    _StandaloneBootVector, . - _StandaloneBootVector
    
    .align  4
    .literal_position
    
    .align  4
    .global         _StandaloneBootHandler
_StandaloneBootHandler:
    
    // Set GPIOs to hardware control
    REG_RMW_BITS    a0, a1, a2, a3, HUP_CHIP_GPIO15_GPIOAFSEL_ADDRESS, HUP_GPIO_15_GPIOAFSEL_DATA, HUP_GPIO_GPIOAFSEL_AFSEL_FIELD_MASK
    
    // Enable receiver on GPIO125
    REG_RMW_BITS    a0, a1, a2, a3, HUP_CHIP_POR_GPIO_IE_GP127_GP96_ADDRESS, HUP_POR_GPIO_IE3_DATA, HUP_POR_GPIO_IE3_MASK
    
    // Enable GPIO register overrides
    REG_RMW_BITS    a0, a1, a2, a3, HUP_CHIP_POR_GPIO_REG_OVERRIDE_ADDRESS, HUP_POR_GPIO_REG_OVERRIDE_DATA, HUP_POR_GPIO_REG_OVERRIDE_ENABLE_FIELD_MASK
    
    // Configure clock 
    REG_RMW_BITS    a0, a1, a2, a3, HUP_CHIP_POR_SPI_DIV16_ADDRESS, HUP_POR_SPI_DIV_DATA, HUP_POR_SPI_DIV16_SEL_FIELD_MASK
    
    // Disable while configuring
    movi            a0, HUP_CHIP_SPI5_SSIENR_ADDRESS
    movi            a1, 0
    s32i            a1, a0, 0
    
    // Barrier after the disable
    memw
    
    // Configure CTRLR0
    movi            a0, HUP_CHIP_SPI5_CTRLR0_ADDRESS
    movi            a1, SPI_REGS_CTRLR0_DATA
    s32i            a1, a0, 0
    
    // Set up BAUD register
    movi            a0, HUP_CHIP_SPI5_BAUDR_ADDRESS
    movi            a1, SPI_REGS_BAUD_DATA
    s32i            a1, a0, 0
    
    // Read data till idle
    SPI_IDLE        a4, a5, a6, a7
    
    // Barrier before the enable
    memw
    
    // Reenable
    movi            a0, HUP_CHIP_SPI5_SSIENR_ADDRESS
    movi            a1, 1
    s32i            a1, a0, 0
    
    // Barrier after the enable
    memw
    
    // Read the header in flash (12 bytes)
    movi            a0, SPI_FLASH_READ_HDR_CMD
    movi            a1, SPI_FLASH_HEADER_SIZE
    SPI_TRANSFER    a0, a1, a2, a3, a4, a5
    
    // Address in Flash where data is stored
    SPI_RECV32      a0, a4, a5, a6, a7
    // Address in memory to copy to
    SPI_RECV32      a1, a4, a5, a6, a7
    // Size of data (in bytes)
    SPI_RECV32      a2, a4, a5, a6, a7
    
    SPI_IDLE        a4, a5, a6, a7
    
    // Construct second read command.
    // Bits 8-31 are the Flash address as read
    // from the header. Need to reverse
    // it as Flash protocol requires it in big-endian
    srli            a3, a0, 0
    movi            a8, SPI_FRAME_MASK
    and             a9, a3, a8
    
    srli            a3, a3, 8
    movi            a8, SPI_FRAME_MASK
    and             a8, a3, a8
    slli            a9, a9, 8
    or              a9, a8, a9
    
    srli            a3, a3, 8
    movi            a8, SPI_FRAME_MASK
    and             a8, a3, a8
    slli            a9, a9, 8
    or              a9, a8, a9
    
    // a3 == <read cmd> | (<flash address> << 8)
    slli            a3, a9, 8
    movi            a4, SPI_FLASH_READ_CMD
    or              a3, a3, a4
    
    // Do next transfer. Size is taken from
    // the 'byte size' we read from Flash
    SPI_TRANSFER    a3, a2, a4, a5, a6, a7
    
    // Convert size in bytes to size in DWORDs (round down).
    // Store the remainder in a0 for later
    movi            a0, 0x3
    and             a0, a0, a2
    srli            a2, a2, 2
    
    // Loop over the data, copying it from
    // Flash to the target memory address.
.recv_loop:
    SPI_RECV32      a3, a4, a5, a6, a7
    s32i            a3, a1, 0
    addi            a2, a2, -1
    addi            a1, a1, 4
    bnez            a2, .recv_loop
    
    beqz            a0, .recv_loop_end
    // Copy any leftover, byte by byte
.recv_loop2:
    SPI_RECV8       a3, a4, a5, a6, a7
    s8i             a3, a1, 0
    addi            a1, a1, 1
    addi            a0, a0, -1
    bnez            a0, .recv_loop2
    
.recv_loop_end:
    
    // Undo register configuration that we did earlier
    // before moving on to 2BL
    
    // Barrier before disabling SPI
    memw
    
    // Disable SPI5
    movi            a0, HUP_CHIP_SPI5_SSIENR_ADDRESS
    movi            a1, 0
    s32i            a1, a0, 0
    
    // Revert SPI clock divider
    REG_RMW_BITS    a0, a1, a2, a3, HUP_CHIP_POR_SPI_DIV16_ADDRESS, 0, HUP_POR_SPI_DIV16_SEL_FIELD_MASK
    
    // Disable GPIO register overrides
    REG_RMW_BITS    a0, a1, a2, a3, HUP_CHIP_POR_GPIO_REG_OVERRIDE_ADDRESS, 0, HUP_POR_GPIO_REG_OVERRIDE_ENABLE_FIELD_MASK
    
    // Disable receiver on GPIO125
    REG_RMW_BITS    a0, a1, a2, a3, HUP_CHIP_POR_GPIO_IE_GP127_GP96_ADDRESS, 0, HUP_POR_GPIO_IE3_MASK
    
    // Revert GPIOs to software control
    REG_RMW_BITS    a0, a1, a2, a3, HUP_CHIP_GPIO15_GPIOAFSEL_ADDRESS, 0, HUP_GPIO_GPIOAFSEL_AFSEL_FIELD_MASK
    
    // Jump to 2BL
    movi            a0, SRAM_RESET_VECTOR
    l32i            a0, a0, 0
    jx              a0
    
    .size   _StandaloneBootHandler, . - _StandaloneBootHandler
    .end    no-absolute-literals

