#pragma once

#include <arch/intr.h>
#include <kern/status.h>

/**
 * Interrupt context structure
 */
struct interrupt_context
{
    const uint64 rip;
    const uint64 cs;
    const uint64 rflags;
    const uint64 rsp;
    const uint64 ss;
};

/**
 * IDT Defns
 */
#define GATE_DPL_0 (0ull << 13)
#define GATE_DPL_1 (1ull << 13)
#define GATE_DPL_2 (2ull << 13)
#define GATE_DPL_3 (3ull << 13)
#define GATE_PRESENT (1ull << 15)
#define GATE_TYPE_CALL (12ull << 8)
#define GATE_TYPE_INTERRUPT (14ull << 8)
#define GATE_TYPE_TRAP (15ull << 8)

#define IDT_ENTRY_NUM 256
#define IDT_ENTRY_SIZE 16

#define IRQL_DPC (1)

/**
 * C declaration
 */
void
arch_intr_init(void);

/**
 * Exported Dispatchers for asm code
 */
void KABI
arch_intr_disp(uint64 int_vec, struct interrupt_context *context);

void KABI
arch_exc_disp(uint64 exc_vec, struct interrupt_context *context, uint32 errorcode);

/**
 * ASM declaration
 */
int
arch_raise_irql(int irql);

int
arch_lower_irql(int irql);

int
arch_get_irql();
