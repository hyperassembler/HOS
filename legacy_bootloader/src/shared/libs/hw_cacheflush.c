/*-----------------------------------------------------
 |
 |      hw_cacheflush.c
 |
 |  Provides routines for flushing cache lines.
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

void hw_cacheFlushAddr(void* addr)
{
#ifdef __XTENSA__
    xthal_dcache_line_writeback_inv(addr);
    hw_serialize();
#elif defined(__x86_64__)
    hw_serialize();
    asm volatile("clflush (%0);"
                :
                : "r" (addr)
                );
    hw_serialize();
#else
    #error Invalid architecture!
#endif
}

void hw_cacheFlushBuffer(void* addr, SIZE_T bytes_size)
{
#ifdef __XTENSA__
    xthal_dcache_region_writeback_inv(addr, bytes_size);
    hw_serialize();
#else
    UINTPTR startAddr = (UINTPTR) addr;
    UINTPTR endAddr = startAddr + bytes_size;
    
    startAddr -= (startAddr % HW_CACHELINE_SIZE);
    endAddr -= (endAddr % HW_CACHELINE_SIZE);
    
    UINTPTR ctrAddr;
    for(ctrAddr = startAddr; ctrAddr <= endAddr; ctrAddr += HW_CACHELINE_SIZE)
    {
        hw_cacheFlushAddr((void*)ctrAddr);
    }
#endif
}

void hw_cacheInvalidateAddr(void* addr)
{
#ifdef __XTENSA__
    xthal_dcache_line_invalidate(addr);
#elif defined(__x86_64__)
    //
    // not implemented yet
    //
#else
    #error Invalid architecture!
#endif
}

void hw_cacheInvalidateBuffer(void* addr, SIZE_T bytes_size)
{
#ifdef __XTENSA__
    xthal_dcache_region_invalidate(addr, bytes_size);
#else
    UINTPTR startAddr = (UINTPTR) addr;
    UINTPTR endAddr = startAddr + bytes_size;
    
    startAddr -= (startAddr % HW_CACHELINE_SIZE);
    endAddr -= (endAddr % HW_CACHELINE_SIZE);
    
    UINTPTR ctrAddr;
    for(ctrAddr = startAddr; ctrAddr <= endAddr; ctrAddr += HW_CACHELINE_SIZE)
    {
        hw_cacheInvalidateAddr((void*)ctrAddr);
    }
#endif
}

