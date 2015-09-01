#include "hal.h"
#include "multiboot.h"
#include "print.h"
#include "mem.h"
#include "io.h"
#include "var.h"
#include "../common/util/list/linked_list/linked_list.h"
#include "../common/util/util.h"

void NATIVE64 hal_init(multiboot_info_t* m_info)
{
    text_pos = get_pos(3, 0);
    hal_printf("Setting up GDT...\n");
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

    if(m_info->flags & (1 << 6))
    {
        multiboot_memory_map_t const *mem_map = (multiboot_memory_map_t *) m_info->mmap_addr;
        uint64_t const mem_map_size = m_info->mmap_length / sizeof(multiboot_memory_map_t);
        hal_printf("Initializing memory descriptors...\n");
        uint64_t total_available_mem = 0;
        uint64_t total_reserved_mem = 0;
        for (int i = 0; i < mem_map_size; i++)
        {
            memory_descriptor_node_t* each_desc = (memory_descriptor_node_t*)hal_halloc(sizeof(memory_descriptor_node_t));
            each_desc->base_addr = (mem_map + i)->addr;
            each_desc->size =  (mem_map + i)->len;
            if((mem_map + i)->type == MULTIBOOT_MEMORY_RESERVED)
            {
                each_desc->type = MEMORY_RESERVED;
                total_reserved_mem += (mem_map + i)->len;
            }
            else if ((mem_map + i)->type == MULTIBOOT_MEMORY_AVAILABLE)
            {
                each_desc->type = MEMORY_AVAILABLE;
                total_available_mem += (mem_map + i)->len;
            }
            linked_list_add(&mem_desc, &each_desc->list_node);
        }
        // TODO: total RAM should be in memory descriptors list
        hal_printf("Total available memory: %uB, %uKB, %uMB.\n", total_available_mem, total_available_mem / 1024,
                   total_available_mem / 1024 / 1024);
        hal_printf("Total reserved memory: %uB, %uKB, %uMB.\n", total_reserved_mem, total_reserved_mem / 1024,
                   total_reserved_mem / 1024 / 1024);
        hal_printf("Memory Segments:\nBase - Size - Type\n");
        for(int i = 0; i < mem_desc.size; i++)
        {
            memory_descriptor_node_t* each_node = OBTAIN_STRUCT_ADDR(linked_list_get(&mem_desc,i), list_node, memory_descriptor_node_t);
            hal_printf("%X - %X - %s\n", each_node->base_addr, each_node->size, each_node->type == MEMORY_AVAILABLE ? "Available" : "Reserved");
        }
    }
    else
    {
        hal_printf("Memory information is currently unavailable.\n");
    }
    cpuid_t cpuid_info;
    cpuid_info.eax = 1;
    cpuid_info.ebx = 0;
    cpuid_info.ecx = 0;
    cpuid_info.edx = 0;
    hal_cpuid(&cpuid_info.eax,&cpuid_info.ebx,&cpuid_info.ecx,&cpuid_info.edx);
    if(cpuid_info.edx & 1 << 9)
    {
        hal_printf("APIC detected...\n");
    }


    for(uint64_t i = 0; i <= 21; i++)
    {
        hal_set_interrupt_handler(i, hal_interrupt_handler_wrapper);
    }
    g_idt_ptr.base = (uint64_t)g_idt;
    g_idt_ptr.limit = 21*16-1;
    hal_flush_idt(&g_idt_ptr);

    return;
}