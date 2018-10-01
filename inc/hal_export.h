#pragma once

#include "common.h"

/**
 * HAL Structures
 */

/**
 * boot_info structure
 * must NOT use kernel structures
 */
struct boot_info
{
    struct
    {
        char cpu_vendor[13];
    } cpu_info;

    struct
    {
        uintptr krnl_start_vaddr;
        uintptr krnl_end_vaddr;
    } mem_info;

    struct intr_info
    {
        uint32 timer_intr_vec;
        uint32 dpc_intr_vec;
        uint32 page_fault_vec;

        uint32 irql_low;
        uint32 irql_dpc;
        uint32 irql_high;
    } intr_info;
};

/**
 * HAL functions
 */
int32 KABI
hal_atomic_xchg_32(int32 *target, int32 val);

int32 KABI
hal_atomic_inc_32(int32 *target, int32 increment);

int32 KABI
hal_atomic_cmpxchg_32(int32 *target, int32 compare, int32 val);

uint32 KABI
hal_set_irql(uint32 irql);

uint32 KABI
hal_get_irql(void);

void KABI
hal_halt(void);

void KABI
hal_issue_intr(uint32 core, uint32 vector);

typedef void (KABI *intr_handler_fp)(uintptr exc_addr, uintptr exc_stack, uint32 error_code);

void KABI
hal_reg_intr(uint32 index, intr_handler_fp handler);

void KABI
hal_dereg_intr(uint32 index);

typedef void (KABI *exc_handler_fp)(uintptr exc_addr, uintptr exc_stack, uint32 error_code);

void KABI
hal_reg_exc(uint32 exc, exc_handler_fp handler);

void KABI
hal_dereg_exc(uint32 exc);

uint32 KABI
hal_get_core_id(void);

void KABI
ke_main(struct boot_info *boot_info);
