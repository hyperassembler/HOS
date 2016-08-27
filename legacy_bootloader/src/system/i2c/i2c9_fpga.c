/*-----------------------------------------------------
 |
 |      i2c9_fpga.c
 |
 |     Read and write an external register 
 |     (in the FPGA) through the I2C9 module.
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2014  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

#include "bifrost.h"
#include "tsb_regs.h"
#include "i2c0_reg_def.h"
#include "por_reg_def.h"

#define EXT_CN_DET      0x08
#define FPGA_MODE       0x10
#define SYS_LED_OVR     0x14
#define SYS_LED_DATA    0x15
#define LED_OVR_0       0x16
#define LED_DATA_0      0x17
#define LED_OVR_1       0x18
#define LED_DATA_1      0x19
#define SEG7_LED_OVR    0x1A
#define SEG7_LED_DATA_0 0x1B
#define SEG7_LED_DATA_1 0x1C
#define SEG7_LED_DATA_2 0x1D
#define SEG7_LED_DATA_3 0x1E
#define I2C_BUF_EN      0x20
#define EXT_PW_EN       0x21

#define I2C_INSTANCE_CPLB   8
#define I2C_INSTANCE_LHNA   9
#define BUS_SPEED           4
#define I2C_SLAVE_ADDR_CPLB 0x3A
#define I2C_SLAVE_ADDR_LHNA 0x3B

static volatile BOOL32 g_FPGA_accessible = FALSE;
static volatile BOOL32 g_FPGA_accessible_checked = FALSE;


//====================================================================================================//
//  Functions to access FPGA Registers
//====================================================================================================//
//
// Function to initialize I2C port
//
BOOL hw_i2c_init_port(UINT8 i2cInstance, UINT8 busSpeed)
{
    HW_RESULT ret = FALSE;
#if (defined __XTENSA__)
    UINT8 speedCode;
    UINT32 rdata, wdata, dll, dlh, i, temp, intrNum, wmask;
    
    //IO Mux Configuration
    //Make sure I2C function is selected for the corresponding GPIO pins
    //FIXME: May need to change the drive strength from default value; right now leaving it at default 4mA.
    //FIXME: May need to change the slew rate control; right now leaving it at default slow.
    //Odd numbered GPIOs are RX pins; hence REN should be set to '1'.
    
    //Set the register to override the efuse values for the PU/PD
    //hw_reg_write32(LAHAINA_POR_BASE, POR_GPIO_REG_OVERRIDE_OFS, 0x1);

    switch (i2cInstance) {
        case 0:
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_PINSHARE0_OFS, POR_GPIO_PINSHARE0_GPIO8_9_MASK, POR_GPIO_PINSHARE0_GPIO8_9_MASK);
            wdata = POR_GPIO_REN0_REN_GPIO8_MASK | POR_GPIO_REN0_REN_GPIO9_MASK;
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_REN0_OFS, wdata, wdata);
            break;
        case 1:
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_PINSHARE0_OFS, POR_GPIO_PINSHARE0_GPIO10_11_MASK, POR_GPIO_PINSHARE0_GPIO10_11_MASK);
            wdata = POR_GPIO_REN0_REN_GPIO10_MASK | POR_GPIO_REN0_REN_GPIO11_MASK;
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_REN0_OFS, wdata, wdata);
            break;
        case 2:
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_PINSHARE0_OFS, POR_GPIO_PINSHARE0_GPIO12_13_MASK, POR_GPIO_PINSHARE0_GPIO12_13_MASK);
            wdata = POR_GPIO_REN0_REN_GPIO12_MASK | POR_GPIO_REN0_REN_GPIO13_MASK;
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_REN0_OFS, wdata, wdata);
            break;
        case 3:
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_PINSHARE0_OFS, POR_GPIO_PINSHARE0_GPIO14_15_MASK, POR_GPIO_PINSHARE0_GPIO14_15_MASK);
            wdata = POR_GPIO_REN0_REN_GPIO14_MASK | POR_GPIO_REN0_REN_GPIO15_MASK;
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_REN0_OFS, wdata, wdata);
            break;
        case 4:
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_PINSHARE0_OFS, POR_GPIO_PINSHARE0_GPIO16_17_MASK, POR_GPIO_PINSHARE0_GPIO16_17_MASK);
            wdata = POR_GPIO_REN0_REN_GPIO16_MASK | POR_GPIO_REN0_REN_GPIO17_MASK;
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_REN0_OFS, wdata, wdata);
            break;
        case 5:
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_PINSHARE0_OFS, POR_GPIO_PINSHARE0_GPIO18_19_MASK, POR_GPIO_PINSHARE0_GPIO18_19_MASK);
            wdata = POR_GPIO_REN0_REN_GPIO18_MASK | POR_GPIO_REN0_REN_GPIO19_MASK;
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_REN0_OFS, wdata, wdata);
            break;
        case 6:
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_PINSHARE0_OFS, POR_GPIO_PINSHARE0_GPIO20_21_MASK, POR_GPIO_PINSHARE0_GPIO20_21_MASK);
            wdata = POR_GPIO_REN0_REN_GPIO20_MASK | POR_GPIO_REN0_REN_GPIO21_MASK;
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_REN0_OFS, wdata, wdata);
            break;
        case 7:
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_PINSHARE0_OFS, POR_GPIO_PINSHARE0_GPIO22_23_MASK, POR_GPIO_PINSHARE0_GPIO22_23_MASK);
            wdata = POR_GPIO_REN0_REN_GPIO22_MASK | POR_GPIO_REN0_REN_GPIO23_MASK;
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_REN0_OFS, wdata, wdata);
            break;
        case 8:
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_PINSHARE0_OFS, POR_GPIO_PINSHARE0_GPIO24_25_MASK, POR_GPIO_PINSHARE0_GPIO24_25_MASK);
            wdata = POR_GPIO_REN0_REN_GPIO24_MASK | POR_GPIO_REN0_REN_GPIO25_MASK;
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_REN0_OFS, wdata, wdata);
            break;
        case 9:
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_PINSHARE0_OFS, POR_GPIO_PINSHARE0_GPIO26_27_MASK, POR_GPIO_PINSHARE0_GPIO26_27_MASK);
            wdata = POR_GPIO_REN0_REN_GPIO26_MASK | POR_GPIO_REN0_REN_GPIO27_MASK;
            hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_REN0_OFS, wdata, wdata);
            break;
    }

    //Write 0 to IC_ENABLE to disable i2c
    hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_ENABLE_OFS, 0x0);

    //Make sure the I2C got disabled indeed.
    rdata = hw_reg_read32(I2C_REG_BASE(i2cInstance), I2C0_IC_ENABLE_STATUS_OFS);
    if (rdata & I2C0_IC_ENABLE_STATUS_IC_EN_MASK)
    {
        hw_errmsg("IC_ENABLE_STATUS of I2C%d is still active; bad test code\n", i2cInstance);
        return ret;
    }

    //IC_CON
    if (busSpeed > 2) speedCode = busSpeed-2;
    else speedCode = busSpeed;
    wdata = 0x0 | I2C0_IC_CON_IC_SLAVE_DISABLE_MASK |
            I2C0_IC_CON_IC_RESTART_EN_MASK    |
            (speedCode << I2C0_IC_CON_SPEED_SHIFT) |
            1 << 9 |
            I2C0_IC_CON_MASTER_MODE_MASK;
    hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_CON_OFS, wdata);

    //SDA_HOLD
    hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_SDA_HOLD_OFS, 0x40004);

    //SDA_SETUP
    hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_SDA_SETUP_OFS, 0x2);

    //TAR
    //wdata = 0x0 | (pCfg->defTargetAddr & 0x3ff);
    //hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_TAR_OFS, wdata);

    //Spike Suppression
    // For simulation, set the Max length of suppressed spikes to 1
    hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_FS_SPKLEN_OFS, 0x1);

    //SCL Settings
    //For simulation set these values to be as small as possible for faster
    //sims; FIXME: These should be adjusted for emulation / silicon.
    if (busSpeed == 1) {
        hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_SS_SCL_LCNT_OFS, 0x9);
        hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_SS_SCL_HCNT_OFS, 0x8);
    } else if (busSpeed == 2) {
        hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_FS_SCL_LCNT_OFS, 0x9);
        hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_FS_SCL_HCNT_OFS, 0x8);
    } else if (busSpeed == 3) {
        hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_SS_SCL_LCNT_OFS, 0x16c);
        hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_SS_SCL_HCNT_OFS, 0xfa);
        hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_SDA_HOLD_OFS, 0x1e0013);
        hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_SDA_SETUP_OFS, 0x64);
        hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_FS_SPKLEN_OFS, 0x4);
    } else if (busSpeed == 4) {
        hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_FS_SCL_LCNT_OFS, 0x6b);
        hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_FS_SCL_HCNT_OFS, 0x26);
        hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_SDA_HOLD_OFS, 0x1e0013);
        hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_SDA_SETUP_OFS, 0x19);
        hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_FS_SPKLEN_OFS, 0x4);
    } else {
        hw_errmsg("Invalid value for busSpeed = %d\n", busSpeed);
        return ret;
    }

    //Disable all the interrupts for this I2C instance
    hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_INTR_MASK_OFS, 0x0);

    //FIFO Thresholds
    hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_RX_TL_OFS, 0x7);
    hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_TX_TL_OFS, 0x8);

    //Write 1 to IC_ENABLE to enable i2c module.
    hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_ENABLE_OFS, 0x1);

    hw_verbose("I2C instance %d initialized: busSpeed = %d\n",
             i2cInstance, busSpeed);

    ret = TRUE;
#endif
    return ret;
}

//
// Function to write to an FPGA register through I2C
// Arguments: 
//     i2cInstance : The instance number of the I2C to be used; [0-9]
//     slvAddr     : I2C Slave Address
//     regAddr     : Address of the external register
//     wrData      : Data to be written
// Returns: None.
//
BOOL hw_i2c_write_fpga(UINT8 i2cInstance, UINT16 slvAddr, UINT8 regAddr, UINT8 wrData) 
{
    BOOL ret = FALSE;
#if (defined __XTENSA__)
    UINT32 wdata, rdata, tfe, activity, mst_activity;

    //Write the slvAddr to the TAR register
    wdata = 0x0 | (slvAddr & 0x3ff);
    hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_TAR_OFS, wdata);

    //Write address of the Register, CMD is write (bit8 = 0), no stop (bit9 = 0), 
    //no restart (bit 10 = 0)
    //wdata = 0x0 | ((regAddr & 0xff00)>>8) | I2C0_IC_DATA_CMD_RESTART_MASK;
    wdata = regAddr;
    hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_DATA_CMD_OFS, wdata);

    //Write data with stop set to 1
    wdata = 0x0 | (wrData & 0xff) | I2C0_IC_DATA_CMD_STOP_MASK;
    hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_DATA_CMD_OFS, wdata);

    //Wait for the transfer to complete
    do {
        rdata = hw_reg_read32(I2C_REG_BASE(i2cInstance), I2C0_IC_STATUS_OFS);
        tfe   = rdata & I2C0_IC_STATUS_TFE_MASK;
        activity   = rdata & I2C0_IC_STATUS_ACTIVITY_MASK;
        mst_activity   = rdata & I2C0_IC_STATUS_MST_ACTIVITY_MASK;
    } while (tfe == 0 || activity == 1 || mst_activity == 1);

    //Read the RAW_INTR_STAT register to make sure there are no errors
    rdata = hw_reg_read32(I2C_REG_BASE(i2cInstance), I2C0_IC_RAW_INTR_STAT_OFS);
    if (rdata & I2C0_IC_RAW_INTR_STAT_TX_ABRT_MASK)
    {
        hw_errmsg("I2C%d TX Aborted during write\n", i2cInstance);
        return ret;
    }

    ret = TRUE;
#endif
    return ret;
}

//
// Function to read to a FPGA register through I2C
// Arguments: 
//     i2cInstance : The instance number of the I2C to be used; [0-9]
//     slvAddr     : I2C Slave Address
//     regAddr     : Address of the external register
// Returns:
//     rdData      : Data that was read
//
BOOL hw_i2c_read_fpga(UINT8 i2cInstance, UINT16 slvAddr, UINT8 regAddr, UINT8* result)
{
    BOOL ret = FALSE;
#if (defined __XTENSA__)
    UINT32 wdata, rdata, tfe, rfne, activity, mst_activity;

    //Write the slvAddr to the TAR register
    wdata = 0x0 | (slvAddr & 0x3ff);
    hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_TAR_OFS, wdata);

    //Write address of the Register, restart (bit10 = 1)
    wdata = 0x0 | regAddr | I2C0_IC_DATA_CMD_RESTART_MASK;
    hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_DATA_CMD_OFS, wdata);

    //Read the Data, CMD is read, stop
    wdata = 0x0 | I2C0_IC_DATA_CMD_CMD_MASK | I2C0_IC_DATA_CMD_STOP_MASK;
    hw_reg_write32(I2C_REG_BASE(i2cInstance), I2C0_IC_DATA_CMD_OFS, wdata);

    //Wait for the transfers to complete
    do {
        rdata = hw_reg_read32(I2C_REG_BASE(i2cInstance), I2C0_IC_STATUS_OFS);
        tfe   = rdata & I2C0_IC_STATUS_TFE_MASK;
        rfne   = rdata & I2C0_IC_STATUS_RFNE_MASK;
        activity   = rdata & I2C0_IC_STATUS_ACTIVITY_MASK;
        mst_activity   = rdata & I2C0_IC_STATUS_MST_ACTIVITY_MASK;
    } while (tfe == 0 || rfne == 0 || activity == 1 || mst_activity == 1);

    //Read the RAW_INTR_STAT register to make sure there are no errors
    rdata = hw_reg_read32(I2C_REG_BASE(i2cInstance), I2C0_IC_RAW_INTR_STAT_OFS);
    if (rdata & I2C0_IC_RAW_INTR_STAT_TX_ABRT_MASK)
    {
        hw_errmsg("I2C%d TX Aborted during read\n", i2cInstance);
		return ret;
    }

    //Now read the byte from the RxFIFO and return it as the read data
    rdata = hw_reg_read32(I2C_REG_BASE(i2cInstance), I2C0_IC_DATA_CMD_OFS);
    *result = rdata;
    ret = TRUE;
#endif
    return ret;
}

BOOL hw_i2c_write_verify_fpga(UINT8 i2cInstance, UINT16 slvAddr, UINT8 regAddr, UINT8 wrData)
{
    BOOL ret = FALSE;
#if (defined __XTENSA__)
    UINT8 rdata8;
    if(!hw_i2c_write_fpga(i2cInstance, slvAddr, regAddr, wrData))
    {
        hw_errmsg("I2C Write failed\n");
        return ret;
    }
    hw_i2c_read_fpga(i2cInstance, slvAddr, regAddr, &rdata8);
    if (rdata8 == wrData) 
    {
       hw_verbose("I2C Ext Reg rData 0x%x == 0x%x expected\n", rdata8, wrData);
       ret = TRUE;
    }
	else
    {
       hw_errmsg("I2C Ext Reg rdata 0x%x != 0x%x expected\n", rdata8, wrData);
    }
#endif
    return ret;
}

BOOL hw_i2c_read_verify_fpga(UINT8 i2cInstance, UINT16 slvAddr, UINT8 regAddr, UINT8 wrData)
{
    BOOL ret = FALSE;
#if (defined __XTENSA__)
    UINT8 rdata8;
    hw_i2c_read_fpga(i2cInstance, slvAddr, regAddr, &rdata8);
    if (rdata8 == wrData)
    {
        hw_verbose("I2C Ext Reg rData 0x%x == 0x%x expected\n", rdata8, wrData);
        ret = TRUE;
    }
    else
    {
        hw_errmsg("I2C Ext Reg rdata 0x%x != 0x%x expected\n", rdata8, wrData);
    }
#endif
    return ret;;
}

BOOL i2c_fpga_accessible()
{
    UINT32 retry = 3;

    if(g_FPGA_accessible_checked)
    {
        return g_FPGA_accessible;
    }

    g_FPGA_accessible_checked = TRUE;
    g_FPGA_accessible = FALSE;
    
    while(retry > 0)
    {
        if(hw_i2c_init_port(I2C_INSTANCE_LHNA, BUS_SPEED))// && hw_i2c_write_fpga(I2C_INSTANCE_LHNA, I2C_SLAVE_ADDR_LHNA, SYS_LED_OVR, 0x0))
        {
            g_FPGA_accessible = TRUE;
            break;
        }
        retry --;
    }

    return g_FPGA_accessible;
}

BOOL hw_i2c_fpga_set_led(UINT32 mask)
{
    BOOL ret = FALSE;
#if (defined __XTENSA__)
    if(!i2c_fpga_accessible())
    {
        return ret;
    }
    
    hw_i2c_write_fpga(I2C_INSTANCE_LHNA, I2C_SLAVE_ADDR_LHNA, LED_DATA_0, UINT8(mask & 0xFF));
    hw_i2c_write_fpga(I2C_INSTANCE_LHNA, I2C_SLAVE_ADDR_LHNA, LED_DATA_1, UINT8((mask >> 8) & 0xFF ));
    hw_i2c_write_fpga(I2C_INSTANCE_LHNA, I2C_SLAVE_ADDR_LHNA, SYS_LED_DATA, UINT8((mask >> 16) & 0xFF ));
    ret = TRUE;
#endif
    return ret;
}

BOOL hw_i2c_fpga_set_seg7(UINT16 code)
{
	BOOL ret = FALSE;
#if (defined __XTENSA__)
    if(!i2c_fpga_accessible())
    {
        return ret;
    }

    hw_i2c_write_fpga(I2C_INSTANCE_LHNA, I2C_SLAVE_ADDR_LHNA, SEG7_LED_DATA_0, UINT8(code & 0xF));
    hw_i2c_write_fpga(I2C_INSTANCE_LHNA, I2C_SLAVE_ADDR_LHNA, SEG7_LED_DATA_1, UINT8((code >> 4) & 0xF ));
    hw_i2c_write_fpga(I2C_INSTANCE_LHNA, I2C_SLAVE_ADDR_LHNA, SEG7_LED_DATA_2, UINT8((code >> 8) & 0xF ));
    hw_i2c_write_fpga(I2C_INSTANCE_LHNA, I2C_SLAVE_ADDR_LHNA, SEG7_LED_DATA_3, UINT8((code >> 12) & 0xF ));
    ret = TRUE;
#endif
    return ret;
}

BOOL hw_i2c_fpga_power_cplb()
{
    BOOL ret = FALSE;
#if (defined __XTENSA__)
    UINT32 wdata = 0;

    if(!hw_i2c_init_port(I2C_INSTANCE_LHNA, BUS_SPEED))
    {
        hw_critical("Power CPLB: LHNA FPGA not accessible\n");
        return ret;
    }
    UINT8 detected = 0;
    if(!hw_i2c_read_fpga(I2C_INSTANCE_LHNA, I2C_SLAVE_ADDR_LHNA, EXT_CN_DET, &detected) || !detected)
    {
        hw_critical("Power CPLB: no board detected \n");
        return ret;
    }
    else
    {
        hw_critical("Power CPLB: ext_cn_det = %d \n", detected);
    }
    hw_i2c_write_fpga(I2C_INSTANCE_LHNA, I2C_SLAVE_ADDR_LHNA, EXT_PW_EN, 0x0);
    hw_sleep(1000000000ull);
    hw_i2c_write_fpga(I2C_INSTANCE_LHNA, I2C_SLAVE_ADDR_LHNA, EXT_PW_EN, 0x1);
    hw_sleep(2500000000ull);

    if(!hw_i2c_init_port(I2C_INSTANCE_CPLB, BUS_SPEED))
    {
        hw_critical("Power CPLB: CPLB FPGA not accessible\n");
        return ret;
    }
    
    hw_i2c_write_fpga(I2C_INSTANCE_CPLB, I2C_SLAVE_ADDR_CPLB, I2C_BUF_EN, 0xFF);
    hw_sleep(100000000ull);
    hw_critical("Power CPLB: up\n");

    //Change functionality of GPIO24, 25, 26 and 27 back to GPIO mode.
    hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_PINSHARE0_OFS, 0x0, POR_GPIO_PINSHARE0_GPIO26_27_MASK);
    wdata = POR_GPIO_REN0_REN_GPIO26_MASK | POR_GPIO_REN0_REN_GPIO27_MASK;
    hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_REN0_OFS, 0x0, wdata);
    hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_PINSHARE0_OFS, 0x0, POR_GPIO_PINSHARE0_GPIO24_25_MASK);
    wdata = POR_GPIO_REN0_REN_GPIO24_MASK | POR_GPIO_REN0_REN_GPIO25_MASK;
    hw_reg_writeBits32(LAHAINA_POR_BASE, POR_GPIO_REN0_OFS, 0x0, wdata);
    
    //Reset I2C8 & I2C9
    wdata = POR_SOFTRESET1_SRST_I2C8PCLKRST_N_MASK | POR_SOFTRESET1_SRST_I2C9PCLKRST_N_MASK |
    		POR_SOFTRESET1_SRST_I2C8CLKRST_N_MASK  | POR_SOFTRESET1_SRST_I2C9CLKRST_N_MASK;
    hw_reg_writeBits32(LAHAINA_POR_BASE, POR_SOFTRESET1_OFS, wdata, wdata);
    hw_reg_writeBits32(LAHAINA_POR_BASE, POR_SOFTRESETRELEASE1_OFS, wdata, wdata);

    ret = TRUE;
#endif
    return ret;
}
