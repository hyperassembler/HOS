/*-------------------------------------------------------
 |
 |    ctor_table.c
 |
 |    Contains pointers to constructor tables.
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2015  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#include "bifrost_private.h"

// The symbols in this table are defined by the linker script.
UINTPTR hw_ctorInfo[6] __attribute__ ((section (".ctor_table"))) = {
// TODO: enable for sfp, vfp
1, // Entry count

(UINTPTR)&_ctor_table_end,
//(UINTPTR)&_sfp_ctor_table_end,
//(UINTPTR)&_vfp_ctor_table_end
};

