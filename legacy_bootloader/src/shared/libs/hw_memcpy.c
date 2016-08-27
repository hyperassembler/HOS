/*-----------------------------------------------------
 |
 |      hw_memcpy.c
 |
 |    Memory copy functions.
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

void hw_memcpy( void* destination, const void* source, SIZE_T n )
{
    SIZE_T ctr;
    
    for(ctr = 0; ctr < n; ctr++)
    {
        hw_write8((((UINT8*) destination) + ctr), hw_read8(((UINT8*) source) + ctr));
    }
}

void hw_memcpyAligned32( void* destination, const void* source, SIZE_T n )
{
    SIZE_T ctr;
    
    for(ctr = 0; ctr < n / sizeof(UINT32); ctr++)
    {
        hw_write32((((UINT32*) destination) + ctr), hw_read32(((UINT32*) source) + ctr));
    }
    
    // If size is not a multiple of sizeof(UINT32),
    // copy whatever is left
    for(ctr *= sizeof(UINT32); ctr < n; ctr++)
    {
        hw_write8((((UINT8*)destination) + ctr), hw_read8(((UINT8*)source) + ctr));
    }
}

void hw_memcpytormem( void* destination, const void* source, SIZE_T n )
{
    SIZE_T ctr;
    
    for(ctr = 0; ctr < n; ctr++)
    {
        hw_write8((((UINT8*) destination) + ctr), *(((UINT8*) source) + ctr));
    }
}

void hw_memcpyfromrmem( void* destination, const void* source, SIZE_T n )
{
    SIZE_T ctr;
    
    for(ctr = 0; ctr < n; ctr++)
    {
        *(((UINT8*) destination) + ctr) = hw_read8(((UINT8*) source) + ctr);
    }
}

void hw_memcpyfromrmemtormem( void* destination, const void* source, SIZE_T n )
{
    SIZE_T ctr;
    
    for(ctr = 0; ctr < n; ctr++)
    {
        hw_write8((((UINT8*) destination) + ctr), hw_read8(((UINT8*) source) + ctr));
    }
}
