/*
 *  Assembly functions to support context switching
 */

#include <xtensa/coreasm.h>

.text
.global arch_set_irql
.global arch_get_irql

.align 4
arch_get_irql:
    entry a1, 16
    movi a8, XCHAL_PS_INTLEVEL_MASK
    rsr.ps a2
    and a2,a2,a8
    retw

.align 4
// a2 = the IRQL you would like to set, only the low 4 bits are accepted
arch_set_irql:
    entry a1, 16
    // only keep low 4 bits of a2
    // a8 = 0xF, a9 = ~a8
    movi a8, XCHAL_PS_INTLEVEL_MASK
    movi a9, ~(XCHAL_PS_INTLEVEL_MASK)
    // keep lower 4 bits
    and a2,a2,a8
    // a10 = old ps, a11 = old irql
    rsr.ps a10
    and a11, a10, a8
    // clear the lower 4 bits of a10
    and a10, a10, a9
    // add with a2, the new irql
    add a10, a10, a2
    mov a2, a11
    wsr.ps a10
    retw




