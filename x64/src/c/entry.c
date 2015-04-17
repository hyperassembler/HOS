#include "type.h"
#include "kdef.h"
#include "print.h"
#include "mm.h"
#include "multiboot.h"

uint8_t g_gdt[8*9];
gdt_ptr_t g_gdt_ptr;
extern uint64_t text_pos;
extern char kernel_start[];
extern char kernel_end[];
extern void NATIVE64 HLT_CPU(void);
extern void NATIVE64 BOCHS_MAGIC_BREAKPOINT();
void NATIVE64 kmain(multiboot_info_t *multiboot_info)
{
    text_pos = get_pos(3, 0);
    kprintf("Kernel Start: 0x%X. End: 0x%X. Size: %dB, %dKB\n\n", (uint64_t) kernel_start, (uint64_t) kernel_end, (uint64_t) kernel_end - (uint64_t) kernel_start, ((uint64_t) kernel_end - (uint64_t) kernel_start) / 1024);
    kprintf("*Setting up GDT...");
    write_segment_descriptor((void *) &g_gdt[0], 0, 0, 0);
    write_segment_descriptor((void *) &g_gdt[8], 0, 0, SEG_DPL_0 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_CODE_X);
    write_segment_descriptor((void *) &g_gdt[16], 0, 0, SEG_DPL_0 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_DATA_RW);
    write_segment_descriptor((void *) &g_gdt[24], 0, 0, SEG_DPL_3 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_CODE_X);
    write_segment_descriptor((void *) &g_gdt[32], 0, 0, SEG_DPL_3 | SEG_CODE_DATA | SEG_PRESENT | SEG_LONG | SEG_TYPE_DATA_RW);

    write_segment_descriptor((void *) &g_gdt[40], 0, 0xFFFFF, SEG_DPL_0 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS | SEG_TYPE_CODE_X);
    write_segment_descriptor((void *) &g_gdt[48], 0, 0xFFFFF, SEG_DPL_0 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS | SEG_TYPE_DATA_RW);
    write_segment_descriptor((void *) &g_gdt[56], 0, 0xFFFFF, SEG_DPL_3 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS | SEG_TYPE_CODE_X);
    write_segment_descriptor((void *) &g_gdt[64], 0, 0xFFFFF, SEG_DPL_3 | SEG_GRANULARITY | SEG_CODE_DATA | SEG_PRESENT | SEG_32_BITS | SEG_TYPE_DATA_RW);
    g_gdt_ptr.base = (uint64_t)g_gdt;
    g_gdt_ptr.limit = 8*9-1;
    flush_gdt(&g_gdt_ptr, SEG_SELECTOR(1, 0), SEG_SELECTOR(2, 0));
    kprintf("Done.\n\n");
    kprintf("*Checking memory information...\n");
    if(multiboot_info->flags & (1 << 6))
    {
        multiboot_memory_map_t const *mem_map = (multiboot_memory_map_t *) multiboot_info->mmap_addr;
        uint64_t const mem_map_size = multiboot_info->mmap_length / sizeof(multiboot_memory_map_t);
        kprintf("BaseAddr  -  Length  -  Type\n");
        uint64_t total_available_mem = 0;
        uint64_t total_reserved_mem = 0;
        for (int i = 0; i < mem_map_size; i++)
        {
            kprintf("0x%X  -  0x%X  -  0x%x\n", (mem_map + i)->addr, (mem_map + i)->len, (mem_map + i)->type);
            if((mem_map + i)->type == MULTIBOOT_MEMORY_RESERVED)
            {
                total_reserved_mem += (mem_map + i)->len;
            }
            else if ((mem_map + i)->type == MULTIBOOT_MEMORY_AVAILABLE)
            {
                total_available_mem += (mem_map + i)->len;
            }
        }
        kprintf("Total available memory: %uB, %uKB, %uMB.\n", total_available_mem, total_available_mem / 1024, total_available_mem / 1024 / 1024);
        kprintf("Total reserved memory: %uB, %uKB, %uMB.\n\n", total_reserved_mem, total_reserved_mem / 1024, total_reserved_mem / 1024 / 1024);
    }
    else
    {
        kprintf("Memory information is currently unavailable.\n\n");
    }
    kprintf("CPUIDing \n");
    cpuid_t cpuid_info;
    cpuid_info.eax = 1;
    cpuid_info.ebx = 0;
    cpuid_info.ecx = 0;
    cpuid_info.edx = 0;
    BOCHS_MAGIC_BREAKPOINT();
    cpuid(&cpuid_info.eax,&cpuid_info.ebx,&cpuid_info.ecx,&cpuid_info.edx);
    if(cpuid_info.edx & 1 << 9)
    {
        kprintf("AIPC detected...");
    }
    HLT_CPU();
}
