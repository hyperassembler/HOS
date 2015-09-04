#include "hal.h"
#include "print.h"
#include "mem.h"
#include "io.h"
#include "var.h"
#include "../common/lib/str.h"
#include "../common/lib/mem.h"

boot_info_t* NATIVE64 hal_init(multiboot_info_t* m_info)
{
    text_pos = get_pos(0, 0);

    // get gdt ready
    hal_write_segment_descriptor((void *) &g_gdt[0], 0, 0, 0);
    hal_write_segment_descriptor((void *) &g_gdt[8], 0, 0, SEG_DPL_0 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &g_gdt[16], 0, 0, SEG_DPL_0 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_DATA_RW);
    hal_write_segment_descriptor((void *) &g_gdt[24], 0, 0, SEG_DPL_3 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &g_gdt[32], 0, 0, SEG_DPL_3 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_DATA_RW);

    hal_write_segment_descriptor((void *) &g_gdt[40], 0, 0xFFFFF, SEG_DPL_0 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS | SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &g_gdt[48], 0, 0xFFFFF, SEG_DPL_0 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS | SEG_TYPE_DATA_RW);
    hal_write_segment_descriptor((void *) &g_gdt[56], 0, 0xFFFFF, SEG_DPL_3 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS | SEG_TYPE_CODE_X);
    hal_write_segment_descriptor((void *) &g_gdt[64], 0, 0xFFFFF, SEG_DPL_3 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS | SEG_TYPE_DATA_RW);
    g_gdt_ptr.base = (uint64_t)g_gdt;
    g_gdt_ptr.limit = 8*9-1;
    hal_flush_gdt(&g_gdt_ptr, SEG_SELECTOR(1, 0), SEG_SELECTOR(2, 0));

    // get idt ptr ready
    g_idt_ptr.base = (uint64_t)g_idt;
    g_idt_ptr.limit = 21*16-1;
    hal_flush_idt(&g_idt_ptr);

    boot_info_t* boot_info = (boot_info_t*)hal_halloc(sizeof(boot_info_t));
    hal_assert(boot_info != NULL, "Unable to allocate memory for boot_info.");
    mem_set(boot_info,0, sizeof(boot_info_t));
    // obtain boot information
    // memory info
    if(m_info->flags & (1 << 6))
    {
        boot_info->mem_info = (mem_info_t*)hal_halloc(sizeof(mem_info_t));
        hal_assert(boot_info->mem_info != NULL, "Unable to allocate memory for mem_info.");
        boot_info->mem_info->mem_available = 0;
        boot_info->mem_info->mem_reserved = 0;
        boot_info->mem_info->mem_seg_list = (linked_list_t*)hal_halloc((sizeof(linked_list_t)));
        hal_assert(boot_info->mem_info->mem_seg_list != NULL, "Unable to allocate memory for mem_seg_list.");
        linked_list_init(boot_info->mem_info->mem_seg_list);
        multiboot_memory_map_t const *mem_map = (multiboot_memory_map_t *) m_info->mmap_addr;
        uint64_t const mem_map_size = m_info->mmap_length / sizeof(multiboot_memory_map_t);
        for (int i = 0; i < mem_map_size; i++)
        {
            memory_descriptor_node_t* each_desc = (memory_descriptor_node_t*)hal_halloc(sizeof(memory_descriptor_node_t));
            hal_assert(each_desc != NULL, "Unable to allocate memory for memory_descriptor.");
            each_desc->base_addr = (mem_map + i)->addr;
            each_desc->size =  (mem_map + i)->len;
           if((mem_map + i)->type == MULTIBOOT_MEMORY_RESERVED)
            {

                boot_info->mem_info->mem_reserved += (mem_map + i)->len;
            }
            else if ((mem_map + i)->type == MULTIBOOT_MEMORY_AVAILABLE)
            {
                linked_list_add(boot_info->mem_info->mem_seg_list, &each_desc->list_node);
                boot_info->mem_info->mem_available += (mem_map + i)->len;
            }
        }
    }
    else
    {
        // halt machine
        hal_printf("HAL: Cannot detect memory information.");
        hal_halt_cpu();
    }

    // loaded kernel modules
    if(m_info->flags & (1 << 3))
    {
        boot_info->module_info = (module_info_t*)hal_halloc(sizeof(module_info_t));
        hal_assert(boot_info->module_info != NULL, "Unable to allocate memory for module_info.");
        boot_info->module_info->module_count = 0;
        boot_info->module_info->module_list = (linked_list_t*)hal_halloc(sizeof(linked_list_t));
        hal_assert(boot_info->module_info->module_list != NULL, "Unable to allocate memory for module_list.");
        linked_list_init(boot_info->module_info->module_list);
        multiboot_module_t const * mods_list = (multiboot_module_t *)m_info->mods_addr;
        boot_info->module_info->module_count = m_info->mods_count;
        for (uint64_t i = 0; i < boot_info->module_info->module_count; i++)
        {
            module_descriptor_node_t* each_module = (module_descriptor_node_t*)hal_halloc(sizeof(module_descriptor_node_t));
            hal_assert(each_module != NULL, "Unable to allocate memory for module_descriptor.");
            each_module->base_addr = (mods_list + i)->mod_start;
            each_module->size = (mods_list + i)->mod_end - (mods_list + i)->mod_start;
            each_module->name = (char*)hal_halloc((size_t)str_len((char *) (mods_list + i)->cmdline) + 1);
            hal_assert(each_module->name != NULL, "Unable to allocate memory for module name string.");
            mem_copy((void*)(mods_list + i)->cmdline, each_module->name, str_len((char *) (mods_list + i)->cmdline) + 1);
            linked_list_add(boot_info->module_info->module_list, &each_module->list_node);
        }
    }

    // detect APIC
    cpuid_t cpuid_info;
    cpuid_info.eax = 1;
    cpuid_info.ebx = 0;
    cpuid_info.ecx = 0;
    cpuid_info.edx = 0;
    hal_cpuid(&cpuid_info.eax,&cpuid_info.ebx,&cpuid_info.ecx,&cpuid_info.edx);
    if(cpuid_info.edx & 1 << 9)
    {
        //TODO: detected.
    }
    else
    {
        // halt machine
        hal_printf("HAL: Cannot detect APIC.");
        hal_halt_cpu();
    }

    return boot_info;
}