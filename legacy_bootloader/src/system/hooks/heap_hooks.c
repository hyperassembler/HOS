/*-------------------------------------------------------
 |
 |     heap_hooks.c
 |
 |     Project-specific implementations for
 |     hooks related to heap allocation.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2016  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#include <bifrost_private.h>

static volatile UINT32 gHostMemMapped = 0;
static UINT64 gHostMemPhyAddrBase = 0;

BOOL mapHostmem()
{
    BOOL ret = FALSE;
    if(hw_get_platform() == CHIP_PLATFORM)
    {
        if(gHostMemMapped)
        {
            ret = TRUE;
        }
        else
        {
            UINT64 hostmemPhyAddrBase = 0;
            BOOL32 succeed = FALSE;
            if(hw_host_notify_hostmem_alloc(&hostmemPhyAddrBase, &succeed) && succeed)
            {
                gHostMemPhyAddrBase = hostmemPhyAddrBase;
                gHostMemMapped = 1;
                ret = TRUE;
            }
            else
            {
                UINT32 hostMemMapped = 0;
                hw_assert(hostMemMapped);
                ret = FALSE;
            }
        }
    }
    return ret;

}
    
UINT64 hw_addr_hostmemVirtualToPhysical(UINT32 virtualAddr)
{
    if(mapHostmem())
    {
        UINT64 offset = (UINT64)(virtualAddr - HW_HEAP_HOSTMEM_CHIP_PLAT_BASE);
        return gHostMemPhyAddrBase + offset;
    }
    else
    {
        return 0;
    }
}

UINT32 hw_addr_hostmemPhysicalToVirtual(UINT64 physicalAddr)
{
    if(mapHostmem())
    {
        UINT32 offset = (UINT32)(physicalAddr - gHostMemPhyAddrBase);
        return HW_HEAP_HOSTMEM_CHIP_PLAT_BASE + offset;
    }
    else
    {
        return 0;
    }
}


HW_HEAP_TYPE sys_mapHeapType(HW_HEAP_TYPE type)
{
    // In BASRAM_HOSTMEM mode, force DRAM
    // allocations to host memory
#if defined(BASRAM_HOSTMEM)
    if(type == HW_HEAP_TYPE_DRAM)
    {
        type = HW_HEAP_TYPE_HOSTMEM;
    }
#endif
    
    // Handle 'default' type
    if(type == HW_HEAP_TYPE_DEFAULT)
    {
    #if defined(BASRAM_HOSTMEM)
        type = HW_HEAP_TYPE_HOSTMEM;
    #elif defined(BASRAM_ONLY)
        type = HW_HEAP_TYPE_SRAM0;
    #else
        type = HW_HEAP_TYPE_DRAM;
    #endif
    }

    if(type == HW_HEAP_TYPE_HOSTMEM)
    {
#if defined(TEST_FAIL_ON_HOSTMEM_ALLOC)    
        UINT32 hostMemAllowed = 0;
        hw_assert(hostMemAllowed);
#else
        mapHostmem();
#endif
    }

    if(type == HW_HEAP_TYPE_TCM_CUR_CORE)
    {
        UINT32 core = hw_getCoreNum();        
        if(core < HW_CORE_VFP_BEGIN)
        {
            type = (HW_HEAP_TYPE)(HW_HEAP_TYPE_TCM_SFP_BEGIN + core);
        }
        else
        {
            type = (HW_HEAP_TYPE)(HW_HEAP_TYPE_TCM_VFP_BEGIN + (core - HW_CORE_VFP_BEGIN));
        }
    }
    return type;
}

HW_HEAP_BLOCK sys_getHeapBlockID(HW_HEAP_TYPE type)
{
    HW_HEAP_BLOCK heapBlock = HW_HEAP_BLOCK_INVALID;
    if(type == HW_HEAP_TYPE_DRAM)
    {
        heapBlock = HW_HEAP_BLOCK_DRAM;
    }
    else if(type == HW_HEAP_TYPE_DRAM_PROTECTED)
    {
        heapBlock = HW_HEAP_BLOCK_DRAM_PROTECTED;
    }
    else if(type == HW_HEAP_TYPE_SRAM0)
    {
        heapBlock = HW_HEAP_BLOCK_SRAM0;
    }
    else if(type == HW_HEAP_TYPE_SRAM1)
    {
        heapBlock = HW_HEAP_BLOCK_SRAM1;
    }
    else if(type == HW_HEAP_TYPE_SRAM2)
    {
        heapBlock = HW_HEAP_BLOCK_SRAM2;
    }
    else if (type == HW_HEAP_TYPE_HOSTMEM)
    {
        heapBlock = HW_HEAP_BLOCK_HOSTMEM;
    }
    else if((type >= HW_HEAP_TYPE_TCM_SFP_BEGIN) && (type < HW_HEAP_TYPE_TCM_VFP_BEGIN))
    {
        heapBlock = (HW_HEAP_BLOCK)(HW_HEAP_BLOCK_TCM_SFP_BEGIN + ((type - HW_HEAP_TYPE_TCM_SFP_BEGIN) / 2));
    }
    else if(type <= HW_HEAP_TYPE_TCM_END)
    {
        heapBlock = (HW_HEAP_BLOCK)(HW_HEAP_BLOCK_TCM_VFP_BEGIN + (type - HW_HEAP_TYPE_TCM_VFP_BEGIN));
    }
    return heapBlock;
}

void* sys_translateAllocAddr(HW_HEAP_TYPE type, void* addr)
{
#if defined(__XTENSA__)  
    if(((type >= HW_HEAP_TYPE_TCM_SFP_BEGIN) && (type < HW_HEAP_TYPE_TCM_VFP_BEGIN))
        && (((type - HW_HEAP_TYPE_TCM_SFP_BEGIN) % 2) == 1))
    {
        addr = (void*)((UINTPTR)addr + HW_HEAP_TCM_OFFSET_DISTANCE);
    }
#endif
    
    return addr;
}

void* sys_untranslateAllocAddr(HW_HEAP_TYPE type, void* addr)
{
#if defined(__XTENSA__)
    if(((type >= HW_HEAP_TYPE_TCM_SFP_BEGIN) && (type < HW_HEAP_TYPE_TCM_VFP_BEGIN))
        && (((type - HW_HEAP_TYPE_TCM_SFP_BEGIN) % 2) == 1))

    {
        addr = (void*)((UINTPTR)addr - HW_HEAP_TCM_OFFSET_DISTANCE);
    }
#endif
    if((type == HW_HEAP_TYPE_DRAM && ((UINTPTR)addr >= HW_ADDR_DRAM_UNCACHED_BASE)) ||
       (type == HW_HEAP_TYPE_SRAM0 && ((UINTPTR)addr >= HW_ADDR_SRAM0_UNCACHED_BASE)) ||
       (type == HW_HEAP_TYPE_SRAM1 && ((UINTPTR)addr >= HW_ADDR_SRAM1_UNCACHED_BASE)) ||
       (type == HW_HEAP_TYPE_SRAM2 && ((UINTPTR)addr >= HW_ADDR_SRAM2_UNCACHED_BASE)))
    {
        addr = hw_cached(addr);
    }
    
    return addr;
}

