/*-------------------------------------------------------
 |
 |    bss_table.c
 |
 |    Contains bss information, obtained by looking
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

//
// The symbols in this table are defined by the linker script.
//
// FORMAT:
// Entry count: 4 bytes
// <Entry 0>
// Start addr : 4 bytes
// End addr   : 4 bytes
// <Entry 1>
// ...
//
#ifdef BASRAM_ONLY
UINTPTR hw_bssInfo[] __attribute__ ((section (".bss_table"))) = {
6, // Entry count

(UINTPTR)&_bss_start + 0x20000000,
(UINTPTR)&_bss_end + 0x20000000,

(UINTPTR)&_ddr_wb_bss_start,
(UINTPTR)&_ddr_wb_bss_end,

(UINTPTR)&_sfp_bss_start + 0x20000000,
(UINTPTR)&_sfp_bss_end + 0x20000000,

(UINTPTR)&_vfp_bss_start + 0x20000000,
(UINTPTR)&_vfp_bss_end + 0x20000000,

(UINTPTR)&_bf_structs_start,
(UINTPTR)&_bf_structs_end,

(UINTPTR)&_global_sram0_wb_end,
(UINTPTR)&_extcom_text_start + 0x20000000,

};

#else
UINTPTR hw_bssInfo[] __attribute__ ((section (".bss_table"))) = {
5, // Entry count

(UINTPTR)&_bss_start - 0x60000000,
(UINTPTR)&_bss_end - 0x60000000,

(UINTPTR)&_ddr_wb_bss_start,
(UINTPTR)&_ddr_wb_bss_end,

(UINTPTR)&_sfp_bss_start - 0x60000000,
(UINTPTR)&_sfp_bss_end - 0x60000000,

(UINTPTR)&_vfp_bss_start - 0x60000000,
(UINTPTR)&_vfp_bss_end - 0x60000000,

(UINTPTR)&_bf_structs_start,
(UINTPTR)&_bf_structs_end,

};
#endif
