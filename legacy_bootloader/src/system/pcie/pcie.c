/*-------------------------------------------------------
 |
 |    pcie.c
 |
 |    pcie util routines
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2014  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

//TODO: as the PCIe implementation is not mature, we will use the functions provided by test for now

#if 0

#include "bifrost_private.h"
#include "pcie.h"
#include "tsb_regs.h"

//////////////////////////////////////////////////////////////////////////
static VOID c_cfg_wr(UINT32 bus, UINT32 dev, UINT32 fun, UINT32 off, UINT32 data)
//////////////////////////////////////////////////////////////////////////
{
    UINT32 addr;
    UINT32 jnk;
  
    HW_PLATFORM platform = hw_get_platform();
    if (platform == CHIP_PLATFORM) 
    {
        addr = LAHAINA_PCIEEP_BASE + off;
        hw_write32((UINT32 *)addr, data);
        hw_serialize();
        jnk = hw_read32((UINT32 *)addr);
    }
    else 
    {
        addr = ((bus<<24) | (dev<<19) | (fun<<16) | off);
    
        hw_write32((UINT32 *)PCIE_BFM_AADDR, addr);
        hw_write32((UINT32 *)PCIE_BFM_DADDR, data);
        hw_serialize();
        jnk = hw_read32((UINT32 *)PCIE_BFM_WADDR); // note: yes it is read, not write
    }
}

//////////////////////////////////////////////////////////////////////////
static UINT32 c_cfg_rd(UINT32 bus, UINT32 dev, UINT32 fun, UINT32 off)
//////////////////////////////////////////////////////////////////////////
{
    UINT32 addr;
    UINT32 data;
    HW_PLATFORM platform = hw_get_platform();
    if (platform == CHIP_PLATFORM) 
    {
        addr = LAHAINA_PCIEEP_BASE + off;
        data = hw_read32((UINT32 *)addr);
    }
    else 
    {
        addr = ((bus<<24) | (dev<<19) | (fun<<16) | off);
    
        hw_write32((UINT32 *)PCIE_BFM_AADDR, addr);
        hw_serialize();
        data = hw_read32((UINT32 *)PCIE_BFM_RADDR); 
    }
    return(data);
}


//////////////////////////////////////////////////////////////////////////
static VOID c_mem_wr(UINT32 addr, UINT32 data)
//////////////////////////////////////////////////////////////////////////
{

    HW_PLATFORM platform = hw_get_platform();
    if (platform == CHIP_PLATFORM) 
    {
  
        BOOL32 result;
        UINT32 patternData[2];
        patternData[0] = data;
        patternData[1] = 0;
    
        if((hw_host_pcie_write((void*)(UINTPTR)addr, 4, PCIE_BURST_MODE_4BYTE, 
                   PCIE_DATA_PATTERN_ARBITRARY, patternData, NULL, &result) && result))
        {
    
        }
        else
        {
          hw_errmsg("c_mem_wr to addr=0x%08x Failed\n", addr);
        }
    }
    else 
    {
        hw_write32((UINT32 *)PCIE_BFM_AADDR, addr);
        hw_write32((UINT32 *)PCIE_BFM_DADDR, data);
        hw_write32((UINT32 *)PCIE_BFM_MADDR, 0); 
    }
}

//////////////////////////////////////////////////////////////////////////
static UINT32 c_mem_rd(UINT32 addr)
//////////////////////////////////////////////////////////////////////////
{
    UINT32 data;
    HW_PLATFORM platform = hw_get_platform();
    if (platform == CHIP_PLATFORM) 
    {
        BOOL32 result;
        UINT32 data;
        if (hw_host_pcie_read((void*)(UINTPTR)addr, 4, 0, PCIE_BURST_MODE_4BYTE, (UINT8*)&data, &result) && result)
        {
          return data;
        }
        else
        {
          /// debug trace for error
          hw_errmsg("c_mem_rd to addr=0x%08x Failed\n", addr);
          return 0xFFFFFFFF;
        }
    }
    else
    {
        hw_write32((UINT32 *)PCIE_BFM_AADDR, addr);
        hw_serialize();
        data = hw_read32((UINT32 *)PCIE_BFM_MADDR);
        return(data);
    }
}

//////////////////////////////////////////////////////////////////////////
static VOID ib_atu_setup(UINT32 region_id, UINT32 bar_id, UINT32 target_addr)
//////////////////////////////////////////////////////////////////////////
{
    UINT32 addr;
    UINT32 data;
    UINT32 idx, tgt, ctl1, ctl2;   
  
    // Index Reg:(bit 31 is INBOUND/OUTBOUND x-1:0 is REGION ID
    addr = PCIE_BAR1_VAL + 0x900;
    data = 0x80000000|region_id; 
    c_mem_wr(addr, data);
    data = 0x0;
    data = c_mem_rd(addr);
    idx = data;
  
    // target addr
    addr  = PCIE_BAR1_VAL + 0x918;
    data = target_addr; 
    c_mem_wr(addr, data);
    data = 0x0;
    data = c_mem_rd(addr);
    tgt = data;
    addr  = PCIE_BAR1_VAL + 0x91c;
    data = 0x00000000; 
    c_mem_wr(addr, data);
  
    // Region Control1 Reg
    addr  = PCIE_BAR1_VAL + 0x904;
    data = 0x00000000; 
    c_mem_wr(addr, data);
    data = 0x0;
    data = c_mem_rd(addr);
    ctl1 = data;
  
    // Region Control2 Reg
    addr  = PCIE_BAR1_VAL + 0x908;
    data = 0xC0000000 | (bar_id<<8); 
    c_mem_wr(addr, data);
    data = 0x0;
    data = c_mem_rd(addr);
    ctl2 = data;
    
    hw_printf(HW_BOOT_DEBUG, "INFO: PCIE IB ATU Setup: INDEX(%08x) TARGET(%08x) CONTROL1(%08x) CONTROL2(%08x)\n", idx, tgt, ctl1, ctl2);

}


//////////////////////////////////////////////////////////////////////////
static VOID ob_atu_setup(UINT32 region_id, 
                  UINT32 rba, UINT32 rla,
                  UINT32 ta_l, UINT32 ta_h)
//////////////////////////////////////////////////////////////////////////
{
    UINT32 addr;
    UINT32 data;
    UINT32 idx, rgn_base, rgn_limit, tgt_addr_lo, tgt_addr_hi, ctl1, ctl2;  
  
    // Index Reg:(bit 31 is INBOUND/OUTBOUND x-1:0 is REGION ID
    addr = PCIE_BAR1_VAL + 0x900;
    data = 0x00000000|region_id; 
    c_mem_wr(addr, data);
    data = 0x0;
    data = c_mem_rd(addr);
    idx = data;
  
    // set region base lower and limit
    addr = PCIE_BAR1_VAL + 0x90C;
    data = 0x00000000|rba; 
    c_mem_wr(addr, data);  
    data = 0x0;
    data = c_mem_rd(addr);
    rgn_base = data;
  
    addr = PCIE_BAR1_VAL + 0x910;
    data = 0x00000000; 
    c_mem_wr(addr, data);  
  
    addr = PCIE_BAR1_VAL + 0x914;
    data = 0x00000000|rla; 
    c_mem_wr(addr, data);  
    data = 0x0;
    data = c_mem_rd(addr);
    rgn_limit = data;
  
    // target addr
    addr  = PCIE_BAR1_VAL + 0x918;
    data = ta_l; 
    c_mem_wr(addr, data);
    data = 0x0;
    data = c_mem_rd(addr);
    tgt_addr_lo = data;
    addr  = PCIE_BAR1_VAL + 0x91c;
    data = ta_h; 
    c_mem_wr(addr, data);
    data = 0x0;
    data = c_mem_rd(addr);
    tgt_addr_hi = data;
  
    // Region Control1 Reg
    addr  = PCIE_BAR1_VAL + 0x904;
    data = 0x00000000; 
    c_mem_wr(addr, data);
    data = 0x0;
    data = c_mem_rd(addr);
    ctl1 = data;
    
    // Region Control2 Reg
    addr  = PCIE_BAR1_VAL + 0x908;
    data = 0x80000000;
    c_mem_wr(addr, data);
    data = 0x0;
    data = c_mem_rd(addr);
    ctl2 = data;  
    hw_printf(HW_BOOT_DEBUG, "INFO: PCIE OB ATU Setup: INDEX(%08x),REGIN BASE(%08x),REGION LIMIT(%08x),TARGET ADDR LOW(%08x),TARGET ADDR LOW(%08x),CONTROL1(%08x),CONTROL2(%08x)\n",
            idx, rgn_base, rgn_limit, tgt_addr_lo, tgt_addr_hi, ctl1, ctl2);

}


void hw_pcie_clear_correctable_error()
{
#if (defined __XTENSA__)
    c_cfg_wr(0, 0, 0, 0x110, 0xFFFFFFFF);
#endif
}

void hw_pcie_clear_uncorrectable_error()
{
#if (defined __XTENSA__)
    c_cfg_wr(0, 0, 0, 0x104, 0xFFFFFFFF);
#endif
}

void hw_pcie_clear_error_status ()
{
    hw_pcie_clear_correctable_error();
    hw_pcie_clear_uncorrectable_error();
}

UINT32 hw_pcie_check_correctable_error(UINT32* counter)
{
    UINT32 error = 0;
#if (defined __XTENSA__)
    error = c_cfg_rd(0, 0, 0, 0x110);
    if(counter != NULL)
    {
        UINT32 checker = error;
        for(UINT32 i = 0; (checker != 0) && (i < 16); i++)
        {
            if(error & (1<< i))
            {
                counter[i]++;
            }
        }
    }
#endif
    return error;
}
    
UINT32 hw_pcie_check_uncorrectable_error(UINT32* counter)
{
    UINT32 error = 0;
#if (defined __XTENSA__)
    UINT32 checker = 0;
    error = c_cfg_rd(0, 0, 0, 0x104);
    if(counter != NULL)
    {
        UINT32 checker = error;
        for(UINT32 i = 0; (checker != 0) && (i < 16); i++)
        {
            if(error & (1<< i))
            {
                counter[i]++;
            }
        }
    }
#endif
    return error;
}

void hw_pcie_parse_correctable_error(UINT32 data)
{
    if(data == 0)
    {
        return;
    }
    else
    {
        if(data & (0x1 << 0))
        {
            hw_critical("PCIE: CORR Err: Receiver\n");
            data &= ~0x1;
        }

        for (UINT32 i = 6; (data != 0) && (i <= 8); i++)
        {
            if(data & (1 << i))
            {
                switch (i)
                {              
                    case 6:
                        hw_critical("PCIE: CORR Err: Bad TLP\n");
                        data &= ~(1 << i);
                        break;
              
                    case 7:
                        hw_critical("PCIE: CORR Err: Flow DLLP Protocol\n");
                        data &= ~(1 << i);
                        break;
              
                    case 8:
                        hw_critical("PCIE: CORR Err: REPLAY_NUM Rollover\n");
                        data &= ~(1 << i);
                        break;
    
                    default:
                        break;
                }
            }
        }

        for (UINT32 i = 12; (i <= 15) && (data != 0); i++)
        {
            if(data & (1 << i))
            {
                switch (i)
                {              
                    case 12:
                        hw_critical("PCIE: CORR Err: Replay Timer Timeout\n");
                        data &= ~(1 << i);
                        break;
              
                    case 13:
                        hw_critical("PCIE: CORR Err: Advisory Non-Fatal\n");
                        data &= ~(1 << i);
                        break;
              
                    case 14:
                        hw_critical("PCIE: CORR Err: Corrected Internal\n");
                        data &= ~(1 << i);
                        break;
    
                    case 15:
                        hw_critical("PCIE: CORR Err: Header Log Overflow\n");
                        data &= ~(1 << i);
                        break;
    
                    default:
                        break;
                }
            }
        }
    }
}


void hw_pcie_parse_uncorrectable_error(UINT32 data, UINT32* counter)
{
    if(data == 0)
    {
        return;
    }
    else
    {
        if(data & (1 << 4))
        {
            hw_errmsg("PCIE: UNCORR Err: Data Link Protocol\n");
            data &= ~0x10;
        }

        if(data & (1 << 5))
        {
            hw_errmsg("PCIE: UNCORR Err: Surprise (Link) Down\n");
            data &= ~0x20;
        }


        for (UINT32 i = 12; (data != 0) && (i <= 22); i++)
        {
            if (data & (1 << i))
            {
                switch (i)
                {              
                    case 12:
                        hw_errmsg("PCIE: UNCORR Err: Poisoned TLP\n");
                        data &= ~(1 << i);
                        break;
              
                    case 13:
                        hw_errmsg("PCIE: UNCORR Err: Flow Control Protocol\n");
                        data &= ~(1 << i);
                        break;
              
                    case 14:
                        hw_errmsg("PCIE: UNCORR Err: Completion Timeout\n");
                        data &= ~(1 << i);
                        break;
              
                    case 15:
                        hw_errmsg("PCIE: UNCORR Err: Completor Abort\n");
                        data &= ~(1 << i);
                        break;
              
                    case 16:
                        hw_errmsg("PCIE: UNCORR Err: Unexpected Completion\n");
                        data &= ~(1 << i);                      
                        break;

                    case 17:
                        hw_errmsg("PCIE: UNCORR Err: Reciver Overflow\n");
                        data &= ~(1 << i);                      
                        break;
              
                    case 18:
                        hw_errmsg("PCIE: UNCORR Err: Malformed TLP\n");
                        data &= ~(1 << i);                      
                        break;
              
                    case 19:
                        hw_errmsg("PCIE: UNCORR Err: ECRC\n");
                        data &= ~(1 << i);                      
                        break;
              
                    case 20:
                        hw_errmsg("PCIE: UNCORR Err: Unsupported Request\n");
                        data &= ~(1 << i);                      
                        break;

                    case 21:
                        hw_errmsg("PCIE: UNCORR Err: ACS Violation\n");
                        data &= ~(1 << i);                      
                        break;
    
                    case 22:
                        hw_errmsg("PCIE: UNCORR Err: Internal\n");
                        data &= ~(1 << i);                      
                        break;
    
                  default:
                        break; 
                }
            }
        }
    }
}

void hw_pcie_parse_correctable_error_counter(UINT32* counter)
{
    if(counter == NULL)
    {
        return;
    }
    else
    {
        if(counter[0] > 0) // 0
        {
            hw_critical("PCIE: CORR Err: Receiver, Cnt(%u)\n", counter[0]);
        }

        for (UINT32 i = 6; (counter[i] > 0) && (i <= 8); i++)
        {
            switch (i)
            {              
                case 6:
                    hw_critical("PCIE: CORR Err: Bad TLP, Cnt(%u)\n", counter[i]);
                    break;
          
                case 7:
                    hw_critical("PCIE: CORR Err: Flow DLLP Protocol, Cnt(%u)\n", counter[i]);
                    break;
          
                case 8:
                    hw_critical("PCIE: CORR Err: REPLAY_NUM Rollover, Cnt(%u)\n", counter[i]);
                    break;

                default:
                    break;
                }
        }

        for (UINT32 i = 12; (counter[i] > 0) && (i <= 15); i++)
        {
            switch (i)
            {              
                case 12:
                    hw_critical("PCIE: CORR Err: Replay Timer Timeout, Cnt(%u)\n", counter[i]);
                    break;
          
                case 13:
                    hw_critical("PCIE: CORR Err: Advisory Non-Fatal, Cnt(%u)\n", counter[i]);
                    break;
          
                case 14:
                    hw_critical("PCIE: CORR Err: Corrected Internal, Cnt(%u)\n", counter[i]);
                    break;

                case 15:
                    hw_critical("PCIE: CORR Err: Header Log Overflow, Cnt(%u)\n", counter[i]);
                    break;

                default:
                    break;
            }
        }
    }
}


void hw_pcie_parse_uncorrectable_error_counter(UINT32* counter)
{
    if(counter == NULL)
    {
        return;
    }
    else
    {
        if(counter[4] > 0) // 4
        {
            hw_errmsg("PCIE: UNCORR Err: Data Link Protocol, Cnt(%u)\n", counter[4]);
        }

        if(counter[5] > 0) // 5
        {
            hw_errmsg("PCIE: UNCORR Err: Surprise (Link) Down, Cnt(%u)\n", counter[5]);
        }


        for (UINT32 i = 12; (counter[i] > 0) && (i <= 22); i++)
        {
            switch (i)
            {              
                case 12:
                    hw_errmsg("PCIE: UNCORR Err: Poisoned TLP, Cnt(%u)\n", counter[i]);
                    break;
          
                case 13:
                    hw_errmsg("PCIE: UNCORR Err: Flow Control Protocol, Cnt(%u)\n", counter[i]);
                    break;
          
                case 14:
                    hw_errmsg("PCIE: UNCORR Err: Completion Timeout, Cnt(%u)\n", counter[i]);
                    break;
          
                case 15:
                    hw_errmsg("PCIE: UNCORR Err: Completor Abort, Cnt(%u)\n", counter[i]);
                    break;
          
                case 16:
                    hw_errmsg("PCIE: UNCORR Err: Unexpected Completion, Cnt(%u)\n", counter[i]);
                    break;
    
                case 17:
                    hw_errmsg("PCIE: UNCORR Err: Reciver Overflow, Cnt(%u)\n", counter[i]);
                    break;
          
                case 18:
                    hw_errmsg("PCIE: UNCORR Err: Malformed TLP, Cnt(%u)\n", counter[i]);
                    break;
          
                case 19:
                    hw_errmsg("PCIE: UNCORR Err: ECRC, Cnt(%u)\n", counter[i]);
                    break;
          
                case 20:
                    hw_errmsg("PCIE: UNCORR Err: Unsupported Request, Cnt(%u)\n", counter[i]);
                    break;
    
                case 21:
                    hw_errmsg("PCIE: UNCORR Err: ACS Violation, Cnt(%u)\n", counter[i]);
                    break;
    
                case 22:
                    hw_errmsg("PCIE: UNCORR Err: Internal, Cnt(%u)\n", counter[i]);
                    break;
          
                default:
                    break; 
            }
        }
    }
}


UINT32 hw_pcie_check_speed()
{
    UINT32 speed = 0;
#if (defined __XTENSA__)
    UINT32 data = c_cfg_rd(0, 0, 0, 0x80);
    speed = (data >> 16) & 0xF;
#endif
    return speed;
}

void hw_pcie_parse_speed(UINT32 data)
{
#if (defined __XTENSA__)
    if(data == 1)
    {
        hw_critical("PCIE: Speed mode: Gen 1\n");
    }
    else if(data == 2)
    {
        hw_critical("PCIE: Speed mode: Gen 2\n");
    }
#endif
}

BOOL32 hw_pcie_check_training()
{
    BOOL32 training = FALSE;
#if (defined __XTENSA__)
    UINT32 data = c_cfg_rd(0, 0, 0, 0x80);
    training = (data >> 16) & 0x0800;
#endif
    return training;
}


UINT32 hw_pcie_init() 
{
    UINT32 err = 0;
#if (defined __XTENSA__)
    UINT32 bus = 0;
    UINT32 dev = 0;
    UINT32 fun = 0;
    UINT32 off = 0;
  
    UINT32 addr = 0;
    UINT32 data = 0;
  
    // add linkup check here
    do
    {
        // Lahaina PCIe status reg
        data = hw_read32((UINT32 *) (POR_PCIESTATUS));
        hw_printf(HW_BOOT_DEBUG, "INFO: Polling for PCIe Linkup =%08x\n",data);    
    }while (!((data >> 2) & 0x1));

    off = 0;
    data = c_cfg_rd(bus, dev, fun, off);
    if (data != VEN_DEVID)
    {
        err |= 0x1;
        hw_errmsg("Invalid VEN/DEVID: exp(0x%08x) act(0x%08x)\n", VEN_DEVID, data);
    }
    else
    {
        hw_printf(HW_BOOT_DEBUG, "INFO: Vendor Id = 0x%08x\n", data);
    }
  
    off = 0x4;
    data = 0x6;
    c_cfg_wr(bus, dev, fun, off, data);
  
    // setup BARs
    off = 0x10;
    c_cfg_wr(bus, dev, fun, off, PCIE_BAR0_VAL);
  
    off = 0x14;
    c_cfg_wr(bus, dev, fun, off, PCIE_BAR1_VAL);
  
    off = 0x18;
    c_cfg_wr(bus, dev, fun, off, PCIE_BAR2_VAL);
  
    off = 0x1C;
    c_cfg_wr(bus, dev, fun, off, PCIE_BAR3_VAL);
  
    // set max_payload(7:5) and max_read req size(14:12) to 128 bytes
    data = hw_read32((UINT32 *) (PCIE_BAR1_VAL + 0x78));
    hw_printf(HW_BOOT_DEBUG, "INFO: Default Dev CTRL_STATUS =%08x\n", data);
    data = data & 0xFFFF8F1F;
    hw_write32((UINT32 *) (PCIE_BAR1_VAL + 0x78), 0x00000000); 
  
  
    // IB ATU for BaSRAM(Region2, BAR2, 0x20100000)
    ib_atu_setup(0x2, 0x2, 0x20100000);
  
    // IB ATU for DRAM(Region3, BAR3, Target 0x80000000)
    ib_atu_setup(0x3, 0x3, 0x80000000);
  
    // IB ATU for MMIO(Region1, BAR0, Target 0x04000000)
    ib_atu_setup(0x1, 0x0, 0x04000000);
      
    // OB region1 map 0x42xx_xxxx to 0x62xx_xxxx(but 0x42000000 to 0x420fffff is RESERVED for BFM use)
    ob_atu_setup(0x1, 0x42000000, 0x43ffffff, 0x62000000, 0x00000000);

#endif  
    return err;
}

#else
#include "bifrost_private.h"
#include "pcie_test.h"

void sys_init_pcie()
{
    if((hw_getCoreNum() == 0) &&
        hw_pTestConfigs->bfinit.PCIE_INIT &&
        !hw_pTestConfigs->bfinit.VPA &&
        (hw_pTestConfigs->platform != CHIP_PLATFORM))
    {
        pcie_init();
    } else if((hw_getCoreNum() == 0) &&
        !hw_pTestConfigs->bfinit.VPA &&
        (hw_pTestConfigs->platform != CHIP_PLATFORM))
    {
        hw_write32((UINT32*)(HUP_CHIP_POR_POR_SPARE1_ADDRESS),0x12345678);
    }
}
#endif
