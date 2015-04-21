#include "../common/kdef.h"
#include "../common/type.h"
#include "../hal/hal.h"

extern char kernel_start[];
extern char kernel_end[];
void NATIVE64 kmain(multiboot_info_t *multiboot_info)
{
    hal_init(multiboot_info);
    hal_printf("Finished setting up HAL\n");
    HLT_CPU();
}
