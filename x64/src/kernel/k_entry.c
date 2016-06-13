/* Copyright 2016 secXsQuared
 * Distributed under GPL license
 * See COPYING under root for details
 */

#include <k_sys_info.h>
#include "k_def.h"
#include "k_hal.h"
#include "std_lib.h"
#include "k_lib_test.h"

// returning from this function results in halting the cpu
void KAPI kmain(void *multiboot_info)
{
    k_hal_info_t* boot_info = hal_init(multiboot_info);

    if(boot_info == NULL)
    {
        hal_printf("HAL failed.\n");
        hal_halt_cpu();
    }

    hal_printf("Kernel Loaded at 0x%X. Size: %uB, %uKB\n\n",kernel_start,(kernel_end-kernel_start),(kernel_end-kernel_start)/1024);
    hal_printf("CPU Vendor:%s\n", boot_info->cpu_vendor_string);

    linked_list_test();
    avl_tree_test();
    salloc_test();

    hal_printf("KRNL: Kernel task finished.");
    hal_halt_cpu();

    if(boot_info->mem_info != NULL)
    {
        hal_printf("Installed Memory: %uB\n", boot_info->mem_info->mem_installed);
        hal_printf("Useable Memory: %uB\n", boot_info->mem_info->mem_available);
        for(int i = 0; i < linked_list_size(boot_info->mem_info->free_page_list); i++)
        {
            memory_descriptor_node_t* each_node = OBTAIN_STRUCT_ADDR(
            linked_list_get(boot_info->mem_info->free_page_list,i),
            list_node,
            memory_descriptor_node_t);

            hal_printf("Base Addr:0x%X Num of Pages:%u\n", each_node->base_addr, each_node->page_count);
        }
    }

    if(boot_info->module_info != NULL)
    {
        hal_printf("\nNumber of Modules Loaded: %u", boot_info->module_info->module_count);
        for (uint64_t i = 0; i <= boot_info->module_info->module_count; i++)
        {
            module_descriptor_node_t *module_descriptor = OBTAIN_STRUCT_ADDR(
                    linked_list_get(boot_info->module_info->module_list, (int) i),
                    list_node,
                    module_descriptor_node_t);
            hal_printf("\nModule Name: %s\n Module Addr: 0x%X\n Module Size: %uB, %uKB\n", module_descriptor->name,
                       module_descriptor->base_addr, module_descriptor->size, module_descriptor->size / 1024);
        };
    }

    hal_printf("KRNL: Kernel task finished.");
    hal_halt_cpu();
}
