.text
.align 4

//
// sram_stall_ASM
//
// Stalls until ts_stall_flag for the core
// has been set to 1. This function is not
// callable from DRAM through windowed calling
// convention since the address is too far
// away--a callx0 instruction should be used.
//
// Arguments:
// [a2] flag_addr: the address to poll
// [a3] Stomped by this function
//
.global sram_stall_ASM
sram_stall_ASM:
    l8ui a3, a2, 0
    beqz a3, sram_stall_ASM
    ret

