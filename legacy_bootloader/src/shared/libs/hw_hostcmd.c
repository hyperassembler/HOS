/*-----------------------------------------------------
 |
 |      hw_hostcmd.c
 |
 |  Functions to send command to host.
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2013  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */

#include "bifrost_private.h"
#include <bifrost_intr.h>

BOOL hw_hostcmd(UINT16 cmd, UINT8* cmd_data, UINT32 cmd_data_size, BOOL need_resp, UINT16* resp, UINT8* resp_data, UINT32 resp_data_size)
{
    UINT32 dwpid = hw_getCoreNum();
    UINT32 testID = hw_getMyInstanceID();
    UINT32 responseFromHost = 0;
    UINT32 ret = 0;
    HW_EXTCOM_SIGNAL command = {{cmd, testID, dwpid, need_resp, 1}};
    HW_EXTCOM_SIGNAL tmp_signal;
    
    if(cmd_data_size > HW_HOSTCMD_DATA_BUF_SIZE)
    {
        hw_printf(HW_IO_DEBUG, "Core %u, test ID %x: Bifrost host cmd(%x) data size (%d) larger than max allowed (%d)\n",
            dwpid, testID, cmd, cmd_data_size, HW_HOSTCMD_DATA_BUF_SIZE);
        return FALSE;
    }
    
    HW_PLATFORM platform = hw_get_platform();
    
    if(hw_pTestConfigs->bfinit.HOSTCMD_EN)
    {
        do
        {
            hw_cacheInvalidateAddr((void*)&hw_extcomSpace.core[dwpid].cmd_buf.cmd.AsUINT32);
            tmp_signal.AsUINT32 = hw_read32(&hw_extcomSpace.core[dwpid].cmd_buf.cmd.AsUINT32);
        } while(tmp_signal.go != 0);
        
        hw_write32(&hw_extcomSpace.core[dwpid].cmd_buf.resp, 0);
        if((cmd_data != NULL) && (cmd_data_size > 0))
        {
            hw_memcpy((void*)hw_extcomSpace.core[dwpid].cmd_buf.cmd_data, (void*)cmd_data, cmd_data_size);
            hw_cacheFlushBuffer((void*)hw_extcomSpace.core[dwpid].cmd_buf.cmd_data, cmd_data_size);
        }
        
        if(platform != CHIP_PLATFORM)
        {
            hw_write32(&hw_extcomSpace.core[dwpid].cmd_buf.cmd.AsUINT32, command.AsUINT32);
            hw_cacheFlushAddr((void*)&hw_extcomSpace.core[dwpid].cmd_buf.cmd);
        }
        else
        {
            HW_MESSAGE message = {{0}};
            message.metadata.command = MSG_TS_CMD_MSI;
            message.metadata.size = 3 * sizeof(UINT32);
            
            UINT32* datafield = (UINT32*) message.data;
            UINT32 vector = 0;
            datafield[0] = vector;
            datafield[1] = testID;
            datafield[2] = command.AsUINT32;
            
            hw_irql_t prev_irql = hw_raise_irql(HW_IRQL_DISABLED_LEVEL);
            
            placeMessage(&testSlaveVars->driverMailbox, &message);
            
            while(!getMessage(&testSlaveVars->slaveMailbox, &message));
            ret = *(UINT32*) message.data;
            
            hw_lower_irql(prev_irql);
            
            if(HW_S_OK != ret)
            {
                hw_errmsg("%s: sent MSI failed (testID=%d)\n", __func__, testID);
                return FALSE;
            }
        }
        if(need_resp)
        {
            while(responseFromHost == 0)
            {
                responseFromHost = hw_read32(&hw_extcomSpace.core[dwpid].cmd_buf.resp);
            }
            *resp = responseFromHost & 0xFF;
            if(*resp != HOST_ERROR_NONE)
            {
                hw_errmsg("%s: host indicated error (%d) in response \n", __func__, *resp);
                hw_write32(&hw_extcomSpace.core[dwpid].cmd_buf.resp, 0);
                return FALSE;
            }
            if(resp_data_size > HW_HOSTCMD_RESP_BUF_SIZE)
            {
                hw_errmsg("%s: resp_data expected size (%d) larger than max allowed (%d) \n", __func__, resp_data_size, HW_HOSTCMD_RESP_BUF_SIZE);
                resp_data_size = HW_HOSTCMD_DATA_BUF_SIZE;
            }
            hw_memcpy((void*)resp_data, (void*)hw_extcomSpace.core[dwpid].cmd_buf.resp_data, resp_data_size);
            hw_write32(&hw_extcomSpace.core[dwpid].cmd_buf.resp, 0);
        }
    }
    else
    {
        hw_errmsg("Core %u, test ID %x: HOSTCMD_EN is False for this environment! (command was 0x%x)\n", dwpid, testID, cmd);
        return FALSE;
    }
    
    return TRUE;
}


BOOL hw_host_memcmp( const void* ptr1, const void* ptr2, const SIZE_T len, INT32* result)
{
    BOOL ret = FALSE;
    HW_HOSTCMD cmd = HW_HOSTCMD_MEMCMP;    
    HW_HOSTCMD_DATA_MEMCMP cmd_data;
    UINT16 resp = 0;
    
    cmd_data.ptr1 = (UINT8*) ptr1;
    cmd_data.ptr2 = (UINT8*) ptr2;
    cmd_data.len = (UINT32) len;

    if(isCachedAddress((UINTPTR)ptr1))
    {
        hw_cacheFlushBuffer((void*) ptr1, len);
    }
    if(isCachedAddress((UINTPTR)ptr2))
    {
        hw_cacheFlushBuffer((void*) ptr2, len);
    }
    return hw_hostcmd(cmd, (UINT8*)&cmd_data, sizeof(cmd_data), TRUE, &resp, (UINT8*)result, (UINT32)sizeof(INT32));
}

BOOL hw_host_crc32( UINT8* data, const SIZE_T len, UINT32 prevRemainder, UINT32* result)
{
    HW_HOSTCMD cmd = HW_HOSTCMD_CRC32;
    HW_HOSTCMD_DATA_CRC32 cmd_data;
    UINT16 resp = 0;

    cmd_data.ptr = (UINT8*) data;
    cmd_data.len = (UINT32)len;
    cmd_data.prevRemainder = (UINT32)prevRemainder;

    if(isCachedAddress((UINTPTR)data))
    {
        hw_cacheFlushBuffer((void*)data, len);
    }
    return hw_hostcmd(cmd, (UINT8*)&cmd_data, sizeof(cmd_data), TRUE, &resp, (UINT8*)result, (UINT32)sizeof(UINT32));
}


BOOL hw_host_cdma_crc32( UINT8* data, const SIZE_T len, UINT32 prevRemainder, UINT32 frag_en, UINT32 frag_threshold_enc, UINT32 xdim, UINT32 ydim, UINT32* result)
{
    HW_HOSTCMD cmd = HW_HOSTCMD_CDMA_CRC32;
    HW_HOSTCMD_DATA_CDMA_CRC32 cmd_data;
    UINT16 resp = 0;

    cmd_data.ptr = (UINT8*) data;
    cmd_data.len = (UINT32)len;
    cmd_data.prevRemainder = (UINT32)prevRemainder;
    cmd_data.frag_en = (UINT32)frag_en;
    cmd_data.frag_threshold_enc = (UINT32)frag_threshold_enc;
    cmd_data.xdim = (UINT32)xdim;
    cmd_data.ydim = (UINT32)ydim;

    if(isCachedAddress((UINTPTR)data))
    {
        hw_cacheFlushBuffer((void*)data, len);
    }
    return hw_hostcmd(cmd, (UINT8*)&cmd_data, sizeof(cmd_data), TRUE, &resp, (UINT8*)result, (UINT32)sizeof(UINT32));
}


BOOL hw_host_file_read( const CHAR* fileName, const SIZE_T fileOffset, const CHAR* buffer, const SIZE_T size, UINT32* len)
{
    HW_HOSTCMD cmd = HW_HOSTCMD_FILE_READ;
    HW_HOSTCMD_DATA_FILE_READ cmd_data;
    UINT16 resp = 0;

    cmd_data.fileName = (CHAR*)fileName;
    cmd_data.fileOffset = (UINT32)fileOffset;
    cmd_data.buffer = (CHAR*)buffer;
    cmd_data.size = (UINT32)size;

    if(isCachedAddress((UINTPTR)fileName))
    {
        hw_cacheFlushBuffer((void*) fileName, 256);
    }

    if(isCachedAddress((UINTPTR)buffer))
    {
        hw_cacheInvalidateBuffer((void*) buffer, size);
    }
    return hw_hostcmd(cmd, (UINT8*)&cmd_data, sizeof(cmd_data), TRUE, &resp, (UINT8*)len, (UINT32)sizeof(UINT32));
}

BOOL hw_host_file_write( const CHAR* fileName, const SIZE_T fileOffset, CHAR* buffer, const SIZE_T size, UINT32* len)
{
    HW_HOSTCMD cmd = HW_HOSTCMD_FILE_WRITE;
    HW_HOSTCMD_DATA_FILE_WRITE cmd_data;
    UINT16 resp = 0;

    cmd_data.fileName = (CHAR*)fileName;
    cmd_data.fileOffset = (UINT32)fileOffset;
    cmd_data.buffer = (CHAR*)buffer;
    cmd_data.size = (UINT32)size;

    if(isCachedAddress((UINTPTR)fileName))
    {
        hw_cacheFlushBuffer((void*) fileName, 256);
    }

    if(isCachedAddress((UINTPTR)buffer))
    {
        hw_cacheFlushBuffer((void*) buffer, size);
    }
    return hw_hostcmd(cmd, (UINT8*)&cmd_data, sizeof(cmd_data), TRUE, &resp, (UINT8*)len, (UINT32)sizeof(UINT32));
}

BOOL hw_host_pcie_read(const void* ptr, const SIZE_T len, UINT32 checksum, HW_HOSTCMD_PCIE_BURST_MODE burstMode, UINT8* data, BOOL32* result)
{
    BOOL ret;
    HW_HOSTCMD cmd = HW_HOSTCMD_PCIE_READ;
    HW_HOSTCMD_DATA_PCIE_READ cmd_data;
    HW_HOSTCMD_RESP_PCIE_READ cmd_resp;
    UINT16 resp = 0;
    UINT32 read_len = len;
    UINT32 i = 0;

    cmd_data.ptr = (UINT8*)ptr;
    cmd_data.len = (UINT32)len;
    cmd_data.checksum = checksum;
    cmd_data.burstMode = burstMode;

    if(isCachedAddress((UINTPTR)ptr))
    {
        hw_cacheFlushBuffer((void*) ptr, len);
    }
    ret = hw_hostcmd(cmd, (UINT8*)&cmd_data, sizeof(cmd_data), TRUE, &resp, (UINT8*)&cmd_resp, (UINT32)sizeof(HW_HOSTCMD_RESP_PCIE_READ));

    if(read_len > 4)
    {
        read_len = 4;
    }
    for(i = 0; i < read_len; i++)
    {
        data[i] = cmd_resp.data[i];
    }
    *result = cmd_resp.result;
    return ret;
}

BOOL hw_host_pcie_write(void* ptr, const SIZE_T len, HW_HOSTCMD_PCIE_BURST_MODE burstMode, 
    HW_HOSTCMD_PCIE_DATA_PATTERN_TYPE patternType, UINT32* patternData, UINT32* checksum, BOOL32* result)
{
    BOOL ret;
    HW_HOSTCMD cmd = HW_HOSTCMD_PCIE_WRITE;
    HW_HOSTCMD_DATA_PCIE_WRITE cmd_data;
    HW_HOSTCMD_RESP_PCIE_WRITE cmd_resp;
    UINT16 resp = 0;

    cmd_data.ptr = (UINT8*)ptr;
    cmd_data.len = (UINT32)len;
    cmd_data.burstMode= burstMode;
    cmd_data.patternType = patternType;
    cmd_data.patternData[0] = patternData[0];
    cmd_data.patternData[1] = patternData[1];
    cmd_data.needChecksum = (checksum != NULL)? TRUE : FALSE;

    if(isCachedAddress((UINTPTR)ptr))
    {
        hw_cacheInvalidateBuffer((void*)ptr, len);
    }
    ret = hw_hostcmd(cmd, (UINT8*)&cmd_data, sizeof(cmd_data), TRUE, &resp, (UINT8*)&cmd_resp, (UINT32)sizeof(HW_HOSTCMD_RESP_PCIE_WRITE));
    if(checksum != NULL)
    {
        *checksum = cmd_resp.checksum;
    }
    *result = cmd_resp.result;
    return ret;
}

BOOL hw_host_pcie_memcpy(void* dest, const void* src, const SIZE_T len, HW_HOSTCMD_PCIE_BURST_MODE burstMode, BOOL32* result)
{
    HW_HOSTCMD cmd = HW_HOSTCMD_PCIE_MEMCPY;
    HW_HOSTCMD_DATA_PCIE_MEMCPY cmd_data;
    UINT16 resp = 0;

    cmd_data.dest = (UINT8*)dest;
    cmd_data.src = (UINT8*)src;
    cmd_data.len = (UINT32)len;
    cmd_data.burstMode= burstMode;

    if(isCachedAddress((UINTPTR)src))
    {
        hw_cacheFlushBuffer((void*)src, len);
    }
    if(isCachedAddress((UINTPTR)dest))
    {
        hw_cacheInvalidateBuffer((void*)dest, len);
    }
    return hw_hostcmd(cmd, (UINT8*)&cmd_data, sizeof(cmd_data), TRUE, &resp, (UINT8*)result, (UINT32)sizeof(BOOL32));
}


BOOL hw_host_pcie_suspend(UINT32 timeout, BOOL32* result)
{
    HW_HOSTCMD cmd = HW_HOSTCMD_PCIE_SUSPEND;
    HW_HOSTCMD_DATA_PCIE_SUSPEND cmd_data;
    UINT16 resp = 0;

    cmd_data.timeout = timeout;
    return hw_hostcmd(cmd, (UINT8*)&cmd_data, sizeof(cmd_data), TRUE, &resp, (UINT8*)result, (UINT32)sizeof(BOOL32));
}

BOOL hw_host_pcie_resume(BOOL32* result)
{
    HW_HOSTCMD cmd = HW_HOSTCMD_PCIE_RESUME;
    UINT16 resp = 0;

    return hw_hostcmd(cmd, NULL, 0, TRUE, &resp, (UINT8*)result, (UINT32)sizeof(BOOL32));
}

BOOL hw_host_notify_standby(BOOL32* result)
{
    HW_HOSTCMD cmd = HW_HOSTCMD_NOTIFY_STANDBY;
    UINT16 resp = 0;

    return hw_hostcmd(cmd, NULL, 0, TRUE, &resp, (UINT8*)result, (UINT32)sizeof(BOOL32));
}

BOOL hw_host_query_msi(UINT8 vector, BOOL32* result)
{
    HW_HOSTCMD cmd = HW_HOSTCMD_QUERY_MSI;
    HW_HOSTCMD_DATA_QUERY_MSI cmd_data;
    UINT16 resp = 0;

    cmd_data.vector = vector;
    return hw_hostcmd(cmd, (UINT8*)&cmd_data, sizeof(cmd_data), TRUE, &resp, (UINT8*)result, (UINT32)sizeof(BOOL32));
}

BOOL hw_host_notify_fir(BOOL32* result)
{
    HW_HOSTCMD cmd = HW_HOSTCMD_NOTIFY_FIR;
    UINT16 resp = 0;

    return hw_hostcmd(cmd, NULL, 0, TRUE, &resp, (UINT8*)result, (UINT32)sizeof(BOOL32));
}

BOOL hw_host_notify_hostmem_alloc(UINT64* hostmemPhyAddrBase, BOOL32* result)
{
    BOOL ret;
    HW_HOSTCMD cmd = HW_HOSTCMD_NOTIFY_HOSTMEM_ALLOC;
    HW_HOSTCMD_RESP_NOTIFY_HOSTMEM_ALLOC cmd_resp;
    UINT16 resp = 0;

    ret = hw_hostcmd(cmd, NULL, 0, TRUE, &resp, (UINT8*)&cmd_resp, (UINT32)sizeof(HW_HOSTCMD_RESP_NOTIFY_HOSTMEM_ALLOC));
    *hostmemPhyAddrBase = cmd_resp.hostmemPhyAddrBase;
    *result = cmd_resp.result;
    return ret;
}

/*******************************************************************************/
// Name: hw_force_release_signal
// Purpose: Used to setup a forced value on an RTL signal or to release the
//          signal to be driven by the normal drivers
// Parameters:
//   fr_n    : FORCE (true) or RELEASE(false)
//   sig_name: Name of the signal to be operated on.  This string should be one
//             of the strings checked for in the forceReleaseSignal verilog
//             function defined in HostCmdMonitor.svh
//   value   : Value to be forced on the signal.  For signals narrower than
//             64 bits, the LSBs should be used
/******************************************************************************/
BOOL hw_force_release_signal(BOOL fr_n, const CHAR * sig_name, UINT64 value)
{
  HW_HOSTCMD cmd = HW_HOSTCMD_FORCE_RELEASE_SIGNAL;
  HW_HOSTCMD_DATA_FORCE_RELEASE cmd_data;
  UINT16 resp;
  UINT8 cmd_resp;

  cmd_data.fr_n = fr_n;
  cmd_data.sig_name = (CHAR *)sig_name;
  cmd_data.value = value;

  if(isCachedAddress((UINTPTR)sig_name))
  {
    hw_cacheFlushBuffer((void*) sig_name, 256);
  }

  return hw_hostcmd(cmd, (UINT8 *)&cmd_data, sizeof(cmd_data), FALSE, &resp, &cmd_resp, 0);
}


// Sends a postcode to the host monitor
void hw_hostcmd_post(HW_STATUSCODE code, UINT32 testID)
{
    UINT32 dwpid = hw_getCoreNum();
    HW_EXTCOM_SIGNAL signal = {{code, testID, dwpid, 0x0, 0x1}};
    
    hw_write32(&hw_extcomSpace.core[dwpid].post.AsUINT32, signal.AsUINT32);
}

// Send a preformatted string to the host monitor
void hw_hostcmd_puts( const char* buffer, UINT32 testID )
{
    UINT32 dwpid = hw_getCoreNum();
    HW_EXTCOM_SIGNAL signal = {{0, testID, dwpid, 0x0, 0x1}};
    
    hw_write32(&hw_extcomSpace.core[dwpid].buf.fmt, (UINTPTR)buffer);
    hw_write32(&hw_extcomSpace.core[dwpid].buf.flag.AsUINT32, signal.AsUINT32);
}

