/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include "k_alloc.h"
#include "k_lib_test.h"

// returning from this function results in halting the cpu
void KAPI kmain(void *multiboot_info)
{
    // init kernel heap
    k_alloc_init();

    k_boot_info_t* boot_info = (k_boot_info_t*)k_alloc(sizeof(boot_info));

    if(boot_info == NULL)
    {
        hal_printf("KERNEL: Unable to allocated memory for boot info struct.\n");
        hal_halt_cpu();
    }

    if(hal_init(multiboot_info, boot_info) != 0)
    {
        hal_printf("KERNEL: HAL initialization failed.\n");
        hal_halt_cpu();
    }

    hal_printf("KERNEL: Base Addr is 0x%X. Size is %uB, %uKB.\n",
               boot_info->krnl_start,
               (boot_info->krnl_end-boot_info->krnl_start),
               (boot_info->krnl_end-boot_info->krnl_start)/1024);

    hal_printf("KERNEL: CPU Vendor is \"%s\".\n", boot_info->cpu_vd_str);

    linked_list_test();
    avl_tree_test();
    salloc_test();

    hal_printf("KERNEL: Kernel tasks finished.\n");
    hal_halt_cpu();
}
