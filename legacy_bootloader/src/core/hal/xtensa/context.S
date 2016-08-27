/*
 *  Assembly functions to support context switching
 */

#include "xtruntime-frames-custom.h"
#include <xtensa/coreasm.h>

.text

// a2 = context base
.global _arch_context_switch
_arch_context_switch:

    // set 4th bit of ps, i.e. EXCM = 1
    rsr.ps a15
    movi a14, 1 << XCHAL_PS_EXCM_SHIFT
    or a15, a15, a14
    wsr.ps a15

    // restore ps
    l32i a15, a2, UEXC_ps
    wsr.eps2 a15

    // restore pc
    l32i a15, a2, UEXC_pc
    wsr.epc2 a15

    // restore scompare
    l32i a15, a2, UEXC_scompare1
    wsr.scompare1 a15

    // restore misc01
    l32i a15, a2, UEXC_misc0
    wsr.misc0 a15
    l32i a15, a2, UEXC_misc1
    wsr.misc1 a15

    // restore sar
    l32i a15, a2, UEXC_sar
    wsr.sar a15

    // restore GPs
    l32i a0, a2, UEXC_a0
    l32i a1, a2, UEXC_a1
    l32i a3, a2, UEXC_a3
    l32i a4, a2, UEXC_a4
    l32i a5, a2, UEXC_a5
    l32i a6, a2, UEXC_a6
    l32i a7, a2, UEXC_a7
    l32i a8, a2, UEXC_a8
    l32i a9, a2, UEXC_a9
    l32i a10, a2, UEXC_a10
    l32i a11, a2, UEXC_a11
    l32i a12, a2, UEXC_a12
    l32i a13, a2, UEXC_a13
    l32i a14, a2, UEXC_a14
    l32i a15, a2, UEXC_a15
    l32i a2, a2, UEXC_a2

    rfi 2

