/*-----------------------------------------------------
 |
 |      hw_memcpy.c
 |
 |    Memory comparison function.
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

INT32 hw_memcmp( const void* ptr1, const void* ptr2, const SIZE_T len )
{
    SIZE_T ctr = 0;
    UINT8 a = 0;
    UINT8 b = 0;
    
    for(ctr = 0; (ctr < len) && (a == b); ctr++)
    {
        a = hw_read8(&(((UINT8*)ptr1)[ctr]));
        b = hw_read8(&(((UINT8*)ptr2)[ctr]));
    }
    
    if (a < b)
    {
        return -1;
    }
    else if (a > b)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
