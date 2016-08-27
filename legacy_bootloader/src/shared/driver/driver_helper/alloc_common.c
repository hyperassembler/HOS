/*-----------------------------------------------------
 |
 |      alloc_common.c
 |
 |  Contains functions which are shared by
 |  all implementations of alloc in Bifrost
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

//--------------------------------------------
//
//    print_blocklist()
//
//  Goes through allocation nodes one
//  by one and prints them.
//
void
print_blocklist(allocBlock* list)
{
    blistNode* ptr;
    
    if ( !list )
    {
        hw_critical( "%s: block_list NULL\n", __func__);
        return;
    }
    for ( ptr = list->block_list; ptr; ptr = ptr->next )
    {
        print_node( ptr );
    }
}

//--------------------------------------------
//
//     print_node()
//
//  Print out an allocation node
//
void print_node( blistNode* np )
{
    hw_critical( "\tnode  %s  addr 0x%llx  "
                 "size 0x%llx  prev 0x%08x  this 0x%08x  next 0x%08x  owner",
                 (np->free) ? "FREE" : "BUSY", 
                 np->addr, np->size, 
                 np->prev, np, np->next );
    
    switch(np->owner)
    {
    case ALLOC_NO_OWNER:
        hw_critical(" <None>\n");
        break;
    case HW_INTERNAL_TESTID:
        hw_critical(" <Internal>\n");
        break;
    default:
        hw_critical(" %d\n", np->owner);
        break;
    }
}

//--------------------------------------------
//
//     find_next_unused()
//
//  Find the first unused allocation node (in
//  the internal array, not the block list).
//  This function is used when creating new nodes.
//
HW_RESULT
find_next_unused( UINT32* index, allocBlock* block )
{
    HW_RESULT rc = HW_E_NOT_FOUND;
    int found = 0;
    int loop_counter = 0;
    *index = (UINT32)(-1);
    
    while ( !found && ( loop_counter <= ( HW_ALLOC_NUM_BLOCKS + 1 ) ) )
    {
        if ( block->block_array[block->next_unused].size == 0 )
        {
            *index = block->next_unused;
            found = 1;
            rc = HW_S_OK;
        }
        
        block->next_unused++;
        
        // Wrap around array if necessary
        if ( block->next_unused > HW_ALLOC_NUM_BLOCKS )
        {
            block->next_unused = 0;
        }
        
        loop_counter++;
    }
    
    return rc;
    
} /* find_next_unused() */

//--------------------------------------------
//
//     new_blist_node()
//
//  Initializes a new allocation node with
//  the given address and size.
//
blistNode*
new_blist_node( PTR_FIELD remote_addr, 
                PTR_FIELD size,
                allocBlock* block )
{
    blistNode* node_ptr = 0;
    UINT32     index;

    hw_printf(HW_ALLOC_VERBOSE,  "\t%s:  addr 0x%llx size 0x%llx\n", 
                      __func__, remote_addr, size );
    
    if ( HW_S_OK == find_next_unused( &index, block ) )
    {
        node_ptr = &block->block_array[ index ];
        
        node_ptr->next    = 0;
        node_ptr->prev    = 0;
        node_ptr->addr    = remote_addr;
        node_ptr->size    = size;
        node_ptr->free    = 1;
        node_ptr->owner   = ALLOC_NO_OWNER;
    }
    else
    {
        hw_errmsg( "%s: out of blistNodes, %d used\n", 
                    __func__, HW_ALLOC_NUM_BLOCKS );
    }
    
    return node_ptr;
    
}  /*  new_blist_node()  */

//--------------------------------------------
//
//     insert_after()
//
//  Inserts an allocation node after another
//  and sets up the pointers.
//
void
insert_after( blistNode* item, 
              blistNode* after )
{
    hw_printf(HW_ALLOC_VERBOSE,  "\t%s:  item 0x%08x  after 0x%08x\n", 
                       __func__, item, after );
    
    item->next = after->next;
    if ( after->next )
    {
        after->next->prev = item;
    }
    item->prev = after;
    after->next = item;
}

//--------------------------------------------
//
//     find_block()
//
//  Finds and returns a previously allocated block
//  at the given address.
//
blistNode*
find_block( PTR_FIELD addr, allocBlock* block )
{
    hw_printf(HW_ALLOC_VERBOSE,  "%s:  addr: 0x%llx\n",
                       __func__, addr );
    
    blistNode* bp;
    blistNode* find = NULL;
    
    for ( bp = block->block_list; bp && !find; bp = bp->next )
    {
        //
        // If this is the block we're looking for, take it
        //
        if ( (UINTPTR) addr == bp->addr )
        {
            find = bp;
        }
    }
    
    
    if ( NULL == find )
    {
        hw_errmsg( "%s: no block with address 0x%llx\n", 
                   __func__, addr );
    }
    
    return find;
}

//--------------------------------------------
//
//     free_block()
//
//  Frees a previously allocated block and
//  merges it with any adjacent free blocks.
//  
//
//  Returns the address of the newly free block
//  (NOT the heap address).
//
blistNode*
free_block( blistNode* bp, HW_TESTID testID )
{
    if( bp == NULL)
    {
        hw_errmsg( "%s: block is NULL\n", __func__);
    }
    else
    {
        if ( bp->free )
        {
            //
            // Hmmm, it looks like it's already free.
            //
            hw_errmsg( "%s: block at address 0x%llx already free\n",
                        __func__, bp->addr );
        }
        else
        {
            //
            // Don't allow a test to free another test's
            // allocations. But Bifrost internal calls
            // can free anything.
            //
            if ( bp->owner != testID && testID != HW_INTERNAL_TESTID)
            {
                hw_errmsg( "%s: test (ID=%d) tried to free block at address 0x%llx which belongs to test (ID=%d)\n",
                            __func__, testID, bp->addr, bp->owner );
            }
            else
            {
                hw_printf(HW_ALLOC_DEBUG,  "\tFreeing address 0x%llx\n", 
                                 bp->addr  );
                bp->free   = 1;
                bp->owner  = ALLOC_NO_OWNER;
                
                //
                // Coalesce adjacent free blocks
                //
                if ( bp->next && bp->next->free )
                {
                    hw_printf(HW_ALLOC_VERBOSE,  "\tFree, coalesce with next  "
                                      "addr 0x%llx  next addr 0x%llx  "
                                      "size 0x%llx  next size 0x%llx\n",
                                      bp->addr, bp->next->addr, 
                                      bp->size, bp->next->size  );
                    
                    bp = merge_blocks( bp, bp->next );
                }
                
                if ( bp->prev && bp->prev->free )
                {
                    hw_printf(HW_ALLOC_VERBOSE,  "\tFree, coalesce with prev  "
                                      "addr 0x%llx  prev addr 0x%llx  "
                                      "size 0x%llx  prev size 0x%llx\n",
                                      bp->addr, bp->prev->addr,
                                      bp->size, bp->prev->size  );
                    
                    bp = merge_blocks( bp->prev, bp );
                }
            }
        }
    }
    
    return bp;
}

//--------------------------------------------
//
//     free_all_blocks()
//
//  Frees all nodes in an allocation block that were
//  allocated by the given testID.
//  If testID == HW_INTERNAL_TESTID, then free everything.
//
HW_ERROR_CODE free_all_blocks(HW_TESTID testID, allocBlock* block)
{
    hw_printf(HW_ALLOC_VERBOSE,  "%s:  testID %d\n",
                     __func__, testID );
    
    HW_ERROR_CODE rc = HW_S_OK;
    blistNode* ptr;
    
    //
    // Check if block list has been initialized
    // (which would be true if any allocation had
    // been called yet).
    //
    if ( !block->block_list )
    {
        hw_critical( "WARNING %s: block list NULL\n", __func__);
        return HW_E_OTHER;
    }
    
    //
    // Free all blocks belonging to the test, or if
    // testID = HW_INTERNAL_TESTID, free all blocks
    // regardless of owner.
    //
    for ( ptr = block->block_list; ptr; ptr = ptr->next )
    {
        if( ptr->owner == testID || testID == HW_INTERNAL_TESTID )
        {
            //
            // When freeing a block, it could get merged with
            // adjacent free blocks. The free_block function
            // will always return the address of the block it
            // just freed, though.
            //
            ptr = free_block( ptr, testID );
        }
    }
    
    return rc;
}




//--------------------------------------------
//
//     merge_blocks()
//
//  Merge two allocation nodes. The merged node
//  will reside in place of the first node.
//  The attributes of the second node will be lost.
//  Primarily used after free_block when we are
//  merging two free blocks and can assume they
//  have identical attributes.
//
//  Returns the newly merged node.
//
blistNode*
merge_blocks( blistNode* b1, 
              blistNode* b2  )
{
    hw_printf(HW_ALLOC_DEBUG,  "\t%s:  b1 0x%08x  b2 0x%08x\n", 
               __func__, b1, b2 );
    
    b1->size += b2->size;
    b1->next  = b2->next;
    if ( b1->next )
    {
        b1->next->prev = b1;
    }
    
    b2->size  = 0;
    b2->next  = 0;
    b2->prev  = 0;
    b2->addr  = 0;
    b2->size  = 0;
    b2->free  = 1;
    b2->owner = ALLOC_NO_OWNER;
    
    return b1;
}



//--------------------------------------------
//
//         matchCandidateAddr()
//
//  Given an address of a free block, mask, and
//  pattern, find the lowest possible match
//  using a binary search method. This function
//  does not check the upper bound of the block,
//  that check is done separately by the calling
//  function.
//
PTR_FIELD matchCandidateAddr(PTR_FIELD ptrAddr, PTR_FIELD masked_pattern, PTR_FIELD mask)
{
    // Initial candidate is simply the block's
    // address with the masked bits replaced by
    // the bits of the pattern. This will only be
    // used if the next step doesn't produce any
    // results.
    // Example: ptrAddr: 0x11111111, pattern 0x00220022, mask 0x00FF00FF
    // result is 0x11221122
    PTR_FIELD candidate_addr = (( ptrAddr & ~mask) | masked_pattern);
    
    hw_printf(HW_ALLOC_VERBOSE, "%s: ptrAddr: 0x%llx, initial candidate_addr: 0x%llx, masked_pattern: 0x%llx, mask: 0x%llx\n",
                      __func__, ptrAddr, candidate_addr, masked_pattern, mask);
    
    // Next, we want to get the lowest possible candidate address.
    // We'll try to fiddle with the unmasked bits to see if we
    // can make anything match.
    INT32 currentBit = (sizeof(PTR_FIELD) * 8) - 1;
    PTR_FIELD currentBitMask;
    PTR_FIELD modifyVal = 0;
    PTR_FIELD tempModifyVal = 0;
    PTR_FIELD temp_candidate_addr;
    
    // We're going to loop through each bit
    // in the pointer, starting at the left.
    while(currentBit >= 0)
    {
        currentBitMask = 0x1ll << currentBit;
        
        // If the bit is masked we can't touch it, it's fixed.
        if(!(mask & currentBitMask))
        {
            // Our temporary candidate for this round is
            // pattern OR'd with the bit under consideration
            // as well as all the bits we've kept from previous
            // rounds.
            tempModifyVal = modifyVal | currentBitMask;
            temp_candidate_addr = masked_pattern | tempModifyVal;
            
            // If this candidate is larger than the block's address,
            // it could be THE candidate address, take it if we
            // don't find anything lower.
            // Also, we won't keep this bit for future rounds,
            // therefore future candidate addresses will be lower
            // than this one.
            if(temp_candidate_addr >= ptrAddr)
            {
                candidate_addr = temp_candidate_addr;
                
                // If it's an exact match, just take it,
                // it's already the lowest
                if(candidate_addr == ptrAddr)
                {
                    break;
                }
            }
            // If the candidate is less than the block address, it's not
            // valid. We'll keep this bit. It will be included
            // in future rounds so that future candidate addresses
            // are higher than this one.
            else if(temp_candidate_addr < ptrAddr)
            {
                modifyVal = tempModifyVal;
            }
        }
        
        currentBit--;
    }
    
    // Do one more check. If we ran out of unmasked bits
    // to toggle, the last check run by the loop would have
    // been (pattern | <kept bits> | <lowest unmasked bit>).
    // So we also need to try (pattern | <kept bits>) without
    // that last bit. If it produces a matching candidate
    // address then this is the lowest one, not the one the
    // loop produced.
    temp_candidate_addr = masked_pattern | modifyVal;
    if(temp_candidate_addr >= ptrAddr)
    {
        candidate_addr = temp_candidate_addr;
    }
    
    hw_printf(HW_ALLOC_VERBOSE, "%s: identified 0x%llx as candidate_addr\n", __func__, candidate_addr);
    
    return candidate_addr;
}

//--------------------------------------------
// 
//          findNextFreeNode()
//
//  Given an allocation node element, searches the
//  block list for the next free element (which could
//  be the element passed to this function). If found,
//  returns that element. Otherwise, returns 0.
//
blistNode* findNextFreeNode(blistNode* node)
{
    blistNode* ptr = node;
    blistNode* prev = NULL;
    blistNode* find = NULL;
    
    //
    // Scan the list for a free block 
    // that meets the criteria.
    //
    hw_printf(HW_ALLOC_VERBOSE, "%s:\n", __func__);
    while ( ptr != NULL && !ptr->free)
    {
        hw_printf(HW_ALLOC_VERBOSE,  "\tfree: %d ptr->addr: 0x%llx ptr->addr+size: 0x%llx\n", 
                           ptr->free, ptr->addr, ptr->addr + ptr->size);
        
        prev = ptr;
        ptr = ptr->next;
        
        // Check for structure integrity
        if (    prev != 0 && ptr != 0
                &&  prev->addr + prev->size != ptr->addr ) {
            hw_errmsg( "%s: blocks gap or overlap\n", __func__ );
            hw_errmsg( "prev addr 0x%llx size 0x%llx  addr + size 0x%llx\n",
                        prev->addr, prev->size, prev->addr + prev->size);
            hw_errmsg( "curr addr 0x%llx\n", ptr->addr );
            return (blistNode*) NULL;
        }
        
    }
    
    if(ptr != NULL)
    {
        hw_printf(HW_ALLOC_VERBOSE,  "\tfree: %d ptr->addr: 0x%llx ptr->addr+size: 0x%llx\n", 
                           ptr->free, ptr->addr, ptr->addr + ptr->size);
    }
    
    return ptr;
}

//--------------------------------------------
//
//          find_best_fit()
//
//  Traverse the linked list of blocks;
//  find the smallest free block that will satisfy the request;
//  if too big, split the free block to achieve exact fit. 
//
//  return a pointer to the exact_fit block on success
//         NULL on failure
//
blistNode*
find_best_fit( PTR_FIELD size, 
               PTR_FIELD pattern, 
               PTR_FIELD mask,
               allocBlock* block,
               UINT32 blockAlign )
{
    blistNode* ptr;
    blistNode* find = NULL;
    PTR_FIELD  use_pat;
    PTR_FIELD  use_mask;
    PTR_FIELD  masked_pattern;
    PTR_FIELD  candidate_addr;
    PTR_FIELD  foundAddr = 0;
    
    if ( 0 == mask )
    {
        use_pat  = 0;
        use_mask = blockAlign - 1;
    }
    else
    {
        use_pat  = pattern;
        use_mask = mask;
    }
    masked_pattern = use_pat & use_mask;
    
    //
    // Scan the list for a free block 
    // that meets the criteria
    //
    ptr = findNextFreeNode(block->block_list);
    while ( ptr != NULL )
    {
        hw_printf(HW_ALLOC_VERBOSE,  "%s:\n\taddr & ~mask 0x%llx   masked_pattern 0x%llx  free: %d ptr->addr: 0x%llx ptr->addr+size: 0x%llx\n", 
                           __func__, ptr->addr & ~use_mask, masked_pattern, ptr->free, ptr->addr, ptr->addr + ptr->size);
        
        candidate_addr = matchCandidateAddr(ptr->addr, masked_pattern, use_mask);
        
        //
        // If block contains a block that meets alignment constraints
        //
        if (      candidate_addr         >= ptr->addr
             && ( candidate_addr + size) <= ( ptr->addr + ptr->size) )
        {
            
            //
            // if it's an exact fit, take it and bail out of list scan
            //
            if (    candidate_addr == ptr->addr
                 && size           == ptr->size   )
            {
                hw_printf(HW_ALLOC_VERBOSE,  "\tExact fit\n" );
                find = ptr;
                foundAddr = candidate_addr;
                break;
            }
            
            //
            // If it's not an exact fit, but it's a better fit 
            // than the previous best, take it until we find a better. 
            //
            else if ( size < ptr->size )
            {
                hw_printf(HW_ALLOC_VERBOSE,  "   -> Meets, too big ;  " );
                
                if ( NULL == find )
                {
                    hw_printf(HW_ALLOC_VERBOSE,  "but is first found\n" );
                    find = ptr;
                    foundAddr = candidate_addr;
                }
                else if ( find->size > ptr->size )
                {
                    hw_printf(HW_ALLOC_VERBOSE,  "but is better fit\n" );
                    find = ptr;
                    foundAddr = candidate_addr;
                }
                else
                {
                    hw_printf(HW_ALLOC_VERBOSE,  "and is worse fit\n" );
                }
                // take it as we are focusing on speed for now
                break;
            }
            else
            {
                hw_errmsg( "%s: not exact fit, but size not greater\n",
                            __func__ );
                hw_errmsg( "\tcandidate_addr 0x%llx  ptr->addr  0x%llx  ptr->size 0x%llx\n",
                            candidate_addr, ptr->addr, ptr->size );
                hw_errmsg( "\tcandidate_addr + size 0x%llx  ptr->addr + ptr->size 0x%llx\n",
                            (candidate_addr + size), ptr->addr + ptr->size);
                return (blistNode*) NULL;
            }
        }
        else
        {
            hw_printf(HW_ALLOC_VERBOSE, "   -> doesn't meet ;\n");
        }
        
        ptr = findNextFreeNode(ptr->next);
    }
    
    if ( NULL == find )
    {
        if(hw_bAllocFailuresAreFatal)
        {
            hw_errmsg( "%s: no remaining free block satisfies "
                       "size 0x%llx  align pattern 0x%llx  align mask 0x%llx\n",
                        __func__, size, pattern, mask );
        }
        else
        {
            hw_critical( "WARNING: %s: no remaining free block satisfies "
                         "size 0x%llx  align pattern 0x%llx  align mask 0x%llx\n",
                          __func__, size, pattern, mask );
        }
        print_blocklist(block);
        
        find = NULL;
    }
    else
    {
        if ( find->size < size )
        {
            hw_errmsg( "%s: find->size 0x%llx < size 0x%llx\n",
                        __func__, find->size, size );
            find = NULL;
        }
        else if ( find->size == size )
        {
            hw_printf(HW_ALLOC_VERBOSE,  "%s: still looks like exact fit\n", __func__ );
        }
        
        //
        // If it's not an exact fit, split the block
        //
        else if ( find->size > size )
        {
            //
            // If the best-fit block start is unaligned,
            // split a little unaligned block off the front
            // so that the block to be returned will be aligned. 
            //
            if ( foundAddr > find->addr )
            {
                PTR_FIELD size_diff = foundAddr - find->addr;
                ptr = new_blist_node( foundAddr, find->size - size_diff, block );
                find->size = size_diff;
                insert_after( ptr, find );
                
                find = ptr;
            }
            
            //
            // If it's still not an exact fit (too big),
            // and it's enough too big that it's worth doing,
            // split the excess off the back into a new block
            //
            if ( find->size - size >= blockAlign )
            {
                ptr = new_blist_node(  ( find->addr + size), 
                                         find->size - size, block  );
                insert_after( ptr, find );
                find->size = size;
            }
        }
        
        // Internal consistency check. If it passes, we've
        // found our address. Mark it as taken.
        if ( ( find->addr & mask) != (pattern & mask) )
        {
            hw_errmsg( "%s: internal error, addr 0x%llx doesn't meet "
                        "alignment constraints\n", __func__, find->addr );
            hw_errmsg( "\taddr 0x%llx  align pattern 0x%llx  mask 0x%llx\n"
                        "\taddr & mask 0x%llx != align pattern & mask 0x%llx\n",
                        find->addr, pattern, mask, 
                        find->addr & mask, pattern & mask);
            find = NULL;
        }
        else
        {
            find->free = FALSE;
        }
    }
    
    return find;
    
}  /*  find_best_fit()  */

//--------------------------------------------
//
//          find_best_fit_identity()
//
//  Traverse the linked list of blocks;
//  find the smallest free block that will satisfy the request;
//  if too big, split the free block to achieve exact fit. 
//
//  Returns TRUE is a match was found and FALSE otherwise.
//  Upon return, foundNodes will contain pointers to the
//  found blocks if successful, and null pointers if unsuccessful.
//
//
BOOL
find_best_fit_identity( PTR_FIELD size, 
                        PTR_FIELD pattern, 
                        PTR_FIELD mask,
                        UINT32 numBlocks,
                        allocBlock** allocBlocks,
                        blistNode** foundNodes,
                        UINT32 blockAlign )
{
    hw_assert(numBlocks <= ALLOC_IDENTITY_MAX_NUM_LISTS);
    
    BOOL ret = TRUE;
    
    blistNode* ptr[ALLOC_IDENTITY_MAX_NUM_LISTS];
    PTR_FIELD  use_pat;
    PTR_FIELD  use_mask;
    PTR_FIELD  masked_pattern;
    PTR_FIELD  candidate_addr;
    
    UINT32 ctr;
    UINT32 lowestBlock = 0;
    PTR_FIELD lowestBlockEnd = ~0ll;
    PTR_FIELD highestBlockStart = 0;
    SIZE_FIELD effectiveSize;
    PTR_FIELD foundAddr = ~0ull;
    SIZE_FIELD foundSize = 0;
    
    for(ctr = 0; ctr < numBlocks; ctr++)
    {
        foundNodes[ctr] = NULL;
    }
    
    if ( 0 == mask )
    {
        use_pat  = 0;
        use_mask = blockAlign - 1;
    }
    else
    {
        use_pat  = pattern;
        use_mask = mask;
    }
    masked_pattern = use_pat & use_mask;
    
    // Initialize pointer variables.
    // When one or more pointers is NULL,
    // we're finished
    BOOL finished = FALSE;
    
    for(ctr = 0; ctr < numBlocks; ctr++)
    {
        if(NULL == (ptr[ctr] = findNextFreeNode(allocBlocks[ctr]->block_list)))
        {
            hw_printf(HW_ALLOC_DEBUG, "%s: Block %d has NULL block list.\n", __func__, ctr);
            finished = TRUE;
        }
        else
        {
            if(ptr[ctr]->addr + ptr[ctr]->size < lowestBlockEnd)
            {
                lowestBlockEnd = ptr[ctr]->addr + ptr[ctr]->size;
                lowestBlock = ctr;
            }
            
            if(ptr[ctr]->addr > highestBlockStart)
            {
                highestBlockStart = ptr[ctr]->addr;
            }
        }
    }
    
    while(!finished)
    {
        //
        // Only try to match an address if there
        // is non-zero overlap between the free blocks
        // we're considering.
        //
        hw_printf(HW_ALLOC_VERBOSE, "%s: Testing node set. highestBlockStart: 0x%llx lowestBlockEnd: 0x%llx\n", __func__, highestBlockStart, lowestBlockEnd);
        if(lowestBlockEnd > highestBlockStart)
        {
            effectiveSize = lowestBlockEnd - highestBlockStart;
            candidate_addr = matchCandidateAddr(highestBlockStart, masked_pattern, use_mask);
            
            //
            // If block contains a block that meets alignment constraints
            //
            if (      candidate_addr         >=  highestBlockStart
                 && ( candidate_addr + size) <= lowestBlockEnd)
            {
                //
                // if it's an exact fit, take it and bail out of list scan
                //
                if (    candidate_addr == highestBlockStart
                     && size           == effectiveSize   )
                {
                    hw_printf(HW_ALLOC_VERBOSE,  "\tExact fit\n" );
                    foundAddr = candidate_addr;
                    foundSize = effectiveSize;
                    for(ctr = 0; ctr < numBlocks; ctr++)
                    {
                        foundNodes[ctr] = ptr[ctr];
                    }
                    break;
                }
                
                //
                // If it's not an exact fit, but it's a better fit 
                // than the previous best, take it until we find a better. 
                //
                else if ( size < effectiveSize )
                {
                    hw_printf(HW_ALLOC_VERBOSE,  "   -> Meets, too big ;  " );
                    
                    if ( ~0ull == foundAddr )
                    {
                        hw_printf(HW_ALLOC_VERBOSE,  "but is first found\n" );
                        foundAddr = candidate_addr;
                        foundSize = effectiveSize;
                        for(ctr = 0; ctr < numBlocks; ctr++)
                        {
                            foundNodes[ctr] = ptr[ctr];
                        }
                    }
                    else if ( foundSize > effectiveSize )
                    {
                        hw_printf(HW_ALLOC_VERBOSE,  "but is better fit\n" );
                        foundAddr = candidate_addr;
                        foundSize = effectiveSize;
                        for(ctr = 0; ctr < numBlocks; ctr++)
                        {
                            foundNodes[ctr] = ptr[ctr];
                        }
                    }
                    else
                    {
                        hw_printf(HW_ALLOC_VERBOSE,  "and is worse fit\n" );
                    }
                    // take it as we are focusing on speed for now
                    break;
                }
                else
                {
                    hw_errmsg( "%s: not exact fit, but size not greater\n",
                                __func__ );
                    hw_errmsg( "\tcandidate_addr 0x%llx  highestBlockStart  0x%llx  effectiveSize 0x%llx\n",
                                candidate_addr, highestBlockStart, effectiveSize );
                    hw_errmsg( "\tcandidate_addr + size 0x%llx  lowestBlockEnd 0x%llx\n",
                                candidate_addr + size, lowestBlockEnd );
                    return FALSE;
                }
            }
            else
            {
                hw_printf(HW_ALLOC_VERBOSE, "   -> doesn't meet ;\n");
            }
        }
        else
        {
            hw_printf(HW_ALLOC_VERBOSE, "\tNo overlap between blocks.\n");
        }
        
        // Increment the lowest block and start the process
        // all over again.
        ptr[lowestBlock] = findNextFreeNode(ptr[lowestBlock]->next);
        
        // If the incremented block is NULL, we're done
        if(ptr[lowestBlock] == NULL)
        {
            finished = TRUE;
        }
        else
        {
            // Recalculate highest block
            if(ptr[lowestBlock]->addr > highestBlockStart)
            {
                highestBlockStart = ptr[lowestBlock]->addr;
            }
            
            // Recalculate lowest block
            lowestBlockEnd = ~0ll;
            for(ctr = 0; ctr < numBlocks; ctr++)
            {
                if(ptr[ctr]->addr + ptr[ctr]->size < lowestBlockEnd)
                {
                    lowestBlockEnd = ptr[ctr]->addr + ptr[ctr]->size;
                    lowestBlock = ctr;
                }
            }
        }
    }
    
    if ( ~0ull == foundAddr )
    {
        hw_errmsg( "%s: no remaining free block satisfies "
                    "size 0x%llx  align pattern 0x%llx  align mask 0x%llx\n",
                    __func__, size, pattern, mask );
        for(ctr = 0; ctr < numBlocks; ctr++)
        {
            hw_errmsg("Alloc block %d:\n", ctr);
            print_blocklist(allocBlocks[ctr]);
        }
        ret = FALSE;
    }
    else
    {
        if ( foundSize < size )
        {
            hw_errmsg( "%s: foundSize 0x%llx < size 0x%llx\n",
                        __func__, foundSize, size );
        }
        //
        // Split any blocks that need to be split
        //
        else
        {
            for(ctr = 0; ctr < numBlocks; ctr++)
            {
                //
                // If the best-fit block start is unaligned,
                // split a little unaligned block off the front
                // so that the block to be returned will be aligned. 
                //
                if ( foundAddr > foundNodes[ctr]->addr )
                {
                    PTR_FIELD size_diff = foundAddr - foundNodes[ctr]->addr;
                    ptr[ctr] = new_blist_node( foundAddr, foundNodes[ctr]->size - size_diff, allocBlocks[ctr] );
                    foundNodes[ctr]->size = size_diff;
                    insert_after( ptr[ctr], foundNodes[ctr] );
                    
                    foundNodes[ctr] = ptr[ctr];
                }
                
                //
                // If it's still not an exact fit (too big),
                // and it's enough too big that it's worth doing,
                // split the excess off the back into a new block
                //
                if ( foundNodes[ctr]->size - size >= blockAlign )
                {
                    ptr[ctr] = new_blist_node(  ( foundNodes[ctr]->addr + size), 
                                                  foundNodes[ctr]->size - size, allocBlocks[ctr]  );
                    insert_after( ptr[ctr], foundNodes[ctr] );
                    foundNodes[ctr]->size = size;
                }
                
                // Internal consistency check. If it passes, we've
                // found our address. Mark it as taken.
                if ( ( foundNodes[ctr]->addr & mask) != (pattern & mask) )
                {
                    hw_errmsg( "%s: internal error, addr 0x%llx doesn't meet "
                                "alignment constraints\n", __func__, foundNodes[ctr]->addr );
                    hw_errmsg( "\taddr 0x%llx  align pattern 0x%llx  mask 0x%llx\n"
                                "\taddr & mask 0x%llx != align pattern & mask 0x%llx\n",
                                foundNodes[ctr]->addr, pattern, mask, 
                                foundNodes[ctr]->addr & mask, pattern & mask);
                    foundNodes[ctr] = NULL;
                    
                    ret = FALSE;
                }
                else if(foundNodes[ctr]->addr != foundAddr)
                {
                    hw_errmsg( "%s: internal error, block addr 0x%llx doesn't match "
                                "common addr 0x%llx\n", __func__, foundNodes[ctr]->addr, foundAddr );
                    foundNodes[ctr] = NULL;
                    
                    ret = FALSE;
                }
                else
                {
                    foundNodes[ctr]->free = FALSE;
                }
            }
        }
    }
    
    return ret;
}  /*  find_best_fit_identity()  */

