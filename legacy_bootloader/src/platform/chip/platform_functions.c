/*-------------------------------------------------------
 |
 |  platform_functions.c
 |
 |  Platform specific implementations of functions
 |  for RTL emulation
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2014  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
*/

#include "bifrost_private.h"

// xt_iss_client_command exists only for the ISS platform
EXTERN_C int xt_iss_client_command (const char*, const char*) { return 0; }

int
plat_puts( const char* buffer, int str_length )
{
    HW_PRINTF_BUFFER* ext_buf = (HW_PRINTF_BUFFER*)hw_readptr(&hw_extcomSpace.ext_buf);
    ts_copyToPrintBuffer( ext_buf, FALSE, buffer, str_length);
    
    return 0;
}

