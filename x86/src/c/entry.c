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
extern void hk_entry_comp(void);

void HYPKERNEL32 hk_init_x86(multiboot_info_t const * const multiboot_info)
{
    hk_print_str("*Setting up GDT...");
    //dummy descriptor
    hk_write_segment_descriptor((void*)(&g_gdt[0]), 0, 0, 0);
    //ring 0 code seg, non-conforming
    hk_write_segment_descriptor((void*)(&g_gdt[8]), 0, 0xFFFFF, SEG_TYPE_CODE_XR | SEG_CODE_DATA | SEG_32_BITS | SEG_GRANULARITY | SEG_PRESENT | SEG_DPL_0);
    //ring 3 code seg
    hk_write_segment_descriptor((void*)(&g_gdt[16]), 0, 0xFFFFF, SEG_TYPE_CODE_XR | SEG_CODE_DATA | SEG_32_BITS | SEG_GRANULARITY | SEG_PRESENT | SEG_DPL_3);
    //ring 0 data RW
    hk_write_segment_descriptor((void*)(&g_gdt[24]), 0, 0xFFFFF, SEG_TYPE_DATA_RW | SEG_CODE_DATA | SEG_32_BITS | SEG_GRANULARITY | SEG_PRESENT | SEG_DPL_0);
    //ring 3 data
    hk_write_segment_descriptor((void*)(&g_gdt[32]), 0, 0xFFFFF, SEG_TYPE_DATA_RW | SEG_CODE_DATA | SEG_32_BITS | SEG_GRANULARITY | SEG_PRESENT | SEG_DPL_3);
    g_gdt_ptr.limit = 8 * 8 - 1;
    g_gdt_ptr.base = (uint32_t) g_gdt;
    hk_load_gdt(&g_gdt_ptr, SEG_SELECTOR(1, 0), SEG_SELECTOR(3, 0));
    hk_print_str(" - Done.\n\n");
    //check memory, definitely < 32 so we assume that
    hk_print_str("*Checking memory information...\n");
    if(multiboot_info->flags & (1 << 6))
    {
        multiboot_memory_map_t const *mem_map = (multiboot_memory_map_t *) multiboot_info->mmap_addr;
        uint32_t const mem_map_size = multiboot_info->mmap_length / sizeof(multiboot_memory_map_t);
        hk_print_str("BaseAddr  -  Length  -  Type\n");
        uint32_t total_available_mem = 0;
        uint32_t total_reserved_mem = 0;
        uint32_t i = 0;
        for (i = 0; i < mem_map_size; i++)
        {
            hk_print_hex((uint32_t)((mem_map + i)->addr));
            hk_print_str("  -  ");
            hk_print_hex((uint32_t)((mem_map + i)->len));
            hk_print_str("  -  ");
            hk_print_hex((mem_map + i)->type);
            hk_print_str("\n");
            if((mem_map + i)->type == MULTIBOOT_MEMORY_RESERVED)
            {
                total_reserved_mem += (uint32_t) ((mem_map + i)->len);
            }
            else if ((mem_map + i)->type == MULTIBOOT_MEMORY_AVAILABLE)
            {
                total_available_mem += (uint32_t) ((mem_map + i)->len);
            }
        }
        hk_print_str("Total available memory: ");
        hk_print_int(total_available_mem);
        hk_print_str("B = ");
        hk_print_int(total_available_mem/1024);
        hk_print_str("KB = ");
        hk_print_int(total_available_mem/1024/1024);
        hk_print_str("MB\n");
        hk_print_str("Total reserved memory: ");
        hk_print_int(total_reserved_mem);
        hk_print_str("B = ");
        hk_print_int(total_reserved_mem/1024);
        hk_print_str("KB = ");
        hk_print_int(total_reserved_mem/1024/1024);
        hk_print_str("MB\n\n");
    }
    else
    {
        hk_print_str("Memory information is currently unavailable.\n\n");
    }

    //check modules
    hk_print_str("*Checking loaded kernel modules...\n");
    if(multiboot_info->flags & (1 << 3))
    {
        multiboot_module_t const * mods_list = (multiboot_module_t *)multiboot_info->mods_addr;
        uint32_t const mods_count = multiboot_info->mods_count;
        hk_print_int(mods_count);
        hk_print_str(" module(s) loaded:\n");
        hk_print_str("Name  -  StartAddr  -  EndAddr\n");
        uint32_t i = 0;
        for (i = 0; i < mods_count; i++)
        {
            hk_print_str((char *) (mods_list + i)->cmdline);
            hk_print_str("  -  ");
            hk_print_hex((mods_list + i)->mod_start);
            hk_print_str("  -  ");
            hk_print_hex((mods_list + i)->mod_end);
            hk_print_str("\n");
        }
        hk_print_str("\n");
    }
    else
    {
        hk_print_str("Module information is currently unavailable.\n\n");
    }
    a:
        goto a;
}

void HYPKERNEL32 hk_init_x64(multiboot_info_t const * const multiboot_info)
{
    //CHECK MODULE AND LOAD ELF
    a:
        goto a;
}

void HYPKERNEL32 hk_main(multiboot_info_t const * const multiboot_info)
{
    //init text_position
    text_pos = 0;

    //detect architecture
    hk_print_str("*Checking architecture...\n");
    if (hk_support_x64() == 0)
    {
        hk_print_str("Arch: x86.\n\n");
        hk_init_x86(multiboot_info);
    }
    else
    {
        hk_print_str("Arch: x86_64.\n\n");
        hk_init_x64(multiboot_info);
    }
    return;
}
