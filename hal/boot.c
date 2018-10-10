#include "mem.h"
#include "print.h"
#include "intr.h"
#include "cpu.h"
#include "kernel.h"
#include "hal.h"
#include "multiboot2.h"

//static void
//halp_obtain_cpu_info(struct boot_info *hal_info)
//{
//    if (hal_info == NULL)
//    {
//        return;
//    }
//    uint32 eax = 0, ebx = 0, ecx = 0, edx = 0;
//    hal_cpuid(&eax, &ebx, &ecx, &edx);
//    mem_cpy(&ebx, &hal_info->cpu_vd_str[0], sizeof(uint32));
//    mem_cpy(&edx, &hal_info->cpu_vd_str[4], sizeof(uint32));
//    mem_cpy(&ecx, &hal_info->cpu_vd_str[8], sizeof(uint32));
//    hal_info->cpu_vd_str[12] = 0;
//}

void HABI
hmain(struct multiboot_tag *mb_info)
{
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
