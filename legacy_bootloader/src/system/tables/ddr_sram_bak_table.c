/*-------------------------------------------------------
 |
 |    ddr_sram_bak_table.c
 |
 |    Contains sram data information, obtained by looking
 |    at symbols defined by the linker script.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2014  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#include "bifrost_private.h"

// The symbols in this table are defined by the linker script.
UINTPTR hw_ddr_sram_bak_info[2] __attribute__ ((section (".ddr_sram_bak_table"))) = {
    (UINTPTR)&_global_sram0_uc_start + 0x20000000,
    (UINTPTR)&_global_sram0_wb_end
};

