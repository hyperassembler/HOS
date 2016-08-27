/*-----------------------------------------------------
 |
 |      td_alloc.c
 |
 |  Contains malloc functions for the Bifrost test
 |  driver, which respond to the alloc API called
 |  by tests.
 |
 |------------------------------------------------------
 |
 |    Copyright (C) 2011-2013  Microsoft Corporation
 |    All Rights Reserved
 |    Confidential and Proprietary
 |
 |------------------------------------------------------
 */
 
#include "bifrost_private.h"

//
// td_alloc handler function
//
void td_alloc_reply(HW_HEAP_BLOCK heapBlockID, PTR_FIELD size, PTR_FIELD pattern, PTR_FIELD mask, HW_TESTID testID, HW_MAILBOX* box, UINT32 core)
{
    //
    // Make allocation based on parameters.
    //
    PTR_FIELD lAddr = (PTR_FIELD)(UINTPTR)td_allocAligned(heapBlockID, size, pattern, mask, testID, core);
    
    //
    // Compose reply message
    //
    HW_MESSAGE reply;
    PTR_FIELD* data = (PTR_FIELD*)reply.data;
    *data = lAddr;
    reply.metadata.command = MSG_TD_ACK_CMD;
    reply.metadata.size = sizeof(PTR_FIELD);
    placeMessage(box, &reply);
}

//
//
// Testdriver Memory allocation functions
//
//

//
// The constant ALLOC_BLOCK_ALIGN 
// sets the size granularity of allocated blocks
// and also the minimum unconstrained alignment of allocated blocks
//
#define ALLOC_BLOCK_ALIGN        (32)

allocBlock heapAllocBlock[HW_HEAP_BLOCK_CNT] __attribute__ ((section(".ddr_wb.bss")));

//--------------------------------------------
// 
//     init_heap_blocklist()
// 
//     Initializes allocation block for
//     Bifrost's heap.
//
HW_ERROR_CODE init_heap_blocklist(UINT32 heapBlockID)
{
    HW_ERROR_CODE hr = HW_S_OK;
    
    heapAllocBlock[(UINT32)heapBlockID].next_unused = 0;
    hw_printf(HW_ALLOC_VERBOSE,  "%s:\n", __func__ );
    
    //
    // RESERVED_MEM_BYTES not used since this allocation
    // block can't return 0 anyway.
    //
    heapAllocBlock[heapBlockID].block_list = new_blist_node(hw_heapBlockInfo[heapBlockID].base,
                                                            hw_heapBlockInfo[heapBlockID].size,
                                                            &heapAllocBlock[heapBlockID] );
    
    if ( NULL == heapAllocBlock[heapBlockID].block_list )
    {
        hr = HW_E_OTHER;
    }
    else
    {
        heapAllocBlock[heapBlockID].unusedAddr = hw_heapBlockInfo[heapBlockID].base;
        hr = HW_S_OK;
    }
    
    return hr;
}  /*  init_heap_blocklist()  */

//--------------------------------------------------- 
//
//   td_free()
//
//  Frees an previously allocated block.
//
int
td_free(HW_HEAP_BLOCK heapBlockID, PTR_FIELD addr, HW_TESTID testID )
{
    int rc        = HW_S_OK;
    blistNode* bp = NULL;
    
    hw_printf(HW_ALLOC_DEBUG,  "%s:  heapBlockID %d  addr 0x%llx  testID %d\n",
                     __func__, heapBlockID, addr, testID );
    
    if(NULL == (bp = find_block(addr, &heapAllocBlock[heapBlockID])))
    {
        rc = HW_E_NOT_FOUND;
    }
    
    if(NULL == free_block(bp, testID))
    {
        rc = HW_E_OTHER;
    }
    
    return rc;
}  /*  td_free()  */




//--------------------------------------------------- 
// 
//    td_freeAll_reply()
// 
//  Frees all blocks belonging to the given test.
// 
int
td_freeAll(HW_HEAP_BLOCK heapBlockID, HW_TESTID testID)
{
    hw_printf(HW_ALLOC_DEBUG,  "%s: heapBlockID %d  testID %d\n",
                     __func__, heapBlockID, testID );
    
    int rc = HW_S_OK;
    
    rc = free_all_blocks(testID, &heapAllocBlock[heapBlockID]);
    
    return rc;
}

//--------------------------------------------------- 
// 
//    td_freeAll()
// 
//  Frees all allocated heap blocks.
// 
int
td_globalFreeAll()
{
    int rc = HW_S_OK;
    
    for(UINT32 i = 0; i < HW_HEAP_BLOCK_CNT; i++)
    {
        rc = td_freeAll((HW_HEAP_BLOCK)i, HW_INTERNAL_TESTID);
    }
    
    return rc;
}  /*  td_freeAll()  */




//--------------------------------------------------- 
//
//   td_allocAligned()
//
//  Allocate a block.
//
void*
td_allocAligned(HW_HEAP_BLOCK heapBlockID, 
                SIZE_FIELD size, 
                PTR_FIELD pattern, 
                PTR_FIELD mask,
                HW_TESTID testID,
                UINT32 core)
{
    blistNode* bp = NULL;
    void*      rc = NULL;
    
    hw_printf(HW_ALLOC_DEBUG,  "%s:  heapBlockID %d  size 0x%llx  pattern 0x%llx  mask 0x%llx testID %d core %d\n",
                     __func__, heapBlockID, size, pattern, mask, testID, core );
    
    if (   NULL    == heapAllocBlock[heapBlockID].block_list
        && HW_S_OK != init_heap_blocklist(heapBlockID) )
    {
        hw_errmsg( "%s: init_heap_blocklist() failed\n", __func__ );
    }
    else if ( 0 == size )
    {
        hw_errmsg( "%s: got 0 for first parameter \"size\"\n", __func__ );
    }
    else if ( NULL == (bp = find_best_fit( size, pattern, mask, &heapAllocBlock[heapBlockID], ALLOC_BLOCK_ALIGN )) )
    {
        if(hw_bAllocFailuresAreFatal)
        {
            hw_errmsg( "%s: find_best_fit( heapBlockID %d, size 0x%llx, pattern 0x%llx, "
                       "mask 0x%llx ) failed\n",
                        __func__, heapBlockID, size, pattern, mask );
        }
        else
        {
            hw_critical( "WARNING: %s: find_best_fit( heapBlockID %d, size 0x%llx, pattern 0x%llx, "
                         "mask 0x%llx ) failed\n",
                          __func__, heapBlockID, size, pattern, mask );
        }
    }
    else
    {
        bp->owner = testID;
        rc        = (void*)(UINTPTR)bp->addr;
        
        hw_printf(HW_ALLOC_DEBUG,  "%s: find_best_fit( %d  0x%llx  0x%llx  0x%llx ) returned 0x%llx\n",
                         __func__, heapBlockID, size, pattern, mask, bp->addr );

        if((bp->addr + bp->size) > heapAllocBlock[heapBlockID].unusedAddr)
        {
            heapAllocBlock[heapBlockID].unusedAddr = bp->addr + bp->size;
        }       
    }
    
    return rc;
}

SIZE_FIELD td_unusedHeapSpace(HW_HEAP_BLOCK heapBlockID, void** addr)
{
    SIZE_FIELD size = 0;
    if((NULL == heapAllocBlock[heapBlockID].block_list) && (HW_S_OK != init_heap_blocklist(heapBlockID)))
    {
        hw_errmsg( "%s: init_heap_blocklist() failed\n", __func__ );
    }
    else
    {
        *addr = (void*)(UINTPTR)heapAllocBlock[heapBlockID].unusedAddr;
        size = hw_heapBlockInfo[heapBlockID].size - (heapAllocBlock[heapBlockID].unusedAddr - hw_heapBlockInfo[heapBlockID].base);
        hw_printf(HW_ALLOC_DEBUG,  "%s: unused heap space( %d  0x%llx  0x%llx  )\n",
                         __func__, heapBlockID, heapAllocBlock[heapBlockID].unusedAddr, size);
    }
    
    return size;
}

