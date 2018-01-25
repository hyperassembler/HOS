/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "../arch/amd64/boot.h"
#include "pmm.h"
#include "alloc.h"
#include "test_case.h"

extern void KABI hal_printf(char const *, ...);

// returning from this function results in halting the cpu
void KABI ke_system_startup(void *boot_info)
{
    if (boot_info == NULL)
    {
        // failed.
        hal_printf("KERNEL: HAL init failed.\n");
        return;
    }

    // init kernel heap
    //sx_pmm_init(boot_info->pmm_info);

    ke_alloc_init();

    hal_printf("KERNEL: Base Addr is 0x%X. Size is %uB, %uKB.\n",
               boot_info->krnl_start,
               (boot_info->krnl_end - boot_info->krnl_start),
               (boot_info->krnl_end - boot_info->krnl_start) / 1024);

    hal_printf("KERNEL: CPU Vendor is \"%s\".\n", boot_info->cpu_vd_str);

    linked_list_test();

    avl_tree_test();

    salloc_test();

    hal_printf("KERNEL: Kernel tasks finished.\n");

    return;
}
