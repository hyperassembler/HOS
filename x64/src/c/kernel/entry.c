#include "../common/kdef.h"
#include "../hal/hal.h"
#include "../hal/print.h"
#include "../hal/io.h"

extern char kernel_start[];
extern char kernel_end[];
void NATIVE64 kmain(multiboot_info_t *multiboot_info)
{
    hal_init(multiboot_info);
    hal_printf("Finished setting up HAL\n");
    hal_enable_interrupt();
    while(1);
}