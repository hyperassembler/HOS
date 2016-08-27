/*-------------------------------------------------------
 |
 |    stall.c
 |
 |    C implementation of stall functions for 'xtensa' architecture
 |
 |--------------------------------------------------------
 |
 |      Copyright ( C ) 2016  Microsoft Corp.
 |      All Rights Reserved
 |      Confidential and Proprietary
 |
 |--------------------------------------------------------
 */

#include "bifrost_private.h"

MEM_SRAM_UC volatile UINT8 sys_tsStallFlag[HW_PROC_CNT];

void sys_tsStall()
{
    asm volatile(
        "mov a4, a0\n"
        "movi a0, sram_stall_ASM\n"
        "mov a2, %[flag_addr]\n"
        "callx0 a0\n"
        "mov a0, a4\n"
        :
        : [flag_addr] "r" (&sys_tsStallFlag[hw_getCoreNum()])
        : "a0", "a2", "a3", "a4"
    );
}


