/*-------------------------------------------------------
 |
 |      bifrost_system_api_xtensa.h
 |
 |      Contains project-specific Bifrost API
 |      for the 'xtensa' archtype
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2016  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#ifndef _BIFROST_SYSTEM_API_XTENSA_H
#define _BIFROST_SYSTEM_API_XTENSA_H

#include <bifrost.h>

//
// Caching support detection based on address
//
static inline BOOL isCachedAddress(UINTPTR addr)
{
    return (addr >= 0x20000000 && addr < 0x40000000) || (addr >= 0x40000000 && addr < 0xA0000000);
}


//
// Memory type functions
//
static inline HW_HEAP_TYPE hw_getTCMHeapType(HW_SN node, HW_SN_TCM_BLOCK block)
{
    if((node >= HW_SN_CNT) || (block >= HW_SN_TCM_CNT))
    {
        hw_errmsg("ERROR: %s, invalid request: node(%d), block(%d)\n", __func__, node, block);
        return HW_HEAP_TYPE_INVALID;
    }

    return (HW_HEAP_TYPE)(HW_HEAP_TYPE_TCM_SFP_BEGIN + (node - HW_SN_SFP_BEGIN) * 2 + block);
}

static inline HW_MEM_TYPE hw_mem_type(void * const p)
{
    const UINTPTR up = (UINTPTR)p;
    if ((up - HW_ADDR_DRAM_CACHED_BASE) < HW_ADDR_DRAM_SIZE)
    {
        return HW_MEM_TYPE_DRAM;
    }
    else if ((up - HW_ADDR_DRAM_UNCACHED_BASE) < HW_ADDR_DRAM_SIZE)
    {
        return HW_MEM_TYPE_DRAM_UC;
    }
    else if ((up - HW_ADDR_SRAM0_CACHED_BASE) == (up & HW_ADDR_SRAM0_OFFSET_MASK) ||
             (up - HW_ADDR_SRAM1_CACHED_BASE) == (up & HW_ADDR_SRAM1_OFFSET_MASK) ||
             (up - HW_ADDR_SRAM2_CACHED_BASE) == (up & HW_ADDR_SRAM2_OFFSET_MASK))
    {
        return HW_MEM_TYPE_SRAM;
    }
    else if ((up - HW_ADDR_SRAM0_UNCACHED_BASE) == (up & HW_ADDR_SRAM0_OFFSET_MASK) ||
             (up - HW_ADDR_SRAM1_UNCACHED_BASE) == (up & HW_ADDR_SRAM1_OFFSET_MASK) ||
             (up - HW_ADDR_SRAM2_UNCACHED_BASE) == (up & HW_ADDR_SRAM2_OFFSET_MASK))
    {
        return HW_MEM_TYPE_SRAM_UC;
    }
    else if ((up - HW_ADDR_HOSTMEM_CACHED_BASE) == (up & HW_ADDR_HOSTMEM_OFFSET_MASK))
    {
        return HW_MEM_TYPE_HOSTMEM;
    }
    else if ((up - HW_ADDR_HOSTMEM_UNCACHED_BASE) == (up & HW_ADDR_HOSTMEM_OFFSET_MASK))
    {
        return HW_MEM_TYPE_HOSTMEM_UC;
    }
    else if ((up - HW_HEAP_TCM_OFFSET_0) == (up & ((HW_HEAP_TCM_SIZE_SFP) - 1)))
    {
        return HW_MEM_TYPE_TCM;
    }
    else
    {
        return HW_MEM_TYPE_INVALID;
    }
}

//
// Atomic operations
//

static inline BOOL hw_atomicsSupported(VOID* const p)
{
    BOOL ret = FALSE;
    if(hw_pTestConfigs->bfinit.REAL_FABRIC || hw_pTestConfigs->bfinit.STANDALONE)
    {
        switch (hw_mem_type(p))
        {
        case HW_MEM_TYPE_SRAM:
        case HW_MEM_TYPE_SRAM_UC:
        case HW_MEM_TYPE_TCM:
            ret = TRUE;
            break;
        default:
            break;
        }
    }
    return ret;
}

extern unsigned _TIE_xt_sync_L32AI(const unsigned * p, immediate o);
extern void _TIE_xt_sync_S32RI(unsigned c, unsigned * p, immediate o);
#if (defined __cplusplus)

template <const unsigned int offset>
static inline UINT32 hw_load_acq(const UINT32* const p)
{
    return _TIE_xt_sync_L32AI(reinterpret_cast<const unsigned int*>(p), offset);
}

template <const unsigned int offset>
static inline INT32 hw_load_acq(const INT32* const p)
{
    return _TIE_xt_sync_L32AI(reinterpret_cast<const unsigned int*>(p), offset);
}

static inline INT32 hw_load_acq(const INT32* const p)
{
    return _TIE_xt_sync_L32AI((const unsigned int*)(p), 0);
}

template <const unsigned int offset>
static inline void hw_store_rel(UINT32* const p, const UINT32 v)
{
    _TIE_xt_sync_S32RI(v, reinterpret_cast<unsigned int*>(p), offset);
}

template <const unsigned int offset>
static inline void hw_store_rel(INT32* const p, const INT32 v)
{
    _TIE_xt_sync_S32RI(v, reinterpret_cast<unsigned int*>(p), offset);
}

static inline void hw_store_rel(INT32* const p, const INT32 v)
{
    _TIE_xt_sync_S32RI(v, (unsigned int*)(p), 0);
}

#endif // (defined __cplusplus)

static inline UINT32 hw_load_acq(const UINT32* const p)
{
    return _TIE_xt_sync_L32AI((const unsigned int*)(p), 0);
}

static inline void hw_store_rel(UINT32* const p, const UINT32 v)
{
    _TIE_xt_sync_S32RI(v, (unsigned int*)(p), 0);
}

//
// Address translation functions
//

// From: cached DRAM address   (0x40000000-0x9FFFFFFF)
// To:   uncached DRAM address (0xA0000000-0xFFFFFFFF)
static inline void* hw_addr_dramCachedToUncached(void* cachedAddr)
{
    if(cachedAddr != NULL)
    {
        if (((UINTPTR)cachedAddr - HW_ADDR_DRAM_CACHED_BASE) < HW_ADDR_DRAM_SIZE)
        {
            return (void*)(HW_ADDR_DRAM_UNCACHED_BASE + ((UINTPTR)cachedAddr - HW_ADDR_DRAM_CACHED_BASE));
        }
    }
    
    return cachedAddr;
}

// From: uncached DRAM address (0xA0000000-0xFFFFFFFF)
// To:   cached DRAM address   (0x40000000-0x9FFFFFFF)
static inline void* hw_addr_dramUncachedToCached(void* uncachedAddr)
{
    if(uncachedAddr != NULL)
    {
        if (((UINTPTR)uncachedAddr - HW_ADDR_DRAM_UNCACHED_BASE) < HW_ADDR_DRAM_SIZE)
        {
            return (void*)(HW_ADDR_DRAM_CACHED_BASE + ((UINTPTR)uncachedAddr - HW_ADDR_DRAM_UNCACHED_BASE));
        }
    }
    
    return uncachedAddr;
}

// From: cached SRAM address   (0x20100000-0x201BFFFF)
// To:   uncached SRAM address (  0x100000-  0x1BFFFF)
static inline void* hw_addr_sramCachedToUncached(void* cachedAddr)
{
    if(cachedAddr != NULL)
    {
        return (void*)((UINTPTR)cachedAddr & ~HW_SRAM_CACHE_MASK);
    }
    else
    {
        return cachedAddr;
    }
}

// From: uncached SRAM address (  0x100000-  0x1BFFFF)
// To:   cached SRAM address   (0x20100000-0x201BFFFF)
static inline void* hw_addr_sramUncachedToCached(void* uncachedAddr)
{
    if(uncachedAddr != NULL)
    {
        return (void*)((UINTPTR)uncachedAddr | HW_SRAM_CACHE_MASK);
    }
    else
    {
        return uncachedAddr;
    }
}

// From: cached HOSTMEM address   (0x3E000000-0x3FFFFFFF)
// To:   uncached HOSTMEM address (0x1E000000-0x1FFFFFFF)
static inline void* hw_addr_hostmemCachedToUncached(void* cachedAddr)
{
    if(cachedAddr != NULL)
    {
        return (void*)((UINTPTR)cachedAddr & ~HW_HOSTMEM_CACHE_MASK);
    }
    else
    {
        return cachedAddr;
    }
}

// From: uncached HOSTMEM address (0x1E000000-0x1FFFFFFF)
// To:   cached HOSTMEM address   (0x3E000000-0x3FFFFFFF)
static inline void* hw_addr_hostmemUncachedToCached(void* uncachedAddr)
{
    if(uncachedAddr != NULL)
    {
        return (void*)((UINTPTR)uncachedAddr | HW_HOSTMEM_CACHE_MASK);
    }
    else
    {
        return uncachedAddr;
    }
}

// From: global TCM address
//           SFP: 0x02080000+NODE_BASE*NODE_NUM ~ 0x020FFFFF+NODE_BASE*NODE_NUM
//                0x02010000+NODE_BASE*NODE_NUM ~ 0x0217FFFF+NODE_BASE*NODE_NUM (alias)
//           VFP: 0x02080000+NODE_BASE*NODE_NUM ~ 0x0209FFFF+NODE_BASE*NODE_NUM
//                0x02010000+NODE_BASE*NODE_NUM ~ 0x0211FFFF+NODE_BASE*NODE_NUM
// To:     local TCM address used by core
//           SFP: 0x80000- 0xFFFFF
//           VFP: 0x80000- 0x9FFFF
static inline void* hw_addr_tcmGlobalToLocal_core(void* globalAddr)
{
    if(globalAddr != NULL)
    {
        UINTPTR inNodeOffset = (UINTPTR)(globalAddr) & HW_ADDR_NODE_LOCAL_OFFSET_MASK;
        UINTPTR ret = inNodeOffset;
        
        UINTPTR lowBound = 0x80000;
        UINTPTR highBound = 0xFFFFF;
        
        UINT32 core = hw_getCoreNum();
        if(core >= HW_CORE_VFP_BEGIN)
        {
            highBound = 0x9FFFF;
        }
        
        if(inNodeOffset >= HW_HEAP_TCM_OFFSET_1)
        {
            ret = inNodeOffset - HW_HEAP_TCM_OFFSET_DISTANCE;
        }
        
        if((ret >= lowBound) && (ret <= highBound))
        {
            return (void*)(ret);
        }
        else
        {
            hw_errmsg("%s: invalid global TCM address(0x%x) for this core(%d)\n", __func__, (UINTPTR)globalAddr, core);
            return NULL;
        }
    }
    else
    {
        return globalAddr;
    }
}

// From: global TCM address
//           SFP: 0x02080000+NODE_BASE*NODE_NUM ~ 0x020FFFFF+NODE_BASE*NODE_NUM
//                0x02010000+NODE_BASE*NODE_NUM ~ 0x0217FFFF+NODE_BASE*NODE_NUM (alias)
//           VFP: 0x02080000+NODE_BASE*NODE_NUM ~ 0x0209FFFF+NODE_BASE*NODE_NUM
//                0x02010000+NODE_BASE*NODE_NUM ~ 0x0211FFFF+NODE_BASE*NODE_NUM
// To:   local TCM address used by CDMA
//           SFP: 0x02080000~ 0x020FFFFF for shared TCM,  0x02100000 ~ 0x0217FFFF for shared TCM alias
//           VFP: 0x02080000~ 0x0209FFFF for TCM_0, 0x02100000~ 0x0211FFFF for TCM_1
static inline void* hw_addr_tcmGlobalToLocal_CDMA(void* globalAddr)
{
    if(globalAddr != NULL)
    {
        UINTPTR localAddr = HW_ADDR_NODE_LOCAL_BASE + ((UINTPTR)(globalAddr) & HW_ADDR_NODE_LOCAL_OFFSET_MASK);

        UINTPTR lowBound = 0x02080000;
        UINTPTR highBound = 0x0217FFFF;

        if((localAddr >= lowBound) && (localAddr <= highBound))
        {
            return (void*)(localAddr);
        }
        else
        {
            hw_errmsg("%s: invalid global TCM address(0x%x)\n", __func__, (UINTPTR)globalAddr);
            return NULL;
        }
    }
    else
    {
        return globalAddr;
    }
}


// From: local TCM address used by core
//           SFP: 0x80000- 0xFFFFF
//           VFP: 0x80000- 0x9FFFF
// To:   global TCM address
//           SFP: 0x02080000+NODE_BASE*NODE_NUM ~ 0x020FFFFF+NODE_BASE*NODE_NUM
//                0x02010000+NODE_BASE*NODE_NUM ~ 0x0217FFFF+NODE_BASE*NODE_NUM (alias)
//           VFP: 0x02080000+NODE_BASE*NODE_NUM ~ 0x0209FFFF+NODE_BASE*NODE_NUM
//                0x02010000+NODE_BASE*NODE_NUM ~ 0x0211FFFF+NODE_BASE*NODE_NUM
static inline void* hw_addr_tcmLocalToGlobal_core(HW_CORE core, void* localAddr)
{
    if(localAddr != NULL)
    {
        UINTPTR inNodeOffset = (UINTPTR)localAddr;
        UINTPTR ret = inNodeOffset;

        UINTPTR lowBound = 0x80000;
        UINTPTR highBound = 0xFFFFF;
        if(core == HW_CORE_CURRENT)
        {
            core = (HW_CORE)hw_getCoreNum();
        }

        if(core >= HW_CORE_VFP_BEGIN)
        {
            highBound = 0x9FFFF;
        }

        if((inNodeOffset < lowBound) || (inNodeOffset > highBound))
        {
            hw_errmsg("%s: invalid local TCM address(0x%x)\n", __func__, (UINTPTR)localAddr);
            return NULL;
        }

        if((core % 2) == 1)
        {
            inNodeOffset += HW_HEAP_TCM_OFFSET_DISTANCE;
        }

        UINTPTR globalBase = HW_SYS_NODE_BASE + HW_SYS_NODE_SIZE * (core /2);
        return (void*)(globalBase + (inNodeOffset & HW_ADDR_NODE_LOCAL_OFFSET_MASK));
    }
    else
    {
        return localAddr;
    }
}

// From: local TCM address used by CDMA
//           SFP:   0x02080000~ 0x020FFFFF for shared TCM,  0x02100000 ~ 0x0217FFFF for shared TCM alias
//           VFP:   0x02080000~ 0x0209FFFF for TCM_0, 0x02100000~ 0x0211FFFF for TCM_1
// To:   global TCM address
//           SFP: 0x02080000+NODE_BASE*NODE_NUM ~ 0x020FFFFF+NODE_BASE*NODE_NUM
//                0x02010000+NODE_BASE*NODE_NUM ~ 0x0217FFFF+NODE_BASE*NODE_NUM (alias)
//           VFP: 0x02080000+NODE_BASE*NODE_NUM ~ 0x0209FFFF+NODE_BASE*NODE_NUM
//                0x02010000+NODE_BASE*NODE_NUM ~ 0x0211FFFF+NODE_BASE*NODE_NUM
static inline void* hw_addr_tcmLocalToGlobal_CDMA(HW_SN node, void* localAddr)
{
    if(localAddr != NULL)
    {
        UINTPTR lowBound = 0x02080000;
        UINTPTR highBound = 0x0217FFFF;

        if(node == HW_SN_CURRENT)
        {
            node = (HW_SN)hw_getSuperNodeNum();
        }

        if(((UINTPTR)localAddr < lowBound) && ((UINTPTR)localAddr > highBound))
        {
            hw_errmsg("%s: invalid local TCM address(0x%x) for cDMA\n", __func__, (UINTPTR)localAddr);
            return NULL;
        }
        else
        {
            UINTPTR globalBase = HW_SYS_NODE_BASE + HW_SYS_NODE_SIZE * node;
            return (void*)(globalBase + ((UINTPTR)localAddr & HW_ADDR_NODE_LOCAL_OFFSET_MASK));
        }
    }
    else
    {
        return localAddr;
    }
}



// From: local node address  ( 0x2000000- 0x21FFFFF)
// To:   global node address
//       which starts at ( 0x2200000 + (0x200000 * (core number / 2))), within size of 0x200000
static inline void* hw_addr_nodeLocalToGlobal(void* localAddr)
{
    UINTPTR globalBase = HW_SYS_NODE_BASE + HW_SYS_NODE_SIZE * (hw_getSuperNodeNum());
    return (void*)(globalBase + ((UINTPTR)localAddr & HW_ADDR_NODE_LOCAL_OFFSET_MASK));
}

// From: global node address
//       which starts at (0x2200000 + (0x200000 * (core number / 2))), within size of 0x200000
// To:   local node address ( 0x2000000- 0x21FFFFF)
static inline void* hw_addr_nodeGlobalToLocal(void* globalAddr)
{
    return (void*)(HW_ADDR_NODE_LOCAL_BASE + ((UINTPTR)(globalAddr) & HW_ADDR_NODE_LOCAL_OFFSET_MASK));
}

static inline UINTPTR hw_addr_globalNodeBase(HW_SN node)
{
    return (HW_SYS_NODE_BASE + HW_SYS_NODE_SIZE * node);
}

static inline void* hw_uncached(void* const p)
{
    switch (hw_mem_type(p))
    {
    case HW_MEM_TYPE_DRAM:
        return hw_addr_dramCachedToUncached(p);
    case HW_MEM_TYPE_SRAM:
        return hw_addr_sramCachedToUncached(p);
    case HW_MEM_TYPE_HOSTMEM:
        return hw_addr_hostmemCachedToUncached(p);
    default:
        return p;
    }
}

static inline void* hw_cached(void* const p)
{
    switch (hw_mem_type(p))
    {
    case HW_MEM_TYPE_DRAM_UC:
        return hw_addr_dramUncachedToCached(p);
    case HW_MEM_TYPE_SRAM_UC:
        return hw_addr_sramUncachedToCached(p);
    case HW_MEM_TYPE_HOSTMEM_UC:
        return hw_addr_hostmemUncachedToCached(p);
    default:
        return p;
    }
}

EXTERN_C UINT64 hw_addr_hostmemVirtualToPhysical(UINT32 virtualAddr);
EXTERN_C UINT32 hw_addr_hostmemPhysicalToVirtual(UINT64 physicalAddr);

//
// Specialized read/write functions
//
static inline UINT128 hw_read128(UINT128* addr)
{
#if defined(xtensa_vfp_TIE) || defined(xtensa_sfp_TIE)
    return *addr;
#else
    UINT32 i;
    UINT128 ret;
    for(i = 0; i < 4; i++)
    {
        ret.data[i] = hw_read32(&addr->data[i]);
    }
    return ret;
#endif
}

static inline void hw_write128(UINT128* addr, UINT128 data)
{
#if defined(xtensa_vfp_TIE) || defined(xtensa_sfp_TIE)
    *addr = data;
#else
    UINT32 i;
    for(i = 0; i < 4; i++)
    {
        hw_write32(&addr->data[i], data.data[i]);
    }
#endif
}

#endif // _BIFROST_SYSTEM_API_XTENSA_H

