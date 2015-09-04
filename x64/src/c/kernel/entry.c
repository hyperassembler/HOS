#include "../common/sys/kdef.h"
#include "../hal/hal.h"
#include "../hal/print.h"
#include "../hal/io.h"
#include "../common/util/util.h"

extern uint64_t kernel_start;
extern uint64_t kernel_end;
void NATIVE64 kmain(multiboot_info_t *multiboot_info)
{
    boot_info_t* boot_info = hal_init(multiboot_info);

    hal_printf("Kernel Loaded at 0x%X. Size: %uB, %uKB\n\n",kernel_start,(kernel_end-kernel_start),(kernel_end-kernel_start)/1024);

    hal_printf("Available Memory: %uKB\n",boot_info->mem_info->mem_available / 1024);
    hal_printf("Reserved Memory: %uB\n",boot_info->mem_info->mem_reserved);
    hal_printf("Installed Memory: %uKB\n",(boot_info->mem_info->mem_reserved + boot_info->mem_info->mem_available)
                                          /1024);

    hal_printf("\nNumber of Modules Loaded: %u", boot_info->module_info->module_count);
    for(uint64_t i = 0; i <= boot_info->module_info->module_count; i++)
    {
        module_descriptor_node_t* module_descriptor = OBTAIN_STRUCT_ADDR(linked_list_get(boot_info->module_info->module_list,(int)i),
                                                                         list_node,
                                                                         module_descriptor_node_t);
        hal_printf("\nModule Name: %s\n Module Addr: 0x%X\n Module Size: %uB, %uKB\n", module_descriptor->name, module_descriptor->base_addr, module_descriptor->size, module_descriptor->size / 1024);
    };

    for(uint64_t i = 0; i <= 21; i++)
    {
        hal_set_interrupt_handler(i, hal_interrupt_handler_wrapper);
    }
    hal_enable_interrupt();

    hal_printf("KRNL: Kernel task finished.");
    hal_halt_cpu();
}