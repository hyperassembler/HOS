#include "type.h"
#include "kdef.h"
#include "multiboot.h"
#include "mem.h"
#include "print.h"
uint8_t g_gdt[8 * 8];
gdt_ptr_t g_gdt_ptr;
uint8_t g_idt[8 * 256];
idt_ptr_t g_idt_ptr;

extern uint32_t text_pos;
extern void init_x64(multiboot_info_t* multiboot_info);
extern void BOCHS_MAGIC_BREAKPOINT(void);
extern void HLT_CPU(void);
extern char kernel_start[];
extern char kernel_end[];

void NATIVE32 kmain(multiboot_info_t *multiboot_info)
{
    //init text_position
    text_pos = 0;

    //detect architecture
    if (support_x64() == 1)
    {
        kprintf("Architecture x64.\n\n");
        init_x64(multiboot_info);
    }
    kprintf("Architecture x86.\n\n");
    kprintf("Kernel Loaded at 0x%X. Size: %uB, %uKB\n\n",kernel_start,(kernel_end-kernel_start),(kernel_end-kernel_start)/1024);
    //dummy descriptor
    write_segment_descriptor((void *) (&g_gdt[0]), 0, 0, 0);
    //ring 0 code seg, non-conforming
    write_segment_descriptor((void *) (&g_gdt[8]), 0, 0xFFFFF, SEG_TYPE_CODE_XR | SEG_CODE_DATA | SEG_32_BITS | SEG_GRANULARITY | SEG_PRESENT | SEG_DPL_0);
    //ring 3 code seg
    write_segment_descriptor((void *) (&g_gdt[16]), 0, 0xFFFFF, SEG_TYPE_CODE_XR | SEG_CODE_DATA | SEG_32_BITS | SEG_GRANULARITY | SEG_PRESENT | SEG_DPL_3);
    //ring 0 data RW
    write_segment_descriptor((void *) (&g_gdt[24]), 0, 0xFFFFF, SEG_TYPE_DATA_RW | SEG_CODE_DATA | SEG_32_BITS | SEG_GRANULARITY | SEG_PRESENT | SEG_DPL_0);
    //ring 3 data
    write_segment_descriptor((void *) (&g_gdt[32]), 0, 0xFFFFF, SEG_TYPE_DATA_RW | SEG_CODE_DATA | SEG_32_BITS | SEG_GRANULARITY | SEG_PRESENT | SEG_DPL_3);
    g_gdt_ptr.limit = 8 * 8 - 1;
    g_gdt_ptr.base = (uint32_t) g_gdt;
    load_gdt(&g_gdt_ptr, SEG_SELECTOR(1, 0), SEG_SELECTOR(3, 0));
    //check memory, definitely < 32 so we assume that
    kprintf("*Checking memory information...\n");
    if(multiboot_info->flags & (1 << 6))
    {
        multiboot_memory_map_t const *mem_map = (multiboot_memory_map_t *) multiboot_info->mmap_addr;
        uint32_t const mem_map_size = multiboot_info->mmap_length / sizeof(multiboot_memory_map_t);
        kprintf("BaseAddr  -  Length  -  Type\n");
        uint32_t total_available_mem = 0;
        uint32_t total_reserved_mem = 0;
        for (uint32_t i = 0; i < mem_map_size; i++)
        {
            kprintf("0x%x  -  0x%X  -  0x%x\n", ((uint32_t) (mem_map + i)->addr), (uint32_t) ((mem_map + i)->len), (uint32_t) (mem_map + i)->type);
            if((mem_map + i)->type == MULTIBOOT_MEMORY_RESERVED)
            {
                total_reserved_mem += (uint32_t) ((mem_map + i)->len);
            }
            else if ((mem_map + i)->type == MULTIBOOT_MEMORY_AVAILABLE)
            {
                total_available_mem += (uint32_t) ((mem_map + i)->len);
            }
        }
        kprintf("Total available memory: %uB, %uKB, %uMB.\n", total_available_mem, total_available_mem / 1024, total_available_mem / 1024 / 1024);
        kprintf("Total reserved memory: %uB, %uKB, %uMB.\n\n", total_reserved_mem, total_reserved_mem / 1024, total_reserved_mem / 1024 / 1024);
    }
    else
    {
        kprintf("Memory information is currently unavailable.\n\n");
    }

    //check modules
    kprintf("*Checking loaded kernel modules...\n");
    if(multiboot_info->flags & (1 << 3))
    {
        multiboot_module_t const * mods_list = (multiboot_module_t *)multiboot_info->mods_addr;
        uint32_t const mods_count = multiboot_info->mods_count;
        kprintf("%u module(s) loaded:\n", mods_count);
        kprintf("Name  -  StartAddr  -  EndAddr\n");
        for (uint64_t i = 0; i < mods_count; i++)
        {
            kprintf("%s  -  0x%X  -  0x%X\n", (char *) (mods_list + i)->cmdline, (mods_list + i)->mod_start, (mods_list + i)->mod_end);
        }
        kprintf("\n");
    }
    else
    {
        kprintf("Module information is currently unavailable.\n\n");
    }
    HLT_CPU();

    return;
}
