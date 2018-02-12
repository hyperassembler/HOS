/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "kernel/ke/boot.h"
#include "kernel/ke/alloc.h"
#include "test/test_case.h"
#include "kernel/ke/print.h"

// returning from this function results in halting the cpu
void KABI ke_main(boot_info_t *boot_info)
{
    if (boot_info == NULL)
    {
        // failed.
        ke_printf("KERNEL: HAL init failed.\n");
        return;
    }

    // init kernel heap
    //sx_pmm_init(boot_info->pmm_info);

    ke_alloc_init();

//    ke_printf("KERNEL: Base Addr is 0x%X. Size is %uB, %uKB.\n",
//               boot_info->krnl_start,
//               (boot_info->krnl_end - boot_info->krnl_start),
//               (boot_info->krnl_end - boot_info->krnl_start) / 1024);

    ke_printf("KERNEL: CPU Vendor is \"%s\".\n", boot_info->cpu_vd_str);

    linked_list_test();

    avl_tree_test();

    salloc_test();

    ke_printf("KERNEL: Kernel tasks finished.\n");

    return;
}
