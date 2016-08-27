/*-------------------------------------------------------
 |
 |    linux_tables.c
 |
 |    Contains heap/bss/ctor information for the linux platform
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2013  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#include "bifrost_private.h"

// The symbols in this table are defined by the linker script.
HW_HEAP_BLOCK_INFO hw_heapBlockInfo[HW_HEAP_BLOCK_CNT] __attribute__ ((section (".heap_table"))) = {
{HW_HEAP_DRAM_BASE,                          HW_HEAP_DRAM_AVAIL_SIZE},
};

