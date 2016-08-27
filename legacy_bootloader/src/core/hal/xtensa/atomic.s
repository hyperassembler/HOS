/*
 *  Assembly functions to support interlocked exchange
 */

.text

// a2 = int ptr
// a3 = val
.global arch_interlocked_compare_exchange
//uint32_t hw_interlocked_compare_exchange(uint32_t* addr, uint32_t compare, uint32_t val)

.align 4
arch_interlocked_compare_exchange:
    entry a1, 16
    wsr.scompare1 a3
    s32c1i a4, a2, 0
    mov a2, a4
    retw

