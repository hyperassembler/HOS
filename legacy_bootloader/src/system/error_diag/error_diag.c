/*-------------------------------------------------------
 |
 |    error_diag.c
 |
 |    logging fabric errors
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2014  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */


#include "bifrost.h"
#include "hup_chip.h"
#include "error_diag.h"

UINT32 hw_fabric_error_diag()
{
    UINT32 error_count = 0;
    UINT32 readData = hw_read32((UINT32 *)(HUP_CHIP_BFABRIC_ERROBS_ERROBS_ERRVLD_ADDRESS));
    UINT32 opc = 0;
    UINT32 errCode = 0;
    UINT32 len1 = 0;
    UINT32 initFlow = 0;
    UINT32 targetFlow = 0;
    ERRORLOG1 errlog1;

    // If there were no errors, just immediately return
    if (readData != HUP_FABRIC_ERROBS_ERROBS_ERRVLD_ERRVLD_FIELD_MASK)
    {
        hw_status("%s: no error detected\n", __func__);
        return (error_count);
    }
    
    error_count = 1;
    readData = hw_read32((UINT32 *)(HUP_CHIP_BFABRIC_ERROBS_ERROBS_ERRLOG0_ADDRESS));
    hw_status("%s: Error Observer has an error (ErrLog0: 0x%x), it will be decoded below\n", __func__, readData);
    // decode OPC field
    opc = HUP_FABRIC_ERROBS_ERROBS_ERRLOG0_OPC_GET(readData);
    switch ((ERRLOG0_OPC)opc)
    {
        case OPC_READ:
            hw_status("OPC Type: Read\n");
            break;
        case OPC_WRAPPED_READ:
            hw_status("OPC Type: Wrapped Read\n");
            break;
        case OPC_LINKED_READ:
            hw_status("OPC Type: Linked Read\n");
            break;
        case OPC_EXCLUSIVE_READ:
            hw_status("OPC Type: Exclusive Read\n");
            break;
        case OPC_WRITE:
            hw_status("OPC Type: Write\n");
            break;
        case OPC_WRAPPED_WRITE:
            hw_status("OPC Type: Wrapped Write\n");
            break;
        case OPC_CONDITIONAL_WRITE:
            hw_status("OPC Type: Conditional Write\n");
            break;
         default: 
            hw_status("OPC Type: Undefined\n");
            break;
    }
    
    errCode = HUP_FABRIC_ERROBS_ERROBS_ERRLOG0_ERRCODE_GET(readData);
    switch ((ERRLOG0_ERRCODE)errCode)
    {
        case SLV:
            hw_status("Error Code Type: Target Error detected by Slave\n");
            hw_status("Error Code Source: Target\n");
            break;
        case DEC:
            hw_status("Error Code Type: Address Decode Error\n");
            hw_status("Error Code Source: Initiator NIU\n");
            break;
        case UNS:
            hw_status("Error Code Type: Unsupported Request\n");
            hw_status("Error Code Source: Target NIU\n");
            break;
        case DISC:
            hw_status("Error Code Type: Disconnected target or domain\n");
            hw_status("Error Code Source: Power Disconnect\n");
            break;
        case SEC:
            hw_status("Error Code Type: Security violation (includes firewall, node-slave, pcie, firewall register access restriction)\n");
            hw_status("Error Code Source: Initiator NIU or Firewall\n");
            break;
        case HIDE:
            hw_status("Error Code Type: Hidden Security violation (reporeted as OK to initiator)\n");
            hw_status("Error Code Source: Firewall\n");
            break;
        case TMO:
            hw_status("Error Code Type: Time Out\n");
            hw_status("Error Code Source: Target NIU\n");
            break;
        default:
            hw_status("Error Code Type: Reserved\n");
            hw_status("Error Code Source: None\n");
            break;
    }
    
    len1 = HUP_FABRIC_ERROBS_ERROBS_ERRLOG0_LEN1_GET(readData);
    hw_status("Len1: %d\n", len1);
    
    readData = hw_read32((UINT32 *)(HUP_CHIP_BFABRIC_ERROBS_ERROBS_ERRLOG1_ADDRESS));
    errlog1.regVal = readData;
    initFlow = errlog1.initFlow;
    hw_status("ERRORLOG1: 0x%x\n", errlog1.regVal);
    
    switch ((ERRLOG1_INITFLOW)initFlow)
    {
       case IA_AIO:
            hw_status("Initiator: 0x%x = AIO\n", initFlow);
            break;
        case IA_CDT:
            hw_status("Initiator: 0x%x = CDT\n", initFlow);
            break;
        case IA_CFPU:
            hw_status("Initiator: 0x%x = CFPU\n", initFlow);
            break;
        case IA_DNN_RD0:
            hw_status("Initiator: 0x%x = DNN_RD0\n", initFlow);
            break;
        case IA_DNN_WR0:
            hw_status("Initiator: 0x%x = DNN_WR0\n", initFlow);
            break;
        case IA_DNN_WR1:
            hw_status("Initiator: 0x%x = DNN_WR1\n", initFlow);
            break;
        case IA_IMP0_NP:
            hw_status("Initiator: 0x%x = IMP0_NP\n", initFlow);
            break;
        case IA_IMP0_P:
            hw_status("Initiator: 0x%x = IMP0_P\n", initFlow);
            break;
        case IA_IMP1_NP:
            hw_status("Initiator: 0x%x = IMP1_NP\n", initFlow);
            break;
        case IA_IMP1_P:
            hw_status("Initiator: 0x%x = IMP1_P\n", initFlow);
            break;
        case IA_IODMA0:
            hw_status("Initiator: 0x%x = IODMA0\n", initFlow);
            break;
        case IA_IODMA1:
            hw_status("Initiator: 0x%x = IODMA1\n", initFlow);
            break;
        case IA_IODMA2:
            hw_status("Initiator: 0x%x = IODMA2\n", initFlow);
            break;
        case IA_IODMA3:
            hw_status("Initiator: 0x%x = IODMA3\n", initFlow);
            break;
        case IA_IODMA4:
            hw_status("Initiator: 0x%x = IODMA4\n", initFlow);
            break;
        case IA_IODMA5:
            hw_status("Initiator: 0x%x = IODMA5\n", initFlow);
            break;
        case IA_JBL_RD:
            hw_status("Initiator: 0x%x = JBL_RD\n", initFlow);
            break;
        case IA_JBL_WR:
            hw_status("Initiator: 0x%x = JBL_WR\n", initFlow);
            break;
        case IA_JTAG:
            hw_status("Initiator: 0x%x = JTAG\n", initFlow);
            break;
        case IA_LSR0:
            hw_status("Initiator: 0x%x = LSR0\n", initFlow);
            break;
        case IA_LSR1:
            hw_status("Initiator: 0x%x = LSR1\n", initFlow);
            break;
        case IA_MIP0:
            hw_status("Initiator: 0x%x = MIP0\n", initFlow);
            break;
        case IA_MIP1:
            hw_status("Initiator: 0x%x = MIP1\n", initFlow);
            break;
        case IA_MIP2:
            hw_status("Initiator: 0x%x = MIP2\n", initFlow);
            break;
        case IA_MIP3:
            hw_status("Initiator: 0x%x = MIP3\n", initFlow);
            break;
        case IA_MIP4:
            hw_status("Initiator: 0x%x = MIP4\n", initFlow);
            break;
        case IA_MIP5:
            hw_status("Initiator: 0x%x = MIP5\n", initFlow);
            break;
        case IA_MIP6:
            hw_status("Initiator: 0x%x = MIP6\n", initFlow);
            break;
        case IA_NODE00:
            hw_status("Initiator: 0x%x = NODE00\n", initFlow);
            break;
        case IA_NODE01:
            hw_status("Initiator: 0x%x = NODE01\n", initFlow);
            break;
        case IA_NODE02:
            hw_status("Initiator: 0x%x = NODE02\n", initFlow);
            break;
        case IA_NODE03:
            hw_status("Initiator: 0x%x = NODE03\n", initFlow);
            break;
        case IA_NODE04:
            hw_status("Initiator: 0x%x = NODE04\n", initFlow);
            break;
        case IA_NODE05:
            hw_status("Initiator: 0x%x = NODE05\n", initFlow);
            break;
        case IA_NODE06:
            hw_status("Initiator: 0x%x = NODE06\n", initFlow);
            break;
        case IA_NODE07:
            hw_status("Initiator: 0x%x = NODE07\n", initFlow);
            break;
        case IA_NODE08:
            hw_status("Initiator: 0x%x = NODE08\n", initFlow);
            break;
        case IA_NODE09:
            hw_status("Initiator: 0x%x = NODE09\n", initFlow);
            break;
        case IA_NODE10:
            hw_status("Initiator: 0x%x = NODE10\n", initFlow);
            break;
        case IA_NODE11:
            hw_status("Initiator: 0x%x = NODE11\n", initFlow);
            break;
        case IA_NODE12:
            hw_status("Initiator: 0x%x = NODE12\n", initFlow);
            break;
        case IA_PCIEC:
            hw_status("Initiator: 0x%x = PCIEC\n", initFlow);
            break;
        case IA_TIP:
            hw_status("Initiator: 0x%x = TIP\n", initFlow);
            break;
        default:
            hw_status("Initiator: 0x%x = Reserved\n", initFlow);
            break;
    }
    
    targetFlow = errlog1.targetFlow;
    switch((ERRLOG1_TARGETFLOW)targetFlow)
    {
        case TA_CDT:
            hw_status("Target: 0x%x = CDT\n", targetFlow);
            break;
        case TA_GPIO0:
            hw_status("Target: 0x%x = GPIO0\n", targetFlow);
            break;
        case TA_GPIO1:
            hw_status("Target: 0x%x = GPIO1\n", targetFlow);
            break;
        case TA_GPIO10:
            hw_status("Target: 0x%x = GPIO10\n", targetFlow);
            break;
        case TA_GPIO11:
            hw_status("Target: 0x%x = GPIO11\n", targetFlow);
            break;
        case TA_GPIO12:
            hw_status("Target: 0x%x = GPIO12\n", targetFlow);
            break;
        case TA_GPIO13:
            hw_status("Target: 0x%x = GPIO13\n", targetFlow);
            break;
        case TA_GPIO14:
            hw_status("Target: 0x%x = GPIO14\n", targetFlow);
            break;
        case TA_GPIO15:
            hw_status("Target: 0x%x = GPIO15\n", targetFlow);
            break;
        case TA_GPIO16:
            hw_status("Target: 0x%x = GPIO16\n", targetFlow);
            break;
        case TA_GPIO17:
            hw_status("Target: 0x%x = GPIO17\n", targetFlow);
            break;
        case TA_GPIO18:
            hw_status("Target: 0x%x = GPIO18\n", targetFlow);
            break;
        case TA_GPIO19:
            hw_status("Target: 0x%x = GPIO19\n", targetFlow);
            break;
        case TA_GPIO2:
            hw_status("Target: 0x%x = GPIO2\n", targetFlow);
            break;
        case TA_GPIO3:
            hw_status("Target: 0x%x = GPIO3\n", targetFlow);
            break;
        case TA_GPIO4:
            hw_status("Target: 0x%x = GPIO4\n", targetFlow);
            break;
        case TA_GPIO5:
            hw_status("Target: 0x%x = GPIO5\n", targetFlow);
            break;
        case TA_GPIO6:
            hw_status("Target: 0x%x = GPIO6\n", targetFlow);
            break;
        case TA_GPIO7:
            hw_status("Target: 0x%x = GPIO7\n", targetFlow);
            break;
        case TA_GPIO8:
            hw_status("Target: 0x%x = GPIO8\n", targetFlow);
            break;
        case TA_GPIO9:
            hw_status("Target: 0x%x = GPIO9\n", targetFlow);
            break;
        case TA_CSIM0:
            hw_status("Target: 0x%x = CSIM0\n", targetFlow);
            break;
        case TA_CSIM1:
            hw_status("Target: 0x%x = CSIM1\n", targetFlow);
            break;
        case TA_CSIM2:
            hw_status("Target: 0x%x = CSIM2\n", targetFlow);
            break;
        case TA_CSIM3:
            hw_status("Target: 0x%x = CSIM3\n", targetFlow);
            break;
        case TA_CSIM4:
            hw_status("Target: 0x%x = CSIM4\n", targetFlow);
            break;
        case TA_CSIM5:
            hw_status("Target: 0x%x = CSIM5\n", targetFlow);
            break;
        case TA_CSIM6:
            hw_status("Target: 0x%x = CSIM6\n", targetFlow);
            break;
        case TA_CSIT:
            hw_status("Target: 0x%x = CSIT\n", targetFlow);
            break;
        case TA_MIP0:
            hw_status("Target: 0x%x = MIP0\n", targetFlow);
            break;
        case TA_MIP1:
            hw_status("Target: 0x%x = MIP1\n", targetFlow);
            break;
        case TA_MIP2:
            hw_status("Target: 0x%x = MIP2\n", targetFlow);
            break;
        case TA_MIP3:
            hw_status("Target: 0x%x = MIP3\n", targetFlow);
            break;
        case TA_MIP4:
            hw_status("Target: 0x%x = MIP4\n", targetFlow);
            break;
        case TA_MIP5:
            hw_status("Target: 0x%x = MIP5\n", targetFlow);
            break;
        case TA_MIP6:
            hw_status("Target: 0x%x = MIP6\n", targetFlow);
            break;
        case TA_TIP:
            hw_status("Target: 0x%x = TIP\n", targetFlow);
            break;
        case TA_I2C0:
            hw_status("Target: 0x%x = I2C0\n", targetFlow);
            break;
        case TA_I2C1:
            hw_status("Target: 0x%x = I2C1\n", targetFlow);
            break;
        case TA_I2C12:
            hw_status("Target: 0x%x = I2C12\n", targetFlow);
            break;
        case TA_I2C13:
            hw_status("Target: 0x%x = I2C13\n", targetFlow);
            break;
        case TA_I2C14:
            hw_status("Target: 0x%x = I2C14\n", targetFlow);
            break;
        case TA_I2C2:
            hw_status("Target: 0x%x = I2C2\n", targetFlow);
            break;
        case TA_I2C6:
            hw_status("Target: 0x%x = I2C6\n", targetFlow);
            break;
        case TA_I2C7:
            hw_status("Target: 0x%x = I2C7\n", targetFlow);
            break;
        case TA_I2C8:
            hw_status("Target: 0x%x = I2C8\n", targetFlow);
            break;
        case TA_JTM1:
            hw_status("Target: 0x%x = JTM1\n", targetFlow);
            break;
        case TA_PSRO1:
            hw_status("Target: 0x%x = PSRO1\n", targetFlow);
            break;
        case TA_WDT:
            hw_status("Target: 0x%x = WDT\n", targetFlow);
            break;
        case TA_I2C10:
            hw_status("Target: 0x%x = I2C10\n", targetFlow);
            break;
        case TA_I2C11:
            hw_status("Target: 0x%x = I2C11\n", targetFlow);
            break;
        case TA_I2C15:
            hw_status("Target: 0x%x = I2C15\n", targetFlow);
            break;
        case TA_I2C3:
            hw_status("Target: 0x%x = I2C3\n", targetFlow);
            break;
        case TA_I2C4:
            hw_status("Target: 0x%x = I2C4\n", targetFlow);
            break;
        case TA_I2C5:
            hw_status("Target: 0x%x = I2C5\n", targetFlow);
            break;
        case TA_I2C9:
            hw_status("Target: 0x%x = I2C9\n", targetFlow);
            break;
        case TA_JTM0:
            hw_status("Target: 0x%x = JTM0\n", targetFlow);
            break;
        case TA_PSRO0:
            hw_status("Target: 0x%x = PSRO0\n", targetFlow);
            break;
        case TA_UART0:
            hw_status("Target: 0x%x = UART0\n", targetFlow);
            break;
        case TA_UART1:
            hw_status("Target: 0x%x = UART1\n", targetFlow);
            break;
        case TA_FM_MCU0:
            hw_status("Target: 0x%x = FM_MCU0\n", targetFlow);
            break;
        case TA_FM_MCU1:
            hw_status("Target: 0x%x = FM_MCU1\n", targetFlow);
            break;
        case TA_MCU0:
            hw_status("Target: 0x%x = MCU0\n", targetFlow);
            break;
        case TA_MCU1:
            hw_status("Target: 0x%x = MCU1\n", targetFlow);
            break;
        case TA_SERVICE_MCU0:
            hw_status("Target: 0x%x = SERVICE_MCU0\n", targetFlow);
            break;
        case TA_SERVICE_MCU1:
            hw_status("Target: 0x%x = SERVICE_MCU1\n", targetFlow);
            break;
        case TA_CFPU:
            hw_status("Target: 0x%x = CFPU\n", targetFlow);
            break;
        case TA_CFPUSRAM:
            hw_status("Target: 0x%x = CFPUSRAM\n", targetFlow);
            break;
        case TA_FM_BASRAM0:
            hw_status("Target: 0x%x = FM_BASRAM0\n", targetFlow);
            break;
        case TA_FM_BASRAM2:
            hw_status("Target: 0x%x = FM_BASRAM2\n", targetFlow);
            break;
        case TA_DSIRX0:
            hw_status("Target: 0x%x = DSIRX0\n", targetFlow);
            break;
        case TA_DSIRX1:
            hw_status("Target: 0x%x = DSIRX1\n", targetFlow);
            break;
        case TA_IMP0:
            hw_status("Target: 0x%x = IMP0\n", targetFlow);
            break;
        case TA_IMP1:
            hw_status("Target: 0x%x = IMP1\n", targetFlow);
            break;
        case TA_DSITX0:
            hw_status("Target: 0x%x = DSITX0\n", targetFlow);
            break;
        case TA_DSITX1:
            hw_status("Target: 0x%x = DSITX1\n", targetFlow);
            break;
        case TA_LSR0:
            hw_status("Target: 0x%x = LSR0\n", targetFlow);
            break;
        case TA_LSR1:
            hw_status("Target: 0x%x = LSR1\n", targetFlow);
            break;
        case TA_AIO:
            hw_status("Target: 0x%x = AIO\n", targetFlow);
            break;
        case TA_SPI0:
            hw_status("Target: 0x%x = SPI0\n", targetFlow);
            break;
        case TA_SPI1:
            hw_status("Target: 0x%x = SPI1\n", targetFlow);
            break;
        case TA_SPI2:
            hw_status("Target: 0x%x = SPI2\n", targetFlow);
            break;
        case TA_SPI3:
            hw_status("Target: 0x%x = SPI3\n", targetFlow);
            break;
        case TA_SPI4:
            hw_status("Target: 0x%x = SPI4\n", targetFlow);
            break;
        case TA_SPI5:
            hw_status("Target: 0x%x = SPI5\n", targetFlow);
            break;
        case TA_TCON:
            hw_status("Target: 0x%x = TCON\n", targetFlow);
            break;
        case TA_DNN:
            hw_status("Target: 0x%x = DNN\n", targetFlow);
            break;
        case TA_DNNDMA:
            hw_status("Target: 0x%x = DNNDMA\n", targetFlow);
            break;
        case TA_FM_BASRAM1:
            hw_status("Target: 0x%x = FM_BASRAM1\n", targetFlow);
            break;
        case TA_JTM2:
            hw_status("Target: 0x%x = JTM2\n", targetFlow);
            break;
        case TA_PCIEPHY:
            hw_status("Target: 0x%x = PCIEPHY\n", targetFlow);
            break;
        case TA_BASRAM0:
            hw_status("Target: 0x%x = BASRAM0\n", targetFlow);
            break;
        case TA_BASRAM1:
            hw_status("Target: 0x%x = BASRAM1\n", targetFlow);
            break;
        case TA_BASRAM2:
            hw_status("Target: 0x%x = BASRAM2\n", targetFlow);
            break;
        case TA_BOOTROM:
            hw_status("Target: 0x%x = BOOTROM\n", targetFlow);
            break;
        case TA_CLSRAM:
            hw_status("Target: 0x%x = CLSRAM\n", targetFlow);
            break;
        case TA_FM_PCIE:
            hw_status("Target: 0x%x = FM_PCIE\n", targetFlow);
            break;
        case TA_INTC:
            hw_status("Target: 0x%x = INTC\n", targetFlow);
            break;
        case TA_IODMA0:
            hw_status("Target: 0x%x = IODMA0\n", targetFlow);
            break;
        case TA_IODMA1:
            hw_status("Target: 0x%x = IODMA1\n", targetFlow);
            break;
        case TA_IODMA2:
            hw_status("Target: 0x%x = IODMA2\n", targetFlow);
            break;
        case TA_IODMA3:
            hw_status("Target: 0x%x = IODMA3\n", targetFlow);
            break;
        case TA_IODMA4:
            hw_status("Target: 0x%x = IODMA4\n", targetFlow);
            break;
        case TA_IODMA5:
            hw_status("Target: 0x%x = IODMA5\n", targetFlow);
            break;
        case TA_JBL:
            hw_status("Target: 0x%x = JBL\n", targetFlow);
            break;
        case TA_MC0Q0:
            hw_status("Target: 0x%x = MC0Q0\n", targetFlow);
            break;
        case TA_MC0Q1:
            hw_status("Target: 0x%x = MC0Q1\n", targetFlow);
            break;
        case TA_MC0Q10:
            hw_status("Target: 0x%x = MC0Q10\n", targetFlow);
            break;
        case TA_MC0Q11:
            hw_status("Target: 0x%x = MC0Q11\n", targetFlow);
            break;
        case TA_MC0Q2:
            hw_status("Target: 0x%x = MC0Q2\n", targetFlow);
            break;
        case TA_MC0Q3:
            hw_status("Target: 0x%x = MC0Q3\n", targetFlow);
            break;
        case TA_MC0Q4:
            hw_status("Target: 0x%x = MC0Q4\n", targetFlow);
            break;
        case TA_MC0Q5:
            hw_status("Target: 0x%x = MC0Q5\n", targetFlow);
            break;
        case TA_MC0Q6:
            hw_status("Target: 0x%x = MC0Q6\n", targetFlow);
            break;
        case TA_MC0Q7:
            hw_status("Target: 0x%x = MC0Q7\n", targetFlow);
            break;
        case TA_MC0Q8:
            hw_status("Target: 0x%x = MC0Q8\n", targetFlow);
            break;
        case TA_MC0Q9:
            hw_status("Target: 0x%x = MC0Q9\n", targetFlow);
            break;
        case TA_MC1Q0:
            hw_status("Target: 0x%x = MC1Q0\n", targetFlow);
            break;
        case TA_MC1Q1:
            hw_status("Target: 0x%x = MC1Q1\n", targetFlow);
            break;
        case TA_MC1Q10:
            hw_status("Target: 0x%x = MC1Q10\n", targetFlow);
            break;
        case TA_MC1Q11:
            hw_status("Target: 0x%x = MC1Q11\n", targetFlow);
            break;
        case TA_MC1Q2:
            hw_status("Target: 0x%x = MC1Q2\n", targetFlow);
            break;
        case TA_MC1Q3:
            hw_status("Target: 0x%x = MC1Q3\n", targetFlow);
            break;
        case TA_MC1Q4:
            hw_status("Target: 0x%x = MC1Q4\n", targetFlow);
            break;
        case TA_MC1Q5:
            hw_status("Target: 0x%x = MC1Q5\n", targetFlow);
            break;
        case TA_MC1Q6:
            hw_status("Target: 0x%x = MC1Q6\n", targetFlow);
            break;
        case TA_MC1Q7:
            hw_status("Target: 0x%x = MC1Q7\n", targetFlow);
            break;
        case TA_MC1Q8:
            hw_status("Target: 0x%x = MC1Q8\n", targetFlow);
            break;
        case TA_MC1Q9:
            hw_status("Target: 0x%x = MC1Q9\n", targetFlow);
            break;
        case TA_NODE00:
            hw_status("Target: 0x%x = NODE00\n", targetFlow);
            break;
        case TA_NODE01:
            hw_status("Target: 0x%x = NODE01\n", targetFlow);
            break;
        case TA_NODE02:
            hw_status("Target: 0x%x = NODE02\n", targetFlow);
            break;
        case TA_NODE03:
            hw_status("Target: 0x%x = NODE03\n", targetFlow);
            break;
        case TA_NODE04:
            hw_status("Target: 0x%x = NODE04\n", targetFlow);
            break;
        case TA_NODE05:
            hw_status("Target: 0x%x = NODE05\n", targetFlow);
            break;
        case TA_NODE06:
            hw_status("Target: 0x%x = NODE06\n", targetFlow);
            break;
        case TA_NODE07:
            hw_status("Target: 0x%x = NODE07\n", targetFlow);
            break;
        case TA_NODE08:
            hw_status("Target: 0x%x = NODE08\n", targetFlow);
            break;
        case TA_NODE09:
            hw_status("Target: 0x%x = NODE09\n", targetFlow);
            break;
        case TA_NODE10:
            hw_status("Target: 0x%x = NODE10\n", targetFlow);
            break;
        case TA_NODE11:
            hw_status("Target: 0x%x = NODE11\n", targetFlow);
            break;
        case TA_NODE12:
            hw_status("Target: 0x%x = NODE12\n", targetFlow);
            break;
        case TA_PCIEC:
            hw_status("Target: 0x%x = PCIEC\n", targetFlow);
            break;
        case TA_PCIEEP:
            hw_status("Target: 0x%x = PCIEEP\n", targetFlow);
            break;
        case TA_SASRAM:
            hw_status("Target: 0x%x = SASRAM\n", targetFlow);
            break;
        case TA_SERVICE:
            hw_status("Target: 0x%x = SERVICE\n", targetFlow);
            break;
        case TA_SERVICE_PROT:
            hw_status("Target: 0x%x = SERVICE_PROT\n", targetFlow);
            break;
        default:
            hw_status("Target: 0x%x = Reserved\n", targetFlow);
            break;
    }
    
    hw_status("Target Sub Range: 0x%x\n", errlog1.targetSubRange);
    hw_status("Sequence ID: 0x%x\n", errlog1.seqID);
    
    readData = hw_read32((UINT32 *)(HUP_CHIP_BFABRIC_ERROBS_ERROBS_ERRLOG3_ADDRESS));
    hw_status("ERRORLOG3 (Address Offset within the target region): 0x%x\n", readData);
    
    readData = hw_read32((UINT32 *)(HUP_CHIP_BFABRIC_ERROBS_ERROBS_ERRLOG5_ADDRESS));
    hw_status("ERRORLOG5 (User): 0x%x\n", readData);
    
    readData = hw_read32((UINT32 *)(HUP_CHIP_BFABRIC_ERROBS_ERROBS_ERRLOG7_ADDRESS));
    hw_status("ERRORLOG7 (Security): 0x%x\n", readData);
    
    return (error_count); 
}

