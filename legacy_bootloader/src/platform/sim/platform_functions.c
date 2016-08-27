/*-------------------------------------------------------
 |
 |  platform_functions.c
 |
 |  Platform specific implementations of functions for
 |  RTL simulation
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2013  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
*/

#include "bifrost.h"
#include <bifrost_intr.h>
#include <conf_defs.h>

// xt_iss_client_command exists only for the ISS platform
EXTERN_C int xt_iss_client_command (const char*, const char*) { return 0; }

#define HW_HOSTCMD_DATA_BUF_SIZE (32)
#define HW_HOSTCMD_RESP_BUF_SIZE (32)

// Formats for EXTCOM flags
typedef union {
    struct
    {
        UINT32 payload   : 16;
        UINT32 testID    : 8;
        UINT32 dwpid     : 6;
        UINT32 need_resp : 1;
        UINT32 go        : 1;
    };
    UINT32 AsUINT32;
} __attribute__ ((packed)) HW_EXTCOM_SIGNAL;


typedef struct {
    HW_EXTCOM_SIGNAL flag;
    UINT32 fmt;
} __attribute__ ((packed)) HW_EXTCOM_PRINT_FLAG;

typedef struct {
    HW_EXTCOM_SIGNAL cmd;
    UINT32 rsvd1[7];
    UINT32 resp;
    UINT32 rsvd2[7];
    UINT8  cmd_data[HW_HOSTCMD_DATA_BUF_SIZE];
    UINT8  resp_data[HW_HOSTCMD_RESP_BUF_SIZE];
} __attribute__ ((packed)) HW_EXTCOM_CMD_BUF;

typedef struct {
    HW_EXTCOM_PRINT_FLAG buf;  // Print flags, one for each core
    UINT32 rsvd[6];            // Pad to cacheline size
    HW_EXTCOM_SIGNAL post;     // Flag for sending POST codes
    UINT32 rsvd2[7];           // Pad to cacheline size
    HW_EXTCOM_CMD_BUF cmd_buf; // host cmd buffer, one for each core
} __attribute__ ((packed)) HW_EXTCOM_CORE_FLAGS;

typedef struct {
    UINT32 version;                          // Must be the first field so that we have a consistent reference point in all versions
    UINT32 rsvd1[1];                         // Reserved
    UINT64 error;                            // Reserved for BFM to cause an error when a write occurs here
    UINT32 rsvd2[12];                        // Reserved
    HW_EXTCOM_CORE_FLAGS core[HW_PROC_CNT];  // Per-core flags
    UINT32 rsvd3[388];                       // Reserved
    UINT32 disp[8];                          // Disp Command interface 4 words each 2 instances
    UINT64 ext_buf; // Pointer to printf buffer between testdriver and host
    UINT32 eot_flag;                         // Flag for signaling end of test with coolcode or baadcode
    UINT32 rsvd4[1];                         // Reserved
} __attribute__ ((packed)) HW_EXTCOM_SPACE;

#define HW_EXTCOM_VERSION 4 // This value should be incremented whenever the memory layout of HW_EXTCOM_SPACE changes

extern volatile HW_EXTCOM_SPACE hw_extcomSpace;

__attribute__ ((section (".extcom"))) volatile HW_EXTCOM_SPACE hw_extcomSpace;


void hw_write32(volatile UINT32* ptr, UINT32 data)
{
    *ptr = data;
}

// Send a preformatted string to the host monitor
void hw_hostcmd_puts( const char* buffer, UINT32 testID )
{
    UINT32 dwpid = hw_get_current_core();
    HW_EXTCOM_SIGNAL signal = {{0, testID, dwpid, 0x0, 0x1}};
    
    hw_write32(&hw_extcomSpace.core[dwpid].buf.fmt, (UINTPTR)buffer);
    hw_write32(&hw_extcomSpace.core[dwpid].buf.flag.AsUINT32, signal.AsUINT32);
}

int
plat_puts( const char* buffer, int str_length )
{
    //UINT32 testID = hw_getMyInstanceID();
    
    //hw_cacheFlushBuffer((void*)buffer, str_length + 1);
    hw_hostcmd_puts(buffer, 0);
    
    return 0;
}

void plat_terminate()
{
    hw_write32(&hw_extcomSpace.eot_flag, 0xC001C0DE);
}

