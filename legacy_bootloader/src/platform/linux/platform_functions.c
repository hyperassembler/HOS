/*-------------------------------------------------------
 |
 |  platform_functions.c
 |
 |  Platform specific implementations of functions for linux
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2013  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
*/

#include <bifrost.h>

extern __thread UINT32 thread_pid;
UINT32 hw_getCoreNum()
{
    return thread_pid;
}

int
plat_puts( const char* buffer, int str_length )
{
    printf(buffer);
    
    return 0;
}

// xt_iss_client_command exists only for the ISS platform
EXTERN_C int xt_iss_client_command (const char*, const char*) { return 0; }

// JTM stub fucntions
extern void td_perfmon_init ()
{
}

extern void td_perfmon_update (UINT64 cur_time)
{    
}

VOID td_tempmon_init()
{
}

VOID td_tempmon_report_temps (UINT64 cur_time)
{
}

UINT32 hw_get_jtm_interval()
{
    return 0;
}

INT32 hw_get_jtm_slope()
{
    return 0;
}

INT32 hw_get_jtm_offset()
{
    return 0;
}

// MSI stub fucntion
BOOL hw_raise_msi(UINT8 vector)
{
    return FALSE;
}

// DDR stub function
UINT32 hw_get_ddr_frequency()
{
    return 0;
}

// System stub functions
void sys_blockTest()
{
}

BOOL sys_standby()
{
    return FALSE;
}

void sys_print_dramc_init_status()
{
}

BOOL sys_need_dram_heap_init()
{
    return FALSE;
}

// I2C stub functions
BOOL hw_i2c_init_port(UINT8 i2cInstance, UINT8 busSpeed)
{
    return FALSE;
}

BOOL hw_i2c_write_fpga(UINT8 i2cInstance, UINT16 slvAddr, UINT8 regAddr, UINT8 wrData)
{
    return FALSE;
}

BOOL hw_i2c_read_fpga(UINT8 i2cInstance, UINT16 slvAddr, UINT8 regAddr, UINT8* result)
{
    return FALSE;
}

BOOL hw_i2c_write_verify_fpga(UINT8 i2cInstance, UINT16 slvAddr, UINT8 regAddr, UINT8 wrData)
{
    return FALSE;
}

BOOL hw_i2c_read_verify_fpga(UINT8 i2cInstance, UINT16 slvAddr, UINT8 regAddr, UINT8 wrData)
{
    return FALSE;
}

BOOL hw_i2c_fpga_set_led(UINT32 mask)
{
    return FALSE;
}

BOOL hw_i2c_fpga_set_seg7(UINT16 code)
{
    return FALSE;
}

BOOL hw_i2c_fpga_power_cplb()
{
    return FALSE;
}

// PCIE stub functions
void   hw_pcie_clear_correctable_error()
{
}

void   hw_pcie_clear_uncorrectable_error()
{
}

void   hw_pcie_clear_error_status ()
{
}

UINT32 hw_pcie_check_correctable_error(UINT32* counter)
{
    return 0;
}

UINT32 hw_pcie_check_uncorrectable_error(UINT32* counter)
{
    return 0;
}

void   hw_pcie_parse_correctable_error(UINT32 data)
{
}

void   hw_pcie_parse_uncorrectable_error(UINT32 data)
{
}

void   hw_pcie_parse_correctable_error_counter(UINT32* counter)
{
}

void   hw_pcie_parse_uncorrectable_error_counter(UINT32* counter)
{
}

UINT32 hw_pcie_check_error()
{
    return 0;
}

UINT32 hw_pcie_check_speed()
{
    return 0;
}

void   hw_pcie_parse_speed(UINT32 data)
{
}

BOOL32 hw_pcie_check_training()
{
    return FALSE;
}

UINT32 hw_pcie_init()
{
    return 0;
}

// Heap stub fucntion
BOOL td_checkUnusedHeapSpace(HW_HEAP_TYPE type)
{
    return TRUE;
}

// Plat stub function
BOOL hw_is_silicon()
{
    return FALSE;
}

