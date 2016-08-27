/*-----------------------------------------------------
 |
 |      hw_checkHeap.c
 |
 |  Contains functions for checking unused heap space.
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2014  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */


#include "bifrost_private.h"

//#define PRINT_ALL_CORRUPTION

//
//check whether the unused heap space is all zero
//
BOOL td_checkUnusedHeapSpace(HW_HEAP_TYPE type)
{
    BOOL ret = TRUE;
    void* addr = NULL;
    SIZE_T size = 0;
    HW_HEAP_BLOCK heapBlockID = sys_getHeapBlockID(type);

    size = td_unusedHeapSpace(heapBlockID, &addr);
    if(size > 0)
    {   
        UINT8* unallignedAddr = (UINT8*)addr; 
        UINT32 unallignedSize = (UINT32)addr % 4;
        UINT32* allignedAddr = (UINT32*)((UINTPTR)addr + unallignedSize);
        UINT32 allignedCnt = (size - unallignedSize) / 4;

#ifdef PRINT_ALL_CORRUPTION
        while(unallignedSize > 0)
        {
            if(*unallignedAddr != 0)
            {
                hw_errmsg("Heap corruption: addr(0x%x) exp(0x0) act(0x%x)\n", (UINTPTR)unallignedAddr, *unallignedAddr);
                ret = FALSE;
            }
            unallignedAddr ++;
            unallignedSize --;
        }

        while(allignedCnt > 0)
        {
            if(*allignedAddr != 0)
            {
                hw_errmsg("Heap corruption: addr(0x%x) exp(0x0) act(0x%x)\n", (UINT32)allignedAddr, *allignedAddr);
                ret = FALSE;
            }
            allignedAddr ++;
            allignedCnt --;
        }
#else
        while((unallignedSize > 0) && (*unallignedAddr == 0))
        {
            unallignedAddr ++;
            unallignedSize --;
        }
        
        if(unallignedSize > 0)
        {
            hw_errmsg("Heap corruption: addr(0x%x) exp(0x0) act(0x%x)\n", (UINT32)unallignedAddr, *allignedAddr);
            ret = FALSE;
        }
        
        while((allignedCnt > 0) && (*allignedAddr == 0))
        {
            allignedAddr ++;
            allignedCnt --;
        }
        
        if(allignedCnt > 0)
        {
            hw_errmsg("Heap corruption: addr(0x%x) exp(0x0) act(0x%x)\n", (UINT32)allignedAddr, *allignedAddr);
            ret = FALSE;
        }
#endif
    }
    
    return ret;
}

