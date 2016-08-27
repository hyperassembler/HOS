/*-------------------------------------------------------
 |
 |    heap_table.c
 |
 |    Contains heap information, obtained by looking
 |    at symbols defined by the linker script.
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
{HW_HEAP_DRAM_BASE,                          HW_HEAP_DRAM_AVAIL_SIZE,              TRUE},
{(UINTPTR)&_global_ddr_protected_end,        (UINTPTR)&_ddr_protected_heap_size,   TRUE},
{(UINTPTR)&_global_sram0_wb_end,             (UINTPTR)&_sram0_heap_size,           TRUE},
{(UINTPTR)&_global_sram1_wb_end,             (UINTPTR)&_sram1_heap_size,           TRUE},
{(UINTPTR)&_global_sram2_wb_end,             (UINTPTR)&_sram2_heap_size,           TRUE},
{(UINTPTR)&_global_tcm_node0_sfp_data_end,   (UINTPTR)&_tcm_node0_sfp_heap_size,   FALSE},
{(UINTPTR)&_global_tcm_node1_sfp_data_end,   (UINTPTR)&_tcm_node1_sfp_heap_size,   FALSE},
{(UINTPTR)&_global_tcm_node2_sfp_data_end,   (UINTPTR)&_tcm_node2_sfp_heap_size,   FALSE},
{(UINTPTR)&_global_tcm_node3_sfp_data_end,   (UINTPTR)&_tcm_node3_sfp_heap_size,   FALSE},
{(UINTPTR)&_global_tcm_node4_sfp_data_end,   (UINTPTR)&_tcm_node4_sfp_heap_size,   FALSE},
{(UINTPTR)&_global_tcm_node5_sfp_data_end,   (UINTPTR)&_tcm_node5_sfp_heap_size,   FALSE},
{(UINTPTR)&_global_tcm_node6_sfp_data_end,   (UINTPTR)&_tcm_node6_sfp_heap_size,   FALSE},
{(UINTPTR)&_global_tcm_node7_vfp0_data_end,  (UINTPTR)&_tcm_node7_vfp0_heap_size,  FALSE},
{(UINTPTR)&_global_tcm_node7_vfp1_data_end,  (UINTPTR)&_tcm_node7_vfp1_heap_size,  FALSE},
{(UINTPTR)&_global_tcm_node8_vfp0_data_end,  (UINTPTR)&_tcm_node8_vfp0_heap_size,  FALSE},
{(UINTPTR)&_global_tcm_node8_vfp1_data_end,  (UINTPTR)&_tcm_node8_vfp1_heap_size,  FALSE},
{(UINTPTR)&_global_tcm_node9_vfp0_data_end,  (UINTPTR)&_tcm_node9_vfp0_heap_size,  FALSE},
{(UINTPTR)&_global_tcm_node9_vfp1_data_end,  (UINTPTR)&_tcm_node9_vfp1_heap_size,  FALSE},
{(UINTPTR)&_global_tcm_node10_vfp0_data_end, (UINTPTR)&_tcm_node10_vfp0_heap_size, FALSE},
{(UINTPTR)&_global_tcm_node10_vfp1_data_end, (UINTPTR)&_tcm_node10_vfp1_heap_size, FALSE},
{(UINTPTR)&_global_tcm_node11_vfp0_data_end, (UINTPTR)&_tcm_node11_vfp0_heap_size, FALSE},
{(UINTPTR)&_global_tcm_node11_vfp1_data_end, (UINTPTR)&_tcm_node11_vfp1_heap_size, FALSE},
{(UINTPTR)&_global_tcm_node12_vfp0_data_end, (UINTPTR)&_tcm_node12_vfp0_heap_size, FALSE},
{(UINTPTR)&_global_tcm_node12_vfp1_data_end, (UINTPTR)&_tcm_node12_vfp1_heap_size, FALSE},
{HW_HEAP_HOSTMEM_BASE,                       HW_HEAP_HOSTMEM_SIZE,                 TRUE}
};

