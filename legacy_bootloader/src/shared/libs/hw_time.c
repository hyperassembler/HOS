/*-----------------------------------------------------
 |
 |      hw_time.c
 |
 |  Contains functions for obtaining the current time.
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


UINT64 hw_getTime()
{
    return sys_getTime();
}

