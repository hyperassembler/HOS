/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#ifndef _HAL_IO_H_
#define _HAL_IO_H_

#include "k_def.h"
#include "k_type.h"

#define GATE_DPL_0 (0ull << 13)
#define GATE_DPL_1 (1ull << 13)
#define GATE_DPL_2 (2ull << 13)
#define GATE_DPL_3 (3ull << 13)
#define GATE_PRESENT (1ull << 15)
#define GATE_TYPE_CALL (12ull << 8)
#define GATE_TYPE_INTERRUPT (14ull << 8)
#define GATE_TYPE_TRAP (15ull << 8)

#define IDT_ENTRY_NUM 40
#define IDT_ENTRY_SIZE 16

#define APIC_SPURIOUS_INT_VEC_REG_OFFSET 0xF0
#define APIC_LVT_CMCI_REG_OFFSET 0x2F0
#define APIC_LVT_TIMER_REG_OFFSET 0x320
#define APIC_LVT_THERMA_MONITOR_REG 0x330
#define APIC_LVT_PERFORMANCE_COUNTER_REG 0x340
#define APIC_LVT_LINT0_REG 0x350
#define APIC_LVT_LINT1_REG 0x360
#define APIC_LVT_ERROR_REG 0x370

// SYSTEM INTERRUPT HANDLERS
extern void hal_interrupt_handler_0(void);
extern void hal_interrupt_handler_1(void);
extern void hal_interrupt_handler_2(void);
extern void hal_interrupt_handler_3(void);
extern void hal_interrupt_handler_4(void);
extern void hal_interrupt_handler_5(void);
extern void hal_interrupt_handler_6(void);
extern void hal_interrupt_handler_7(void);
extern void hal_interrupt_handler_8(void);
extern void hal_interrupt_handler_10(void);
extern void hal_interrupt_handler_11(void);
extern void hal_interrupt_handler_12(void);
extern void hal_interrupt_handler_13(void);
extern void hal_interrupt_handler_14(void);
extern void hal_interrupt_handler_16(void);
extern void hal_interrupt_handler_17(void);
extern void hal_interrupt_handler_18(void);
extern void hal_interrupt_handler_19(void);
extern void hal_interrupt_handler_20(void);

// USER DEFINED INTERRUPT_HANDLERS
extern void hal_interrupt_handler_32(void);
extern void hal_interrupt_handler_33(void);
extern void hal_interrupt_handler_34(void);
extern void hal_interrupt_handler_35(void);
extern void hal_interrupt_handler_36(void);
extern void hal_interrupt_handler_37(void);
extern void hal_interrupt_handler_38(void);
extern void hal_interrupt_handler_39(void);

typedef struct
{
    const uint64_t rip;
    const uint64_t cs;
    const uint64_t rflags;
    const uint64_t rsp;
    const uint64_t ss;
} hal_intr_context_t;

void KAPI hal_register_interrupt_handler(uint64_t index,
                                         void (*handler)(uint64_t pc,
                                                         uint64_t sp,
                                                         uint64_t error));

void KAPI hal_deregister_interrupt_handler(uint64_t index);

void KAPI hal_set_interrupt_handler(uint64_t index, void (*handler)(void));

void KAPI hal_write_gate(void *const gate, uint64_t const offset, uint32_t const selector, uint32_t const attr);

void KAPI hal_assert(int64_t exp, char *message);

int32_t KAPI hal_interrupt_init(void);

#endif