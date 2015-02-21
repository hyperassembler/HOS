#include "type.h"
#include "kdef.h"
#include "print.h"
#include "mem.h"
#include "multiboot.h"
uint8_t g_gdt[8*9];
gdt_ptr_t g_gdt_ptr;
extern uint64_t text_pos;
extern char kernel_start[];
extern char kernel_end[];
extern void NATIVE64 HLT_CPU(void);
extern void NATIVE64 BOCHS_MAGIC_BREAKPOINT();
extern void NATIVE64 hk_flush_gdt(gdt_ptr_t* gdt_ptr, uint64_t code_slct, uint64_t data_slct);
void NATIVE64 hk_main(multiboot_info_t* multiboot_info)
{
    text_pos = get_pos(3, 0);
    hk_printf("Kernel Start: 0x%X. End: 0x%X. Size: %dB, %dKB\n\n", (uint64_t)kernel_start, (uint64_t)kernel_end, (uint64_t)kernel_end - (uint64_t)kernel_start,((uint64_t)kernel_end - (uint64_t)kernel_start)/1024);
    hk_printf("*Setting up GDT...");
    hk_write_segment_descriptor((void*)&g_gdt[0], 0, 0, 0);
    hk_write_segment_descriptor((void*)&g_gdt[8], 0, 0, SEG_DPL_0 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_CODE_X);
    hk_write_segment_descriptor((void*)&g_gdt[16], 0, 0, SEG_DPL_0 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_DATA_RW);
    hk_write_segment_descriptor((void*)&g_gdt[24], 0, 0, SEG_DPL_3 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_CODE_X);
    hk_write_segment_descriptor((void*)&g_gdt[32], 0, 0, SEG_DPL_3 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_DATA_RW);

    hk_write_segment_descriptor((void*)&g_gdt[40], 0, 0xFFFFF, SEG_DPL_0 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS | SEG_TYPE_CODE_X);
    hk_write_segment_descriptor((void*)&g_gdt[48], 0, 0xFFFFF, SEG_DPL_0 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS | SEG_TYPE_DATA_RW);
    hk_write_segment_descriptor((void*)&g_gdt[56], 0, 0xFFFFF, SEG_DPL_3 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS | SEG_TYPE_CODE_X);
    hk_write_segment_descriptor((void*)&g_gdt[64], 0, 0xFFFFF, SEG_DPL_3 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS | SEG_TYPE_DATA_RW);

    g_gdt_ptr.base = (uint64_t)g_gdt;
    g_gdt_ptr.limit = 8*9-1;
    hk_flush_gdt(&g_gdt_ptr, SEG_SELECTOR(1, 0), SEG_SELECTOR(2, 0));
    hk_printf("Done.\n\n");

    hk_printf("*Checking memory information...\n");
    if(multiboot_info->flags & (1 << 6))
    {
        multiboot_memory_map_t const *mem_map = (multiboot_memory_map_t *) multiboot_info->mmap_addr;
        uint64_t const mem_map_size = multiboot_info->mmap_length / sizeof(multiboot_memory_map_t);
        hk_printf("BaseAddr  -  Length  -  Type\n");
        uint64_t total_available_mem = 0;
        uint64_t total_reserved_mem = 0;
        for (uint64_t i = 0; i < mem_map_size; i++)
        {
            hk_printf("0x%X  -  0x%X  -  0x%x\n",(mem_map + i)->addr,(mem_map + i)->len,(mem_map + i)->type);
            if((mem_map + i)->type == MULTIBOOT_MEMORY_RESERVED)
            {
                total_reserved_mem += (mem_map + i)->len;
            }
            else if ((mem_map + i)->type == MULTIBOOT_MEMORY_AVAILABLE)
            {
                total_available_mem += (mem_map + i)->len;
            }
        }
        hk_printf("Total available memory: %uB, %uKB, %uMB.\n",total_available_mem,total_available_mem/1024,total_available_mem/1024/1024);
        hk_printf("Total reserved memory: %uB, %uKB, %uMB.\n\n", total_reserved_mem, total_reserved_mem/1024, total_reserved_mem/1024/1024);
    }
    else
    {
        hk_printf("Memory information is currently unavailable.\n\n");
    }
    HLT_CPU();
}
