/*
 *  Assembly functions to support context switching
 */
#include <xtensa/cacheasm.h>

.text
.align 4

//
// do_global_ctors_ASM
//
// Call global constructors
//
// Arguments:
// [a2] table_start: address of the start of the global constructor info table
//
.global do_global_ctors_ASM
do_global_ctors_ASM:
    entry   a1, 32
    
    mov     a3, a2 // table start
    
    // Load entry count
    l32i    a4, a3, 0
    addi    a3, a3, 4
    
    beqz    a4, .L1zte

.L0zte:
    l32i    a5, a3, 0    // get start address, assumed multiple of 4
    addi    a3, a3, 4    // next entry
    
    addi    a5, a5, -8   // First entry is at symbol - 8
.L1zte:
    l32i    a6, a5, 0    // Load constructor pointer
    beqi    a6, -1, .L2zte // Value == -1 signals end of table
    callx8  a6           // Call constructor
    addi    a5, a5, -4   // Next entry
    j       .L1zte
.L2zte:
    
    addi    a4, a4, -1
    bnez    a4, .L0zte    // loop until end of table of constructor sections
.L3zte:
    
    // Synchronize memory access
    dcache_writeback_inv_all a7, a8
    memw
    
    retw

