/*-----------------------------------------------------
 |
 |      hw_serialize.c
 |
 |  Provides routines for serializing memory access
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

void hw_serialize()
{
#if defined(__XTENSA__)
    #pragma flush_memory
#elif defined(__x86_64__)
    asm volatile ("mfence;");
#else
    #error Invalid architecture!
#endif
}

