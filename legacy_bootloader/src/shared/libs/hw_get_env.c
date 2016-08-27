/*-----------------------------------------------------
 |
 |      hw_get_env.c
 |
 |  Provides functions to query environment
 |  information.
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

HW_PLATFORM hw_get_platform()
{
    return (HW_PLATFORM)hw_pTestConfigs->platform;
}

const char* hw_get_config()
{
    return hw_pTestConfigs->config;
}

HW_ARCHTYPE hw_get_archType()
{
    return hw_archTypeList[hw_getCoreNum()];
}

BOOL hw_is_node_rtl(UINT32 node_number)
{
    return (hw_read32(&hw_pTestConfigs->bfinit.NODE_RTL) >> node_number) & 0x1;
}

