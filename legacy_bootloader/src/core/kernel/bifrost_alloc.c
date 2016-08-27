/*-----------------------------------------------------
 |
 |      hw_alloc.c
 |
 |------------------------------------------------------
 |
 |  Revision History :
 |
 |    #5 - 15 February 2005 J. Hanes
 |         Drop hw_api_trace_enable; use SHARED_ERRMSG, SHARED_TRACE
 |
 |    #5 - 15 February 2005 J. Hanes
 |         Use SHARED_ERRMSGfor print level control
 |             SHARED_TRACEfor execution trace
 |
 |    #5 - 13 January 2005 D. McDonnell
 |         changed rc var to type void * to support 64 bit targets
 |
 |    #4 - 12 April J. Hanes
 |         Return (void *) 0 on fail, not HW_FAIL.
 |
 |    #3 - 12 January 2004 J. Hanes
 |         Use hw_api_trace_enable to control interface trace.
 |         Use ctp_printf() instead of HW_printf()
 |
 |    #2 - 8 January 2004 J. Hanes
 |         Use stop-on-error to control stop on error.
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2003  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */
#include <bifrost_mem.h>
#include "bifrost_alloc.h"
#include "bifrost_lock.h"
#include "bifrost_print.h"

int32_t allocs;
int32_t frees;

#define HW_HEAP_SIZE 1024*1024*16

char heap[HW_HEAP_SIZE];
char *heap_ptr;

static hw_spin_lock_t _lock;

void hw_alloc_setup()
{
    heap_ptr = &heap[0];
    ke_spin_lock_init(&_lock);
}

void *hw_alloc(size_t size)
{
    hw_irql_t irql;
    irql = ke_spin_lock_raise_irql(&_lock, HW_IRQL_DISABLED_LEVEL);
    void *result = NULL;
    if (heap_ptr + size <= heap + HW_HEAP_SIZE)
    {
        result = heap_ptr;
        heap_ptr += size;
    }
    ke_spin_unlock_lower_irql(&_lock, irql);

    // Alloc profiling
    //if(ke_get_current_core() == 0)
    //    hw_printf("Alloced %d: 0x%X\n", ke_interlocked_increment(&allocs,1), result);
    return result;
}

void hw_free(void *ptr)
{
    // Alloc profiling
//    if(ke_get_current_core() == 0)
//        hw_printf("Freed %d: 0x%X\n", ke_interlocked_increment(&frees,1), ptr);
    return;
}

//
//
////
//// Hooks for preprocessing heap types and blocks,
//// and for translating addresses.
//// To be overridden by project-specific
//// functions, if necessary.
////
//HW_HEAP_TYPE sys_mapHeapType(HW_HEAP_TYPE type) __attribute__ ((weak));
//HW_HEAP_TYPE sys_mapHeapType(HW_HEAP_TYPE type)
//{
//    return type;
//}
//void* sys_translateAllocAddr(HW_HEAP_TYPE type, void* addr) __attribute__ ((weak));
//void* sys_translateAllocAddr(HW_HEAP_TYPE type, void* addr)
//{
//    return addr;
//}
//void* sys_untranslateAllocAddr(HW_HEAP_TYPE type, void* addr) __attribute__ ((weak));
//void* sys_untranslateAllocAddr(HW_HEAP_TYPE type, void* addr)
//{
//    return addr;
//}
//
////
//// Modifies a size, pattern, and mask so that
//// they all match a certain align boundary.
//// This boundary must be a power of 2.
//// 'description' is printed in the resulting
//// message if a parameter is modified.
////
//void alignAllocationWithName(PTR_FIELD* size,
//                             PTR_FIELD* pattern,
//                             PTR_FIELD* mask,
//                             PTR_FIELD alignValue,
//                             const char* caller,
//                             const char* description)
//{
//    if(*size % alignValue != 0)
//    {
//        hw_printf(HW_ALLOC_DEBUG, "%s: allocation request size 0x%llx not aligned to %s 0x%llx. Bifrost has corrected it, new value is: 0x%llx.\n",
//                  caller, *size, description, alignValue, *size + (alignValue - (*size % alignValue)));
//        *size += (alignValue - (*size % alignValue));
//    }
//
//    if(*pattern % alignValue != 0)
//    {
//        hw_printf(HW_ALLOC_DEBUG, "%s: allocation request pattern 0x%llx not aligned to %s 0x%llx. Bifrost has corrected it, new value is: 0x%llx.\n",
//                  caller, *pattern, description, alignValue, *pattern - (*pattern % alignValue));
//        *pattern -= (*pattern % alignValue);
//    }
//
//    //
//    // This is guaranteed to produce the correct mask because
//    // all allocation correction sizes are powers of 2.
//    //
//    UINTPTR alignMask = alignValue - 1;
//
//    if((*mask & alignMask) != alignMask)
//    {
//        hw_printf(HW_ALLOC_DEBUG, "%s: allocation request mask 0x%llx does not mask sufficient bits for %s 0x%llx. Bifrost has corrected it, new value is: 0x%llx.\n",
//                  caller, *mask, description, alignValue, *mask | alignMask);
//        *mask |= alignMask;
//    }
//}
//
//static HW_RESULT
//find_next_unused_record( HW_ALLOC_TRACKER* allocTracker, HW_ALLOC_RECORD** returnRecord )
//{
//    HW_RESULT rc = HW_E_NOT_FOUND;
//    BOOL found = FALSE;
//    UINT32 ctr = 0;
//
//    HW_ALLOC_RECORD (*allocRecords)[HW_ALLOC_NUM_BLOCKS] = &allocTracker->records;
//    UINT32 next_unused = hw_read32(&allocTracker->next_unused);
//
//    HW_ALLOC_RECORD* currentRecord = NULL;
//
//    while ( !found && ( ctr < HW_ALLOC_NUM_BLOCKS ) )
//    {
//        currentRecord = &(*allocRecords)[next_unused];
//        if ( hw_readptr(&currentRecord->addr) == (UINTPTR)NULL )
//        {
//            found = TRUE;
//            rc = HW_S_OK;
//        }
//
//        next_unused++;
//
//        // Wrap around array if necessary
//        if ( next_unused >= HW_ALLOC_NUM_BLOCKS )
//        {
//            next_unused = 0;
//        }
//
//        ctr++;
//    }
//
//    if(!found)
//    {
//        *returnRecord = NULL;
//        hw_errmsg("%s: no unused record found (max number of allocs reached)!\n", __func__);
//        rc = HW_E_OVERFLOW;
//    }
//    else
//    {
//        *returnRecord = currentRecord;
//        hw_write32(&allocTracker->next_unused, next_unused);
//    }
//
//    return rc;
//}
//
//static HW_RESULT
//find_record_for_address( UINTPTR addr, HW_ALLOC_TRACKER* allocTracker, HW_ALLOC_RECORD** returnRecord )
//{
//    HW_RESULT rc = HW_E_NOT_FOUND;
//    BOOL found = FALSE;
//    UINT32 ctr = 0;
//
//    UINT32 entry_cnt = hw_read32(&allocTracker->count);
//    UINT32 entries_found = 0;
//    HW_ALLOC_RECORD (*allocRecords)[HW_ALLOC_NUM_BLOCKS] = &allocTracker->records;
//
//    HW_ALLOC_RECORD* currentRecord = NULL;
//
//    while ( !found && ctr < HW_ALLOC_NUM_BLOCKS && entries_found < entry_cnt )
//    {
//        currentRecord = &(*allocRecords)[ctr];
//        UINTPTR rec_addr = hw_readptr(&currentRecord->addr);
//
//        if( rec_addr != (UINTPTR)NULL )
//        {
//            if ( rec_addr == addr )
//            {
//                found = TRUE;
//                rc = HW_S_OK;
//            }
//            entries_found++;
//        }
//
//        ctr++;
//    }
//
//    if(!found)
//    {
//        *returnRecord = NULL;
//        hw_errmsg("%s: no record found for addr 0x%x!\n", __func__, addr);
//        rc = HW_E_NOT_FOUND;
//    }
//    else
//    {
//        *returnRecord = currentRecord;
//    }
//
//    return rc;
//}
//
////
//// Function: hw_alloc
////
//// Allocates heap addresses. The addresses
//// returned by this function are already set
//// up in CPU pagetables and are available for
//// immediate use by the caller.
////
//// Parameters:
////    type - Heap type. See <HW_HEAP_TYPE>
////    size - Size of the allocation.
////    pattern, mask - This function guarantees that:
////      <returned address> & mask == pattern & mask
////      To request an exact address set mask to HW_MASK_EXACT_ADDR.
////
//// Returns:
////    The address of the allocated memory block.
////
//// See Also:
////    <hw_free>
////
////
//void* hw_allocAlignedWithType(HW_HEAP_TYPE type,
//                              SIZE_FIELD size,
//                              PTR_FIELD pattern,
//                              PTR_FIELD mask)
//{
//    hw_log(HW_EVENT_CHECKPOINT, (UINTPTR)__func__, (UINTPTR)"enter", 0, 0);
//    //
//    // Get the test ID of the current test
//    //
//    HW_TESTID testID = hw_getMyInstanceID();
//
//    type = sys_mapHeapType(type);
//
//    HW_HEAP_BLOCK heapBlockID = sys_getHeapBlockID(type);
//    if(heapBlockID == HW_HEAP_BLOCK_INVALID)
//    {
//        hw_errmsg("%s, invalid heap type (%d)\n", __func__, type);
//        return NULL;
//    }
//
//    void* ret = NULL;
//    HW_ALLOC_TRACKER* allocTracker = (HW_ALLOC_TRACKER*)hw_readptr(&testSlaveVars->allocTracker);
//    HW_ALLOC_RECORD* allocRecord = NULL;
//    HW_RESULT record_find_rc = HW_S_OK;
//
//    if(hw_getCoreNum() == bifrostCachedGlobals.tpid)
//    //
//    // If this thread is the driver, use the
//    // td_allocAligned function.
//    //
//    {
//        ret = td_allocAligned(heapBlockID, size, pattern, mask, testID, bifrostCachedGlobals.tpid);
//        // Find an appropriate alloc record
//        if(1)
//        {
//            record_find_rc = find_next_unused_record(allocTracker, &allocRecord);
//        }
//    }
//    else
//    {
//        //
//        // Align all memory allocations to cacheline
//        // boundaries.
//        //
//        alignAllocation(&size, &pattern, &mask, HW_CACHELINE_SIZE, "cacheline boundary");
//
//        HW_MESSAGE message = {{0}};
//        message.metadata.command = MSG_TS_CMD_ALLOC;
//        message.metadata.size = 5 * sizeof(PTR_FIELD);
//
//        PTR_FIELD* datafield = (PTR_FIELD*) message.data;
//        datafield[0] = (PTR_FIELD)heapBlockID;
//        datafield[1] = size;
//        datafield[2] = pattern;
//        datafield[3] = mask;
//        datafield[4] = testID;
//
//        hw_irql_t prev_irql = hw_raise_irql(HW_IRQL_DISABLED_LEVEL);
//
//        placeMessage(&testSlaveVars->driverMailbox, &message);
//
//        // While we wait for the driver to respond, find
//        // an appropriate alloc record
//        if(1)
//        {
//            record_find_rc = find_next_unused_record(allocTracker, &allocRecord);
//        }
//
//        while(!getMessage(&testSlaveVars->slaveMailbox, &message));
//        ret = (void*)(UINTPTR)*(PTR_FIELD*) message.data;
//
//        hw_lower_irql(prev_irql);
//
//        //
//        // Raise a fatal error if the allocation or
//        // the alloc tracking failed.
//        //
//        if(NULL == ret || record_find_rc != HW_S_OK)
//        {
//            if(hw_bAllocFailuresAreFatal)
//            {
//                hw_errmsg("%s(type=0x%x, size=0x%llx, pattern=0x%llx, mask=0x%llx, testID=%d) failed\n",
//                           __func__, type, size, pattern, mask, testID);
//                hw_fatal();
//            }
//            else
//            {
//                hw_critical("WARNING: %s(type=0x%x, size=0x%llx, pattern=0x%llx, mask=0x%llx, testID=%d) failed\n",
//                             __func__, type, size, pattern, mask, testID);
//            }
//        }
//    }
//
//    // If the allocation succeeded, record it
//    if(ret != NULL)
//    {
//        // Record allocation
//        hw_printf(HW_ALLOC_DEBUG, "%s: recording addr 0x%x size 0x%llx type %d -> 0x%x\n", __func__, ret, size, type, allocRecord);
//
//        hw_writeptr(&allocRecord->addr, (UINTPTR)ret);
//        hw_writeptr(&allocRecord->size, (SIZE_T)size);
//        hw_write32((UINT32*)&allocRecord->type, type);
//        hw_write32(&allocTracker->count, hw_read32(&allocTracker->count) + 1);
//    }
//
//    ret = sys_translateAllocAddr(type, ret);
//
//    hw_log(HW_EVENT_CHECKPOINT, (UINTPTR)__func__, (UINTPTR)"exit", 0, 0);
//    return ret;
//}
//
////
//// Function: hw_free
////
//// Free a previously allocated physical address
////
//// Parameters:
////    type - Heap type. See <HW_HEAP_TYPE>
////    addr - Address of the block to free
////
//// Returns:
////    Nothing
////
//// See Also:
////    <hw_alloc>
////
//void hw_freeWithType(HW_HEAP_TYPE type, void* addr)
//{
//    HW_TESTID testID = hw_getMyInstanceID();
//    HW_ALLOC_TRACKER* allocTracker = (HW_ALLOC_TRACKER*)hw_readptr(&testSlaveVars->allocTracker);
//    HW_ALLOC_RECORD* allocRecord;
//    HW_RESULT record_find_rc = HW_S_OK;
//
//    type = sys_mapHeapType(type);
//    HW_HEAP_BLOCK heapBlockID = sys_getHeapBlockID(type);
//    if(heapBlockID == HW_HEAP_BLOCK_INVALID)
//    {
//        hw_errmsg("%s, invalid heap type (%d)\n", __func__, type);
//        return;
//    }
//
//    addr = sys_untranslateAllocAddr(type, addr);
//
//    // Record allocation, and
//    // use this information to invalidate cache if necessary.
//    // Kernel code only frees at a point where cache invalidation
//    // is no longer required.
//    if(1)
//    {
//        record_find_rc = find_record_for_address((UINTPTR)addr, allocTracker, &allocRecord);
//
//        if(record_find_rc == HW_S_OK)
//        {
//            if(hw_heapBlockInfo[type].cached)
//            {
//                hw_cacheInvalidateBuffer((void*)hw_readptr(&allocRecord->addr), hw_readptr(&allocRecord->size));
//            }
//
//            // Clear allocation record
//            hw_writeptr(&allocRecord->addr, (UINTPTR)NULL);
//            hw_write32(&allocTracker->count, hw_read32(&allocTracker->count) - 1);
//        }
//    }
//
//    if(hw_getCoreNum() == bifrostCachedGlobals.tpid)
//    //
//    // If this thread is the driver, use the
//    // td_free function.
//    //
//    {
//        td_free(heapBlockID, (UINTPTR)addr, testID);
//    }
//    else
//    {
//        HW_MESSAGE message = {{0}};
//        message.metadata.command = MSG_TS_CMD_FREE;
//        message.metadata.size = 3 * sizeof(PTR_FIELD);
//        PTR_FIELD* datafield = (PTR_FIELD*) message.data;
//
//        datafield[0] = (PTR_FIELD) heapBlockID;
//        datafield[1] = (UINTPTR)addr;
//        datafield[2] = testID;
//
//        placeMessage(&testSlaveVars->driverMailbox, &message);
//    }
//}
//
////
//// Free all previously allocated heap addresses
////
//void hw_freeAllWithType(HW_HEAP_TYPE type)
//{
//    HW_TESTID testID = hw_getMyInstanceID();
//    HW_ALLOC_TRACKER* allocTracker = (HW_ALLOC_TRACKER*)hw_readptr(&testSlaveVars->allocTracker);
//    HW_ALLOC_RECORD (*allocRecords)[HW_ALLOC_NUM_BLOCKS] = &allocTracker->records;
//
//    type = sys_mapHeapType(type);
//    HW_HEAP_BLOCK heapBlockID = sys_getHeapBlockID(type);
//    if(heapBlockID == HW_HEAP_BLOCK_INVALID)
//    {
//        hw_errmsg("%s, invalid heap type (%d)\n", __func__, type);
//        return;
//    }
//
//    // Record allocation, and
//    // use this information to invalidate cache if necessary.
//    // Kernel code only frees at a point where cache invalidation
//    // is no longer required.
//    if(1)
//    {
//        HW_ALLOC_RECORD* currentRecord;
//        UINT32 entry_cnt = hw_read32(&allocTracker->count);
//        UINT32 entries_found = 0;
//        UINT32 entries_freed = 0;
//        for ( UINT32 ctr = 0; ctr < HW_ALLOC_NUM_BLOCKS && entries_found < entry_cnt; ctr++ )
//        {
//            currentRecord = &(*allocRecords)[ctr];
//            UINTPTR addr = hw_readptr(&currentRecord->addr);
//            if( addr != (UINTPTR)NULL )
//            {
//                if( (HW_HEAP_TYPE)hw_read32((UINT32*)&currentRecord->type) == type )
//                {
//                    if(hw_heapBlockInfo[type].cached)
//                    {
//                        hw_cacheInvalidateBuffer((void*)addr, hw_readptr(&currentRecord->size));
//                    }
//                    // Clear allocation record
//                    hw_writeptr(&currentRecord->addr, (UINTPTR)NULL);
//                    entries_freed++;
//                }
//                entries_found++;
//            }
//        }
//        hw_write32(&allocTracker->count, entry_cnt - entries_freed);
//    }
//
//    if(hw_getCoreNum() == bifrostCachedGlobals.tpid)
//    //
//    // If this thread is the driver, use the
//    // td_freeAll function.
//    //
//    {
//        td_freeAll(heapBlockID, testID);
//    }
//    else
//    {
//        HW_MESSAGE message = {{0}};
//        message.metadata.command = MSG_TS_CMD_FREE_ALL;
//        message.metadata.size = 2 * sizeof(PTR_FIELD);
//
//        PTR_FIELD* datafield = (PTR_FIELD*) message.data;
//        datafield[0] = (PTR_FIELD)heapBlockID;
//        datafield[1] = testID;
//
//        placeMessage(&testSlaveVars->driverMailbox, &message);
//    }
//}
//
//void hw_setAllocFailuresNonFatal()
//{
//    hw_bAllocFailuresAreFatal = FALSE;
//}
//
