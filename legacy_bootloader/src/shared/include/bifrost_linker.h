/*-------------------------------------------------------
 |
 |    bifrost_linker.h
 |
 |    Contains externs of constants defined
 |    by the linker.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2013  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#ifndef _BIFROST_LINKER_H
#define _BIFROST_LINKER_H

extern UINT32 _global_sram0_uc_start;
extern UINT32 _global_sram1_uc_start;
extern UINT32 _global_sram2_uc_start;

extern UINT32 _global_sram0_wb_end;
extern UINT32 _global_sram1_wb_end;
extern UINT32 _global_sram2_wb_end;

extern UINT32 _sram0_heap_size;
extern UINT32 _sram1_heap_size;
extern UINT32 _sram2_heap_size;

extern UINT32 _global_ddr_protected_end;
extern UINT32 _ddr_protected_heap_size;

extern UINT32 _ddr_sram_bak_start;

extern UINT32 _tcm_node0_sfp_heap_size;
extern UINT32 _tcm_node1_sfp_heap_size;
extern UINT32 _tcm_node2_sfp_heap_size;
extern UINT32 _tcm_node3_sfp_heap_size;
extern UINT32 _tcm_node4_sfp_heap_size;
extern UINT32 _tcm_node5_sfp_heap_size;
extern UINT32 _tcm_node6_sfp_heap_size;
extern UINT32 _tcm_node7_vfp0_heap_size;
extern UINT32 _tcm_node7_vfp1_heap_size;
extern UINT32 _tcm_node8_vfp0_heap_size;
extern UINT32 _tcm_node8_vfp1_heap_size;
extern UINT32 _tcm_node9_vfp0_heap_size;
extern UINT32 _tcm_node9_vfp1_heap_size;
extern UINT32 _tcm_node10_vfp0_heap_size;
extern UINT32 _tcm_node10_vfp1_heap_size;
extern UINT32 _tcm_node11_vfp0_heap_size;
extern UINT32 _tcm_node11_vfp1_heap_size;
extern UINT32 _tcm_node12_vfp0_heap_size;
extern UINT32 _tcm_node12_vfp1_heap_size;

extern UINT32 _global_tcm_node0_sfp_data_end;
extern UINT32 _global_tcm_node1_sfp_data_end;
extern UINT32 _global_tcm_node2_sfp_data_end;
extern UINT32 _global_tcm_node3_sfp_data_end;
extern UINT32 _global_tcm_node4_sfp_data_end;
extern UINT32 _global_tcm_node5_sfp_data_end;
extern UINT32 _global_tcm_node6_sfp_data_end;
extern UINT32 _global_tcm_node7_vfp0_data_end;
extern UINT32 _global_tcm_node7_vfp1_data_end;
extern UINT32 _global_tcm_node8_vfp0_data_end;
extern UINT32 _global_tcm_node8_vfp1_data_end;
extern UINT32 _global_tcm_node9_vfp0_data_end;
extern UINT32 _global_tcm_node9_vfp1_data_end;
extern UINT32 _global_tcm_node10_vfp0_data_end;
extern UINT32 _global_tcm_node10_vfp1_data_end;
extern UINT32 _global_tcm_node11_vfp0_data_end;
extern UINT32 _global_tcm_node11_vfp1_data_end;
extern UINT32 _global_tcm_node12_vfp0_data_end;
extern UINT32 _global_tcm_node12_vfp1_data_end;

extern UINT32 _bss_start;
extern UINT32 _bss_end;
extern UINT32 _ddr_wb_bss_start;
extern UINT32 _ddr_wb_bss_end;
extern UINT32 _sfp_bss_start;
extern UINT32 _sfp_bss_end;
extern UINT32 _vfp_bss_start;
extern UINT32 _vfp_bss_end;
extern UINT32 _bf_structs_start;
extern UINT32 _bf_structs_end;
extern UINT32 _extcom_text_start;
extern UINT32 _ctor_table_end;
extern UINT32 _sfp_ctor_table_end;
extern UINT32 _vfp_ctor_table_end;

#endif // _BIFROST_LINKER_H

