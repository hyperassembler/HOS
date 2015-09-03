#include "../common/sys/kdef.h"
#include "../hal/hal.h"
#include "../hal/print.h"
#include "../hal/io.h"

extern char kernel_start[];
extern char kernel_end[];
void NATIVE64 kmain(multiboot_info_t *multiboot_info)
{
    boot_info_t* boot_info = hal_init(multiboot_info);


    hal_printf("Available Memory: %uKB\n",boot_info->mem_info->mem_available / 1024);
    hal_printf("Reserved Memory: %uB\n",boot_info->mem_info->mem_reserved);
    hal_printf("Installed Memory: %uKB\n",(boot_info->mem_info->mem_reserved + boot_info->mem_info->mem_available)
                                          /1024);


    for(uint64_t i = 0; i <= 21; i++)
    {
        hal_set_interrupt_handler(i, hal_interrupt_handler_wrapper);
    }
    hal_enable_interrupt();

    hal_printf("KRNL: Kernel task finished.");
    hal_halt_cpu();
}