/*-----------------------------------------------------
 |
 |      hw_memset.c
 |
 |  It's memset.
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2011  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
*/

#include "bifrost_private.h"

void hw_memset(void * ptr, UINT8 value, SIZE_T len )
{
    SIZE_T ctr;
    UINT32 value32 = value;
    value32 |= (value32 << 8) | (value32 << 16) | (value32 << 24);
    
    //
    // Handle if ptr is not aligned to 4 bytes
    //
    UINT8* bPtr = (UINT8*)ptr;

    for(ctr = 0; ctr < len && ctr < (4 - ((UINTPTR) ptr % 4)); ctr++)
    {
        hw_write8( &bPtr[ctr], value);
    }
    
    //
    // While remaining length is greater than 4 bytes,
    // write 4 bytes at a time
    //
    UINT32 * dPtr = (UINT32*)(bPtr + ctr);
    len -= ctr;

    for(ctr = 0; ctr < len / 4; ctr++)
    {
        hw_write32( &dPtr[ctr], value32);
    }
    
    //
    // If the endpoint is not aligned to 4 bytes, handle
    //
    bPtr = (UINT8*)(dPtr + ctr);
    len -= (ctr * 4);
    
    for(ctr = 0; ctr < len; ctr++)
    {
        hw_write8( &bPtr[ctr], value);
    }
}

