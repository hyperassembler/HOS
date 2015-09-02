#include "hal.h"
#include "print.h"
#include "mem.h"
#include "io.h"
#include "var.h"
#include "../common/util/util.h"
#include "../common/sys/sys_info.h"
#include "../common/util/list/linked_list/linked_list.h"

boot_info_t* NATIVE64 hal_init(multiboot_info_t* m_info)
{
    text_pos = get_pos(3, 0);

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
    boot_info->mem_info = (mem_info_t*)hal_halloc(sizeof(mem_info_t));
    boot_info->mem_info->mem_available = 0;
    boot_info->mem_info->mem_reserved = 0;
    boot_info->mem_info->mem_seg_list = (linked_list_t*)hal_halloc((sizeof(linked_list_t)));
    boot_info->mem_info->mem_seg_list->head = NULL;
    boot_info->mem_info->mem_seg_list->size = 0;

    // obtain boot information

    // memory info
    if(m_info->flags & (1 << 6))
    {
        multiboot_memory_map_t const *mem_map = (multiboot_memory_map_t *) m_info->mmap_addr;
        uint64_t const mem_map_size = m_info->mmap_length / sizeof(multiboot_memory_map_t);
        for (int i = 0; i < mem_map_size; i++)
        {
            memory_descriptor_node_t* each_desc = (memory_descriptor_node_t*)hal_halloc(sizeof(memory_descriptor_node_t));
            each_desc->base_addr = (mem_map + i)->addr;
            each_desc->size =  (mem_map + i)->len;
            if((mem_map + i)->type == MULTIBOOT_MEMORY_RESERVED)
            {
                each_desc->type = MEMORY_RESERVED;
                boot_info->mem_info->mem_reserved += (mem_map + i)->len;
            }
            else if ((mem_map + i)->type == MULTIBOOT_MEMORY_AVAILABLE)
            {
                each_desc->type = MEMORY_AVAILABLE;
                boot_info->mem_info->mem_available += (mem_map + i)->len;
            }
            linked_list_add(boot_info->mem_info->mem_seg_list, &each_desc->list_node);
        }
    }
    else
    {
        // halt machine
        hal_printf("HAL: Cannot detect memory information.");
        hal_halt_cpu();
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