#pragma once

#include "cdef.h"

 /**
 * boot_info structure
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

void KABI
kmain(struct boot_info *boot_info);
