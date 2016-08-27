/*
 *  Assembly functions to support context switching
 */
#include <xtensa/cacheasm.h>

.text
.align 4

//
// bss_clear_ASM
//
// Clears all BSS sections in cacheline-size chunks
//
// Arguments:
// [a2] table_start: address of the start of the BSS info table
//
.global bss_clear_ASM
bss_clear_ASM:
    entry   a1, 32
    
    movi    a5, 0
    
    mov     a6, a2 // table start
    
    // Load entry count
    l32i    a7, a6, 0
    addi    a6, a6, 4
    
    beqz    a7, .L5zte

.L0zte:
    l32i    a8, a6, 0    // get start address, assumed multiple of 4
    l32i    a9, a6, 4    // get end address, assumed multiple of 4
    addi    a6, a6, 8    // next entry
    sub     a10, a9, a8    // a10 = length, assumed a multiple of 4
    bbci.l  a10, 2, .L1zte
    s32i    a5, a8, 0    // clear 4 bytes to make length multiple of 8
    addi    a8, a8, 4
.L1zte:
    bbci.l  a10, 3, .L2zte
    s32i    a5, a8, 0    // clear 8 bytes to make length multiple of 16
    s32i    a5, a8, 4
    addi    a8, a8, 8
.L2zte:
    bbci.l  a10, 4, .L3zte
    s32i    a5, a8, 0    // clear 16 bytes to make length multiple of 32
    s32i    a5, a8, 4
    s32i    a5, a8, 8
    s32i    a5, a8, 12
    addi    a8, a8, 16
.L3zte:
    bbci.l  a10, 5, .L4zte
    s32i    a5, a8, 0    // clear 32 bytes to make length multiple of 64
    s32i    a5, a8, 4
    s32i    a5, a8, 8
    s32i    a5, a8, 12
    s32i    a5, a8, 16
    s32i    a5, a8, 20
    s32i    a5, a8, 24
    s32i    a5, a8, 28
    addi    a8, a8, 32
.L4zte:
    srli    a10, a10, 6    // length is now multiple of 64, divide by 64
    loopnez a10, .end_clearzte
    dpfwo   a8, 384
    s32i    a5, a8,     0    // clear 64 bytes at a time...
    s32i    a5, a8,     4
    s32i    a5, a8,     8
    s32i    a5, a8, 12
    s32i    a5, a8, 16
    s32i    a5, a8, 20
    s32i    a5, a8, 24
    s32i    a5, a8, 28
    s32i    a5, a8, 32
    s32i    a5, a8, 36
    s32i    a5, a8, 40
    s32i    a5, a8, 44
    s32i    a5, a8, 48
    s32i    a5, a8, 52
    s32i    a5, a8, 56
    s32i    a5, a8, 60
    addi    a8, a8, 64
.end_clearzte:
    
    addi    a7, a7, -1
    bnez    a7, .L0zte    // loop until end of table of *.bss sections
.L5zte:
    
    // Synchronize memory access
    dcache_writeback_inv_all a11, a12
    memw
    
    retw

