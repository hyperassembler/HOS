#include <ke/cdef.h>
#include <arch/print.h>

// private headers
#include "multiboot2.h"

void arch_init(void* mb_info)
{
    arch_print_init();
    
    if (mb_info == NULL)
    {
        goto err;
    }

    char *cur_ptr = (char *) mb_info + 8;
    char *bootloader_name = NULL;

    while (1)
    {
        struct multiboot_tag *cur_tag = (struct multiboot_tag *) cur_ptr;
        switch (cur_tag->type)
        {
            case MULTIBOOT_TAG_TYPE_MMAP:
                hal_mem_init((struct multiboot_tag_mmap*) cur_ptr);
                break;
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                hal_print_init((struct multiboot_tag_framebuffer *) cur_ptr);
                break;
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                bootloader_name = ((struct multiboot_tag_string*)cur_ptr)->string;
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_NEW:
            default:
                break;
        }

        if (cur_tag->type == MULTIBOOT_TAG_TYPE_END)
        {
            break;
        }

        cur_ptr += cur_tag->size;
        cur_ptr = (char *) ALIGN(uintptr, cur_ptr, 8);
    }
    hal_halt_cpu();

    hal_printf("Boot loader:%d\n", bootloader_name);

    struct boot_info *boot_info = halloc(sizeof(struct boot_info));

//    // obtain cpu info
//    halp_obtain_cpu_info(boot_info);

    // init interrupt
    if (hal_interrupt_init() != 0)
    {
        hal_halt_cpu();
    }

    kmain(boot_info);
err:
    hal_halt_cpu();
}