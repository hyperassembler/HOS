/*-------------------------------------------------------
 |
 |  bifrost_alloc.h
 |
 |  Contains functions and structs for Bifrost's
 |  various implementaions of alloc.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2011  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |-l-------------------------------------------------------
 */

#ifndef _BIFROST_ALLOC_H
#define _BIFROST_ALLOC_H

#include "bifrost_types.h"

void* hw_alloc(size_t size);

void hw_free(void* ptr);

void hw_alloc_setup();
//
//#define HW_ALLOC_NUM_BLOCKS             (0x2000 - 1)
//
//typedef struct
//{
//    uint64_t addr;
//    uint64_t size;
//    HW_HEAP_TYPE type;
//} HW_ALLOC_RECORD;
//
//typedef struct
//{
//    HW_ALLOC_RECORD records[HW_ALLOC_NUM_BLOCKS];
//    uint32_t next_unused;
//    uint32_t count;
//} __attribute__ ((packed,aligned(HW_MAX_CACHELINE_SIZE))) HW_ALLOC_TRACKER;
//
//#define ALLOC_NO_OWNER                (~0u)
//#define HW_INTERNAL_TESTID            (~1u)
//
//#define ALLOC_IDENTITY_MAX_NUM_LISTS  (3)
//
//#define HW_MASK_EXACT_ADDR            (0xFFFFFFFF)
//
////
//// RESERVED_MEM_BYTES indicates the number of bytes at the
//// beginning of each block which will not be handed out
//// as allocations.
////
//// Setting RESERVED_MEM_BYTES non-zero has the effect
//// of making the first successful allocation return a non-zero
//// buffer address, even if the block at address zero is free.
////
//// This is a Good Thing, because the allocator returns
//// zero to indicate failure. An error could result from
//// letting RESERVED_MEM_BYTES be 0, because allocation
//// could succeed and return 0 as the block address.
////
//#define RESERVED_MEM_BYTES            (0x1000)
//
//typedef struct blocklist_node_struct
//{
//    PTR_FIELD  addr;
//    SIZE_FIELD size;
//    UINT32     free;
//    UINT32     owner;
//    struct blocklist_node_struct* next;
//    struct blocklist_node_struct* prev;
//} blistNode;
//
//typedef struct
//{
//    blistNode* block_list;
//    blistNode block_array[ HW_ALLOC_NUM_BLOCKS + 1 ];
//    UINT32 next_unused;
//    PTR_FIELD  unusedAddr;
//    UINT8 rsvd[16]; // Pad out to cacheline size
//} __attribute__ ((aligned(HW_MAX_CACHELINE_SIZE))) allocBlock;
//
//extern allocBlock heapAllocBlock[HW_HEAP_BLOCK_CNT];
//extern HW_ERROR_CODE init_heap_blocklist();
//
//extern allocBlock hostPhysicalAllocBlock;
//extern HW_ERROR_CODE init_physical_blocklist();
//
////
//// Functions for printing alloc's internal
//// structures.
////
//extern void print_blocklist( allocBlock* block );
//extern void print_node( blistNode* np );
//
////
//// Functions for performing allocation
////
//extern HW_RESULT find_next_unused( UINT32* index, allocBlock* block );
//extern blistNode* new_blist_node( PTR_FIELD remote_addr,
//                                  PTR_FIELD size,
//                                  allocBlock* block );
//
//extern void insert_after( blistNode* item, blistNode* after );
//extern blistNode* merge_blocks( blistNode* b1, blistNode* b2 );
//extern blistNode* find_block( PTR_FIELD addr, allocBlock* block );
//extern blistNode* free_block( blistNode* bp, HW_TESTID testID );
//extern HW_ERROR_CODE free_all_blocks( HW_TESTID testID, allocBlock* block );
//
//extern blistNode* find_best_fit( PTR_FIELD size,
//                                 PTR_FIELD pattern,
//                                 PTR_FIELD mask,
//                                 allocBlock* block,
//                                 UINT32 blockAlign );
//
//extern BOOL find_best_fit_identity( PTR_FIELD size,
//                                    PTR_FIELD pattern,
//                                    PTR_FIELD mask,
//                                    UINT32 numBlocks,
//                                    allocBlock** allocBlocks,
//                                    blistNode** foundNodes,
//                                    UINT32 blockAlign );

#endif
