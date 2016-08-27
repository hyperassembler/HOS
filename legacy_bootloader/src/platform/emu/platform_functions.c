/*-------------------------------------------------------
 |
 |  platform_functions.c
 |
 |  Platform specific implementations of functions
 |  for RTL emulation
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

// xt_iss_client_command exists only for the ISS platform
EXTERN_C int xt_iss_client_command (const char*, const char*) { return 0; }

//  Displays POST codes or the environment equivalent.
void plat_post(HW_STATUSCODE code, UINT32 testID)
{
    // Even though general hostcmd is not supported
    // in this platform, the POST is.
    hw_hostcmd_post(code, testID);
}

int
plat_puts( const char* buffer, int str_length )
{
    for(int i = 0; i < str_length; i++)
    {
        hw_write8((UINT8*)&hw_extcomSpace.core[0].buf.flag, buffer[i]);
        hw_cacheFlushAddr((void*)&hw_extcomSpace.core[0].buf.flag);
    }
    
    return 0;
}

